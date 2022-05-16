#ifndef D13D3A1D_37E6_4394_BC57_E0A1DF662379
#define D13D3A1D_37E6_4394_BC57_E0A1DF662379
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

bool mutex_trylock(pthread_mutex_t* mutex, unsigned int timeout_ms);


bool file_lock(int fd);

void file_unlock(int fd);


inline static pthread_mutex_t* mutex_lock_guard_lock(pthread_mutex_t* mutex)
{
	if (0 != pthread_mutex_lock(mutex) ) {
		printf("lock error!\n");
	}
	//printf("%s\n", __func__);
	return mutex;
}
inline static void mutex_lock_guard_unlock(pthread_mutex_t** mutex)
{
	if (0 != pthread_mutex_unlock(*mutex) ) {
		printf("unlock error!\n");
	}
	//printf("%s\n", __func__);
}
#define MUTEX_LOCK_GUARD(m) \
	__attribute__((cleanup(mutex_lock_guard_unlock))) pthread_mutex_t* _mtx ## __LINE__ = mutex_lock_guard_lock((m));

#endif /* D13D3A1D_37E6_4394_BC57_E0A1DF662379 */
