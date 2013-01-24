/*
 *      EMP: Explicit Multithread Package
 *
 *      Copyright 2013, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#ifdef WIN32

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <process.h>


static long
EMP_getseconds()
{
  SYSTEMTIME    systime;
  FILETIME      filtime;
  LARGE_INTEGER large;
  DWORDLONG     int64;

  GetSystemTime(&systime);
  SystemTimeToFileTime(&systime, &filtime);
  large.LowPart  = filtime.dwLowDateTime;
  large.HighPart = filtime.dwHighDateTime;
  int64          = large.QuadPart/10000000;
  return int64 & 0x7FFFFFFF;
}


/* initialize the EMP block */

int EMP_Init(long *start)
{
  int         np;
  char        *env;
  SYSTEM_INFO siSysInfo;

  GetSystemInfo(&siSysInfo);
  np = siSysInfo.dwNumberOfProcessors;

  env = getenv("EMPnumProc");
  if (env != NULL) {
    np = atoi(env);
    if (np < 1) np = 1;
  }
  if (start != NULL) *start = EMP_getseconds();
  return np;
}


/* close the EMP block */

long EMP_Done(long *start)
{
  if (start == NULL) return 0L;
  return EMP_getseconds() - *start;
}


/* Waste a little time */

void EMP_ThreadSpin()
{
  Sleep(1);
}


/* Get the current Thread ID */

long EMP_ThreadID()
{
  return GetCurrentThreadId();
}


/* Spawn off another thread */

HANDLE *EMP_ThreadCreate(void (*entry)(void *), void *arg)
{
  HANDLE   *thread;
  unsigned  threadID;

  thread = (HANDLE *) malloc(sizeof(HANDLE));
  if (thread == NULL) return NULL;

  *thread = (HANDLE) _beginthreadex(NULL, 0, 
				    (unsigned int (__stdcall *)(void *)) entry,
                                    arg,  0, &threadID);
  return thread;
}


/* Ends the thread execution with the return code */

void EMP_ThreadExit()
{
  _endthreadex(0);
}


/* Wait for the thread to finish */

void EMP_ThreadWait(HANDLE *thread)
{
  if (WaitForSingleObject(*thread, INFINITE) == WAIT_FAILED)
    printf(" Warning: ThreadWait FAILED!\n");
}


/* Destroy the thread memory */

void EMP_ThreadDestroy(HANDLE *thread)
{
  CloseHandle(*thread);
  free(thread);
}


/* Create a lock (unlocked) */

HANDLE *EMP_LockCreate()
{
  HANDLE *mutex;

  mutex  = (HANDLE *) malloc(sizeof(HANDLE));
  if (mutex == NULL) return NULL;
  *mutex = CreateMutex(NULL, FALSE, NULL);
  if (*mutex == NULL) {
    printf(" ERROR: MUTEX not assigned (LockCreate)!\n");
    free(mutex);
    return NULL;
  }
  return mutex;
}


/* Destroy the lock memory */

void EMP_LockDestroy(HANDLE *mutex)
{
  ReleaseMutex(*mutex);
  free(mutex);
}


/* Set a lock (wait if already set) */

void EMP_LockSet(HANDLE *mutex)
{
  if (WaitForSingleObject(*mutex, INFINITE) == WAIT_FAILED)
    printf(" Warning: LockSet Wait FAILED!\n");
}


/* Gets the value of a lock (0-unset, 1-set) */

int EMP_LockTest(HANDLE *mutex)
{
  DWORD stat;

  stat = WaitForSingleObject(*mutex, 0L);
  if (stat == WAIT_FAILED) {
    return 1;
  } else {
    ReleaseMutex(*mutex);
    return 0;
  }
}


/* Release a lock */

void EMP_LockRelease(HANDLE *mutex)
{
  if (ReleaseMutex(*mutex) == 0)
    printf(" Warning: LockRelease Unlock FAILED!\n");
}


#else


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>


static long
EMP_getseconds()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec;
}


/* Initialize the EMP block */

int EMP_Init(/*@null@*/ long *start)
{
  char *env;
  long nprocs = 1;

#ifdef _SC_NPROCESSORS_ONLN
  nprocs = sysconf(_SC_NPROCESSORS_ONLN);
  if (nprocs < 1)
    printf("Could not determine number of CPUs online:\n%s\n", strerror(errno));
#else
  printf(stderr, "Could not determine number of CPUs");
#endif

  env = getenv("EMPnumProc");
  if (env != NULL) {
    nprocs = atoi(env);
    if (nprocs < 1) nprocs = 1;
  }
  if (start != NULL) *start = EMP_getseconds();
  return nprocs;
}


/* Close the EMP block */

long EMP_Done(long *start)
{
  if (start == NULL) return 0L;
  return EMP_getseconds() - *start;
}


