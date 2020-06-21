#ifndef _mutexh
#define _mutexh

typedef volatile int mutex_t;

#define acquire_mutex(m) while(!__sync_bool_compare_and_swap(m, 0, 1)){}
#define release_mutex(m) *m = 0 

#endif