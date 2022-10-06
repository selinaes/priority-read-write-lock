#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>

typedef struct {
	pthread_mutex_t     mutex;
	pthread_cond_t      r_cond;
	pthread_cond_t      w_cond[3];
	int                 r_active;
	int                 w_active[3];
	int                 r_wait;
	int                 w_wait[3];
}rwl;

void rwl_init(rwl *l);
void rwl_rlock(rwl *l);
void rwl_runlock(rwl *l);
void rwl_wlock(rwl *l, int priority);
void rwl_wunlock(rwl *l, int priority);

#endif