/* Waste a little time -- yeild */

void EMP_ThreadSpin()
{
  usleep(1000);
}


/* Get the current Thread ID */

long EMP_ThreadID()
{
  return (long) pthread_self();
}


/* Spawn off another thread */

/*@null@*/ void *EMP_ThreadCreate(void (*entry)(void *), /*@null@*/ void *arg)
{
  int            stat;
  pthread_t      *thread;
  pthread_attr_t attr;

  thread = (pthread_t *) malloc(sizeof(pthread_t));
  if (thread == NULL) return NULL;

  pthread_attr_init(&attr);
  stat = pthread_create(thread, &attr, (void * (*) (void *)) entry, arg);
  if (stat != 0) {
    free(thread);
    return NULL;
  }

  return (void *) thread;
}


/* Ends the thread execution */

void EMP_ThreadExit()
{
  pthread_exit(NULL);
}


/* Wait for the thread to finish */

void EMP_ThreadWait(void *vthread)
{
  int       stat;
  pthread_t *thread;

  thread = (pthread_t *) vthread;
  stat   = pthread_join(*thread, NULL);
  if (stat != 0)
    printf(" Threading ERROR: %d (ThreadWait)\n", stat);
}


/* Destroy the thread memory */

void EMP_ThreadDestroy(/*@only@*/ void *vthread)
{
  pthread_t *thread;

  thread = (pthread_t *) vthread;
  pthread_detach(*thread);
  free(vthread);
}


/* Create a lock (unlocked) */

/*@null@*/ void *EMP_LockCreate()
{
  int             stat;
  pthread_mutex_t *mutex;

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  if (mutex != NULL) {
    stat = pthread_mutex_init(mutex, NULL);
    if (stat != 0) {
      printf(" Threading ERROR: %d (LockCreate)\n", stat);
      free(mutex);
      return NULL;
    }
  }
  return (void *) mutex;
}


/* Destroy the lock memory */

void EMP_LockDestroy(/*@only@*/ void *vlock)
{
  pthread_mutex_t *lock;

  lock = (pthread_mutex_t *) vlock;
  pthread_mutex_unlock(lock);
  free(lock);
}


/* Set a lock (wait if already set) */

void EMP_LockSet(void *vlock)
{
  int             stat;
  pthread_mutex_t *lock;

  lock = (pthread_mutex_t *) vlock;
  stat = pthread_mutex_lock(lock);
  if (stat != 0) printf(" Threading Warning: %d (LockSet)\n", stat);
}


/* Gets the value of a lock (0-unset, 1-set) */

int EMP_LockTest(void *vlock)
{
  int             stat;
  pthread_mutex_t *lock;

  lock = (pthread_mutex_t *) vlock;
  stat = pthread_mutex_trylock(lock);
  if (stat == EBUSY) {
    return 1;
  } else if (stat == 0) {
    pthread_mutex_unlock(lock);
    return 0;
  } else {
    printf(" Fatal Threading ERROR: %d (LockTest)\n", stat);
    exit(EXIT_FAILURE);
  }
}


/* Release a lock */

void EMP_LockRelease(void *vlock)
{
  pthread_mutex_t *lock;

  lock = (pthread_mutex_t *) vlock;
  pthread_mutex_unlock(lock);
}
#endif


/* structure to hold control info for EMP_for, EMP_sum, and EMP_min */

typedef struct {
  int    nindex;                    /* number of indices */
  int    index;                     /* current loop index */
  int    status;                    /* status flag */
  long   masterID;                  /* master thread ID */
  void   *mutex;                    /* the mutex or NULL for single thread */
  int    (*forFn)(int);             /* routine that does work in EMP_for */
  int    (*sumFn)(int, double *);   /* routine that does work in EMP_sum */
  double sum;                       /* global sum */
  int    (*minFn)(int, double *);   /* routine that does work for EMP_min */
  double min;                       /* global minimum */
  int    imin;                      /* index associated with minimum (or -1) */
} emp_T;


/* Inner routine used by EMP_for */

static void EMP_for_inner(void *Global)
{
  int   index, status;
  long  ID;
  emp_T *global;

  global = (emp_T *)Global;

  /* get our identifier */
  ID = EMP_ThreadID();

  /* loop as long as work remains and status==0 */
  while (1) {

    /* only one thread at a time here - controlled by a mutex */
    if (global->mutex != NULL ) EMP_LockSet(global->mutex);

    status        = global->status;
    index         = global->index;
    global->index = index + 1;

    if (global->mutex != NULL) EMP_LockRelease(global->mutex);

    /* check that status is still 0 */
    if (status != 0) break;

    /* check that there is really work to be done */
    if (index >= global->nindex) break;

    /* do the work */
    status = global->forFn(index);

    /* if an error was found, save status (inside a mutex) and return */
    if (status != 0) {
      if (global->mutex != NULL) EMP_LockSet(global->mutex);

      global->status = status;

      if (global->mutex != NULL) EMP_LockRelease(global->mutex);

      break;
    }
  }

  /* finished all work, so exit */
  if (ID != global->masterID) EMP_ThreadExit();
}


