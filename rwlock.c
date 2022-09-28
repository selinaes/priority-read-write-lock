#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include "rwlock.h"

/* rwl implements a reader-writer lock.
 * A reader-write lock can be acquired in two different modes, 
 * the "read" (also referred to as "shared") mode,
 * and the "write" (also referred to as "exclusive") mode.
 * Many threads can grab the lock in the "read" mode.  
 * By contrast, if one thread has acquired the lock in "write" mode, no other 
 * threads can acquire the lock in either "read" or "write" mode.
 */



//rwl_init initializes the reader-writer lock 
void
rwl_init(rwl *l)
{
	// initialization of read/write lock
}



//rwl_rlock attempts to grab the lock in "read" mode
void
rwl_rlock(rwl *l)
{
	// your code here
}


//rwl_runlock unlocks the lock held in the "read" mode
void
rwl_runlock(rwl *l)
{
	// your code here
}


//rwl_wlock attempts to grab the lock in "write" mode
void
rwl_wlock(rwl *l, int priority)
{			
	// your code here
}

//rwl_wunlock unlocks the lock held in the "write" mode
void
rwl_wunlock(rwl *l, int priority)
{
	// your code here
}