#include <pthread.h>

static pthread_mutex_t __vrd_global_lock = PTHREAD_MUTEX_INITIALIZER;

void __VERIFIER_atomic_begin(void) {
	pthread_mutex_lock(&__vrd_global_lock);
}

void __VERIFIER_atomic_end(void) {
	pthread_mutex_unlock(&__vrd_global_lock);
}