/* Simple parallel "for" loop */

int EMP_for(int maxproc, int nindex, int (*forFn)(int index))
{
  int   i, np;
  long  start;
  void  **threads = NULL;
  emp_T global;

  /* set up structure to hold parallelization info */
  global.nindex   = nindex;
  global.index    = 0;
  global.status   = 0;

  global.masterID = EMP_ThreadID();
  global.mutex    = NULL;

  global.forFn    = forFn;

  global.sumFn    = NULL;
  global.sum      = 0;

  global.minFn    = NULL;
  global.min      = 0;
  global.imin     = -1;

  /* np is limited by user-given number of processors */
  np = EMP_Init(&start);
  if (np > maxproc) np = maxproc;

  /* 
   * create the mutex to handle list synchronization and
   * get storage for the extra threads 
   */
  if (np > 1) {
    global.mutex = EMP_LockCreate();
    if (global.mutex == NULL) {
      np = 1;
    } else {
      threads = (void **) malloc((np-1)*sizeof(void *));
      if (threads == NULL) {
        EMP_LockDestroy(global.mutex);
        np = 1;
      }
    }
  }

  /* create the threads and execute the inner function in them */
  if (threads != NULL)
    for (i = 0; i < np-1; i++) {
      threads[i] = EMP_ThreadCreate(EMP_for_inner, &global);
      if (threads[i] == NULL) printf(" Error creating thread %d\n", i+1);
    }

  /* now run the inner routine from the original thread */
  EMP_for_inner(&global);

  /* wait for all others to return */
  if (threads != NULL)
    for (i = 0; i < np-1; i++)
      if (threads[i] != NULL) EMP_ThreadWait(threads[i]);

  /* cleanup */
  if (threads != NULL)
    for (i = 0; i < np-1; i++)
      if (threads[i] != NULL) EMP_ThreadDestroy(threads[i]);

  if (global.mutex != NULL) EMP_LockDestroy(global.mutex);
  if (threads      != NULL) free(threads);

  /* close the EMP package (for now) */
  EMP_Done(&start);

  return global.status;
}


/* Inner routine used by EMP_sum */

static void EMP_sum_inner(void *Global)
{
  int    index, status;
  long   ID;
  double sum;
  emp_T  *global;

  global = (emp_T *)Global;

  /* get our identifier */
  ID = EMP_ThreadID();

  /* loop as long as work remains and status==0 */
  while (1) {

    /* only one thread at a time here - controlled by a mutex */
    if (global->mutex != NULL) EMP_LockSet(global->mutex);

    status        = global->status;
    index         = global->index;
    global->index = index + 1;

    if (global->mutex != NULL) EMP_LockRelease(global->mutex);

    /* check that status is still 0 */
    if (status != 0) break;

    /* check that there is really work to be done */
    if (index >= global->nindex) break;

    /* do the work */
    status = global->sumFn(index, &sum);

    /* if an error was found, save status (inside a mutex) and return */
    if (status != 0) {
      if (global->mutex != NULL) EMP_LockSet(global->mutex);

      global->status = status;

      if (global->mutex != NULL) EMP_LockRelease(global->mutex);

      break;
    }

    /* add the sum to the global sum (inside a mutex) */
    if (global->mutex != NULL) EMP_LockSet(global->mutex);

    global->sum += sum;

    if (global->mutex != NULL) EMP_LockRelease(global->mutex);
  }

  /* finished all work, so exit */
  if (ID != global->masterID) EMP_ThreadExit();

}

/* Parallel loop to find a sum */

