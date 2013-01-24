/*
 *      EMP: Explicit Multithread Package
 *
 *          simple test code
 *
 *      Copyright 2013, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef WIN32
#define sleep(x) Sleep(x*1000)
#else
#include <unistd.h>
#endif

#include "emp.h"


/* building this tester ->
 *
 * on Windows:   cl  ThreadTest.c /I../include emp.c /DWIN32 /MD
 *
 * on Linux/Mac: gcc ThreadTest.c -I../include emp.c -lpthread -lm
 *
 */


/* structure to pass data to the thread for a block */

  typedef struct {
    void *mutex;      /* the mutex or NULL for single thread */
    long master;      /* master thread ID */
    int  end;         /* end of loop */
    int  index;       /* current loop index */
    int  *work;       /* the data to work upon */
  } EMPdata;


/* our work function -- just sleep and report */

static void
doWork(long ID, int index, int sleeps)
{
  sleep(sleeps);
  printf("%lX: doWork %d %d!\n", ID, index, sleeps);
}

/* test functions for EMP_for and EMP_sum */

static int
testFor(int index)
{
  int work[20] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5,
                  1, 5, 3, 4, 2, 5, 4, 3, 2, 1};

  sleep(work[index]);
  printf("in testFor(index=%2d)  waited %2d sec\n", index, work[index]);

  /* index=18 throws an error to test error processing mechanism */
  if (index == 18) {
      return -1234;
  } else {
      return 0;
  }
}

static int
testSum(int index, double *sum)
{
  int work[20] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5,
                  1, 5, 3, 4, 2, 5, 4, 3, 2, 1};

  sleep(work[index]);

  *sum = work[index];
  printf("in testSum(index=%2d)  returned sum=%5.2f\n", index, *sum);

  return 0;
}

static int
testMin(int index, double *min)
{
  int       i, j, k, imax, jmax;
  double    dtest, x = 222.2, y = 333.3, z = 44.4;

  if (index%4 == 0) {
      imax = jmax = 10000;
  } else {
      imax = jmax =  1000;
  }

  k = index;
  for (j = 0; j < jmax; j++) {
    for (i = 0; i < imax; i++) {
      dtest = sqrt((x-i)*(x-i) + (y-j)*(y-j) + (z-k)*(z-k));
      if (dtest < *min) *min = dtest;
    }
  }

  printf("in testMin(index=%3d)  returned min=%f\n", index, *min);

  /* index=60 throws an error to test error processing mechanism */
  if (index == 60) {
    return 2;
  } else {
    return 0;
  }
}

/*
 * the high-level function which runs in each thread
 *     this parcels out work by looping through the work
 */

static void
testFn(void *global)
{
  int     index;
  long    ID;
  EMPdata *work;

  work = (EMPdata *) global;

  /* get our identifier */
  ID = EMP_ThreadID();

  /* look for work */
  for (;;) {

    /* only one thread at a time here -- controlled by a mutex! */
    if (work->mutex != NULL) EMP_LockSet(work->mutex);
    index = work->index;
    work->index = index+1;
    if (work->mutex != NULL) EMP_LockRelease(work->mutex);
    if (index >= work->end) break;

    /* do the work */
    doWork(ID, index, work->work[index]);

  }

  /* exhausted all work -- exit */
  if (ID != work->master) EMP_ThreadExit();
}


int
main(/*@unused@*/ int argc, /*@unused@*/ char *argv[])
{
    int        i, np, status, imin;
  long       start;
  double     sum, min;
  void       **threads = NULL;
  EMPdata    global;
  static int work[20] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5,
                         1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

  global.mutex  = NULL;
  global.index  = 0;
  global.work   = work;
  global.end    = 20;
  global.master = EMP_ThreadID();

  np = EMP_Init(&start);
  printf(" NumProcs = %d!\n\n", np);

  if (np > 1) {
    /* create the mutex to handle list synchronization */
    global.mutex = EMP_LockCreate();
    if (global.mutex == NULL) {
      printf(" mutex creation = NULL!\n");
      np = 1;
    } else {
      /* get storage for our extra threads */
      threads = (void **) malloc((np-1)*sizeof(void *));
      if (threads == NULL) {
        EMP_LockDestroy(global.mutex);
        np = 1;
      }
    }
  }

  /* create the threads and get going! */
  if (threads != NULL)
    for (i = 0; i < np-1; i++) {
      threads[i] = EMP_ThreadCreate(testFn, &global);
      if (threads[i] == NULL) printf(" Error Creating Thread #%d!\n", i+1);
    }
  /* now run the thread block from the original thread */
  testFn(&global);

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

  /* report the time */
  printf("\n Number of Seconds on Thread Block = %ld\n\n", EMP_Done(&start));

  /* test EMP_for with as many processors as are available (up to 99) */
  status = EMP_for(99, 20, testFor);
  printf("\nEMP_for(99,20) returned status=%d (should be -1234)\n\n", status);

  /* test EMP_for with only 1 processor */
  status = EMP_for( 1, 10, testFor);
  printf("\nEMP_for( 1,10) returned status=%d (should be 0)\n\n", status);

  /* test EMP_sum with as many processors as are available (up to 99) */
  status = EMP_sum(99, 20, testSum, &sum);
  printf("\nEMP_sum(99,20) returned status=%d (should be 0) and sum=%5.2f (should be 60)\n\n", status, sum);

  /* test EMP_sum with only one processor */
  status = EMP_sum( 1, 10, testSum, &sum);
  printf("\nEMP_sum(1, 10) returned status=%d (should be 0) and sum=%5.2f (should be 30)\n\n", status, sum);

  /* test EMP_min with as many processor as are available (up to 99) */
  min = 1.0e+20;
  status = EMP_min(99, 100, testMin, &min, &imin);
  printf("\nEMP_min(99,100) returned status=%d (should be 2) and min=%10.5f (should be 0.53852) at imin=%2d (should be 44)\n\n", status, min, imin);

  /* test EMP_min with only one processor */
  min = 1.0e+20;
  status = EMP_min( 1, 100, testMin, &min, &imin);
  printf("\nEMP_min( 1,100) returned status=%d (should be 2) and min=%10.5f (should be 0.53852) at imin=%2d (should be 44)\n\n", status, min, imin);

  return 0;
}
