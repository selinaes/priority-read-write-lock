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
	l->mutex = PTHREAD_MUTEX_INITIALIZER;
	l->r_cond = PTHREAD_COND_INITIALIZER;
	l->r_active = 0;
	l->r_wait = 0;
	for (i=0; i<3; i++)
	{
		l->w_cond[i] = PTHREAD_COND_INITIALIZER;
		l->w_active[i] = 0;
		l->w_wait[i] = 0;
	}
}

void Pthread_mutex_lock(pthread_mutex_t *mutex){
	int rc = pthread_mutex_lock(mutex);
	assert (rc == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex){
	int rc = pthread_mutex_unlock(mutex);
	assert (rc == 0);
}

void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	int rc = pthread_cond_wait(cond, mutex);
	assert (rc == 0);
}

void Pthread_cond_broadcast(pthread_cond_t *cond){
	int rc = Pthread_cond_broadcast(cond);
	assert (rc == 0);
}

//rwl_rlock attempts to grab the lock in "read" mode
void
rwl_rlock(rwl *l)
{
	Pthread_mutex_lock(&l->mutex);
	// if any writer active, wait for reading condition (which writing thread will change)
	while (!memcmp(l->w_active,[0,0,0],3)==0){
		l->r_wait++; //感觉可能有问题，比如被错误唤醒之后，又加了一次自己
		Pthread_cond_wait(&l->r_cond, &l->mutex); //put self to sleep, onto r_cond queue
	}
	// othersies, successfully grab and read; 
	l->r_wait--; //decrement waiting reader count
	l->r_active++; //increment active reader count
	Pthread_mutex_unlock(&l->mutex);
}


//rwl_runlock unlocks the lock held in the "read" mode
void
rwl_runlock(rwl *l)
{
	Pthread_mutex_lock(&l->mutex);
	// decrement active reader count
	l->r_active--;
	if (l->r_active == 0) {
		for (i=0; i<3; i++){
			Pthread_cond_broadcast(&l->w_cond[i]);
		}
	}
	Pthread_mutex_unlock(&l->mutex);
}




//rwl_wlock attempts to grab the lock in "write" mode
void
rwl_wlock(rwl *l, int priority)
{			
	Pthread_mutex_lock(&l->mutex);
	// while higher priority writer active/waiting, put self to respective priority w_wait & w_cond queue
	for (i=0; i<priority; i++){
		if ()
	}



	while (memcmp(l->w_active,[0,0,0],3)==0){
	    l->w_wait[priority]++;
		Pthread_cond_wait(&l->w_cond[priority], &l->mutex);
	}
	// successfully grab and set active
	l->w_wait[priority]--; //decrement waiting writer count
	l->w_active[priority]++; //increment active writer count
	// wait for active readers to complete reading
	while (l->r_active){
		Pthread_cond_wait(&l->w_cond[priority], &l->mutex);
	}
	Pthread_mutex_unlock(&l->mutex);
}

//rwl_wunlock unlocks the lock held in the "write" mode
void
rwl_wunlock(rwl *l, int priority)
{
	Pthread_mutex_lock(&l->mutex);
	l->w_active[priority]--;   // decrement corresponding active writer count
	if (memcmp(l->w_active,[0,0,0],3)==0){ // if no write active now, broadcast
		for (i=0; i<3; i++){
			Pthread_cond_broadcast(&l->w_cond[i]);
		}
		Pthread_cond_broadcast(&l->r_cond);
	}
	Pthread_mutex_unlock(&l->mutex);
}