int EMP_sum(int maxproc, int nindex, int (*sumFn)(int index, double *sum),
            double *sum)
{
  int   i, np;
  long  start;
  void  **threads = NULL;
  emp_T global;

  /* set up structure to hold parallelization info */
  global.nindex   = nindex;
  global.index    = 0;
  global.status   = 0;

  global.masterID = EMP_ThreadID();
  global.mutex    = NULL;

  global.forFn    = NULL;

  global.sumFn    = sumFn;
  global.sum      = 0;

  global.minFn    = NULL;
  global.min      = 0;
  global.imin     = -1;

  /* np is limited by user-given number of processors */
  np = EMP_Init(&start);
  if (np > maxproc) np = maxproc;

  /* 
   * create the mutex to handle list synchronization and
   * get storage for the extra threads 
   */
  if (np > 1) {
    global.mutex = EMP_LockCreate();
    if (global.mutex == NULL) {
      np = 1;
    } else {
      threads = (void **) malloc((np-1)*sizeof(void *));
      if (threads == NULL) {
        EMP_LockDestroy(global.mutex);
        np = 1;
      }
    }
  }

  /* create the threads and execute the inner routine in them */
  if (threads != NULL)
    for (i = 0; i < np-1; i++) {
      threads[i] = EMP_ThreadCreate(EMP_sum_inner, &global);
      if (threads[i] == NULL) printf(" Error creating thread %d\n", i+1);
    }

  /* now run the inner routine from the original thread */
  EMP_sum_inner(&global);

  /* wait for all others to return */
  if (threads != NULL)
    for (i = 0; i < np-1; i++)
      if (threads[i] != NULL) EMP_ThreadWait(threads[i]);

  /* cleanup */
  if (threads != NULL) 
    for (i = 0; i < np-1; i++)
      if (threads[i] != NULL) EMP_ThreadDestroy(threads[i]);

  if (global.mutex != NULL) EMP_LockDestroy(global.mutex);
  if (threads != NULL) free(threads);

  /* close the EMP package (for now) */
  EMP_Done(&start);

  *sum = global.sum;
  return global.status;
}


/* Inner routine used by EMP_min */

static void EMP_min_inner(void *Global)
{
  int    index, status;
  long   ID;
  double min;
  emp_T  *global;

  global = (emp_T *)Global;

  /* get our identifier */
  ID = EMP_ThreadID();

  /* loop as long as work remains and status==0 */
  while (1) {

    /* only one thread at a time here - controlled by a mutex */
    if (global->mutex != NULL) EMP_LockSet(global->mutex);

    status        = global->status;
    index         = global->index;
    min           = global->min;
    global->index = index + 1;

    if (global->mutex != NULL) EMP_LockRelease(global->mutex);

    /* check that status is still 0 */
    if (status != 0) break;

    /* check that there is really work to be done */
    if (index >= global->nindex) break;

    /* do the work */
    status = global->minFn(index, &min);

    /* if an error was found, save status (inside a mutex) and return */
    if (status != 0) {
      if (global->mutex != NULL) EMP_LockSet(global->mutex);

      global->status = status;

      if (global->mutex != NULL) EMP_LockRelease(global->mutex);

      break;
    }

    /* remember this min if small than global min (inside a mutex) */
    if (global->mutex != NULL) EMP_LockSet(global->mutex);

    if (min < global->min) {
        global->min  = min;
        global->imin = index;
    }

    if (global->mutex != NULL) EMP_LockRelease(global->mutex);
  }

  /* finished all work, so exit */
  if (ID != global->masterID) EMP_ThreadExit();

}


/* Parallel loop to find a minimum */

int EMP_min(int maxproc, int nindex, int (*minFn)(int index, double *min),
            double *min, int *imin)
{
  int   i, np;
  long  start;
  void  **threads = NULL;
  emp_T global;

  /* set up structure to hold parallelization info */
  global.nindex   = nindex;
  global.index    = 0;
  global.status   = 0;

  global.masterID = EMP_ThreadID();
  global.mutex    = NULL;

  global.forFn    = NULL;

  global.sumFn    = NULL;
  global.sum      = 0;

  global.minFn    = minFn;
  global.min      = *min;
  global.imin     = -1;

  /* np is limited by user-given number of processors */
  np = EMP_Init(&start);
  if (np > maxproc) np = maxproc;

  /* 
   * create the mutex to handle list synchronization and
   * get storage for the extra threads 
   */
  if (np > 1) {
    global.mutex = EMP_LockCreate();
    if (global.mutex == NULL) {
      np = 1;
    } else {
      threads = (void **) malloc((np-1)*sizeof(void *));
      if (threads == NULL) {
        EMP_LockDestroy(global.mutex);
        np = 1;
      }
    }
  }

  /* create the threads and execute the inner routine in them */
  if (threads != NULL)
    for (i = 0; i < np-1; i++) {
      threads[i] = EMP_ThreadCreate(EMP_min_inner, &global);
      if (threads[i] == NULL) printf(" Error creating thread %d\n", i+1);
    }

  /* now run the inner routine from the original thread */
  EMP_min_inner(&global);

  /* wait for all others to return */
  if (threads != NULL)
    for (i = 0; i < np-1; i++)
      if (threads[i] != NULL) EMP_ThreadWait(threads[i]);

  /* cleanup */
  if (threads != NULL) 
    for (i = 0; i < np-1; i++)
      if (threads[i] != NULL) EMP_ThreadDestroy(threads[i]);

  if (global.mutex != NULL) EMP_LockDestroy(global.mutex);
  if (threads != NULL) free(threads);

  /* close the EMP package (for now) */
  EMP_Done(&start);

  *min  = global.min;
  *imin = global.imin;
  return global.status;
}
