#include <sys/file.h>
#include <time.h>

#include "lock_helper.h"


// static void reinit_mutex(struct lrucache* ctx)
// {
// 	pthread_mutexattr_t  attr;
// 	pthread_mutexattr_init(&attr);
// 	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
// 	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
// 	pthread_mutex_init(&ctx->mutex, &attr);
// }

// bool mutex_trylock(pthread_mutex_t* mutex, unsigned int timeout_ms)
// {
//   int res;

//   if (timeout_ms) {
//     struct timespec timestamp;

//     clock_gettime(CLOCK_REALTIME, &timestamp);
//     timestamp.tv_sec += timeout_ms / 1000;
//     timestamp.tv_nsec += (timeout_ms % 1000) * 1000000;

//     if (timestamp.tv_nsec >= 1000000000) {
//       timestamp.tv_nsec -= 1000000000;
//       ++timestamp.tv_sec;
//     }

//     res = pthread_mutex_timedlock(mutex, &timestamp);
//   } else {
//     res = pthread_mutex_trylock(mutex);
//   }

//   return res ? false : true;
// }

bool file_lock(int fd) 
{
	return flock(fd, LOCK_EX) == 0;
}

void file_unlock(int fd) 
{
	flock(fd, LOCK_UN);
}