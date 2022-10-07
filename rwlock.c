#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
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
	pthread_mutex_init(&l->mutex,NULL);
	pthread_cond_init(&l->r_cond, NULL);
	l->r_active = 0;
	l->r_wait = 0;
	for (int i=0; i<3; i++)
	{
		pthread_cond_init(&l->w_cond[i], NULL);
		l->w_active[i] = 0;
		l->w_wait[i] = 0;
	}
}

typedef enum{false, true} bool;

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
	int rc = pthread_cond_broadcast(cond);
	assert (rc == 0);
}

bool check_all_zeros(int arr[3]){
	for (int i=0; i<3; i++){
		if (arr[i] != 0){
			return false;
		}
	}
	return true;
}


//rwl_rlock attempts to grab the lock in "read" mode
void
rwl_rlock(rwl *l)
{
	Pthread_mutex_lock(&l->mutex);
	// if any writer active/writer waiting, wait for reading condition
	while (!check_all_zeros(l->w_active) || !check_all_zeros(l->w_wait)){
		l->r_wait++; 
		Pthread_cond_wait(&l->r_cond, &l->mutex); //put self to sleep, onto r_cond queue
	}
	// othersies, successfully grab and read; 
	l->r_active++; //increment active reader count
	Pthread_mutex_unlock(&l->mutex);
}

int highest_wait_priority(rwl *l){
	for (int i=0; i<3; i++){
		if (l->w_wait[i]){
			return i;
		}
	}
	return -1;
}

//rwl_runlock unlocks the lock held in the "read" mode
void
rwl_runlock(rwl *l)
{
	Pthread_mutex_lock(&l->mutex);
	assert(check_all_zeros(l->w_active)); // &&"shouldn't have active writer at runlock"
	// decrement active reader count
	l->r_active--;
	if (l->r_active == 0) {
		int highest_wait = highest_wait_priority(l);
		l->w_wait[highest_wait] = 0;
		Pthread_cond_broadcast(&l->w_cond[highest_wait]);
	}
	Pthread_mutex_unlock(&l->mutex);
}



bool check_can_write(rwl *l, int priority){
	for (int i=0; i<priority; i++){
		if (l->w_wait[i]){
			return false;
		}
	}
	for (int i=0; i<3; i++){
		if (l->w_active[i]){
			return false;
		}
	}
	if (l->r_active){
		return false;
	}
	return true;
}

//rwl_wlock attempts to grab the lock in "write" mode
void
rwl_wlock(rwl *l, int priority)
{			
	Pthread_mutex_lock(&l->mutex);
	// while higher priority writer waiting, or any writer active, 
	// or any active reader
	// put self to respective priority w_wait & w_cond queue
	while (!check_can_write(l, priority)){
	    l->w_wait[priority]++;
		Pthread_cond_wait(&l->w_cond[priority], &l->mutex);
	}
	// successfully grab and set active
	l->w_active[priority]++; //increment active writer count
	Pthread_mutex_unlock(&l->mutex);
}



//rwl_wunlock unlocks the lock held in the "write" mode
void
rwl_wunlock(rwl *l, int priority)
{
	Pthread_mutex_lock(&l->mutex);
	assert(l->w_active[0]+l->w_active[1]+l->w_active[2]==1 ); //&&"must have one active writer at wunlock"
	assert(l->r_active==0); // &&"shouldn't have active reader at wunlock"
	l->w_active[priority]--;   // decrement corresponding active writer count

	int highest_wait = highest_wait_priority(l);

	if (highest_wait > -1){
		l->w_wait[highest_wait] = 0;
		Pthread_cond_broadcast(&l->w_cond[highest_wait]);
	} else {
		l->r_wait = 0;
		Pthread_cond_broadcast(&l->r_cond);
	}
	Pthread_mutex_unlock(&l->mutex);
}