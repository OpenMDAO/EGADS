/*
 *      EMP: Explicit Multithread Package
 *
 *             Function Header
 *
 *      Copyright 2013, Massachusetts Institute of Technology
 *      Licensed under The GNU Lesser General Public License, version 2.1
 *      See http://www.opensource.org/licenses/lgpl-2.1.php
 *
 */

#ifndef _EMP_H_

#define _EMP_H_

#ifdef __ProtoGlarp__
#undef __ProtoGlarp__
#endif
#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32)
#define __ProtoGlarp__(x) x
#else
#define __ProtoGlarp__(x) ()
#endif

#ifdef __cplusplus
extern "C" {
#endif

           int   EMP_Init          __ProtoGlarp__(( /*@null@*/ long *start ));
           long  EMP_Done          __ProtoGlarp__(( /*@null@*/ long *start ));

/*@null@*/ void *EMP_ThreadCreate  __ProtoGlarp__(( void (*entry)(void *),
                                                    /*@null@*/ void *arg ));
           void  EMP_ThreadExit    __ProtoGlarp__((  ));
           void  EMP_ThreadWait    __ProtoGlarp__(( void *thread ));
           void  EMP_ThreadSpin    __ProtoGlarp__((  ));
           long  EMP_ThreadID      __ProtoGlarp__((  ));
           void  EMP_ThreadDestroy __ProtoGlarp__(( /*@only@*/ void *thread ));

/*@null@*/ void *EMP_LockCreate    __ProtoGlarp__(( ));
           void  EMP_LockSet       __ProtoGlarp__(( void *lock ));
           int   EMP_LockTest      __ProtoGlarp__(( void *lock ));
           void  EMP_LockRelease   __ProtoGlarp__(( void *lock ));
           void  EMP_LockDestroy   __ProtoGlarp__(( /*@only@*/ void *lock ));

           int   EMP_for           __ProtoGlarp__(( int maxproc, int nindex,
                                                    int (*forFn)(int index) ));
           int   EMP_sum           __ProtoGlarp__(( int maxproc, int nindex,
                                                    int (*sumFn)(int index,
                                                                 double *sum),
                                                    double *sum));
           int   EMP_min           __ProtoGlarp__(( int maxproc, int nindex,
                                                    int (*minFn)(int index,
                                                                 double *min),
                                                    double *min, int *imin));

#ifdef __cplusplus
}
#endif

#endif  /*_EMP_H_*/

