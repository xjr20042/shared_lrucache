#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shm_buffer.h"
#include "shm_def.h"

char* shm_lru_get_buffer(char* addr, int bufsize)
{
	int fd = open(MMAP_FILE_PATH, O_CREAT|O_CLOEXEC|O_RDWR, 0644);

	if (fd == -1) {
		perror("open error");
		return NULL;
	}

	struct stat st;
	if (fstat(fd, &st) == -1) {
		perror("fstat error");
		return NULL;
	}
	if (st.st_size < bufsize) {
		ftruncate(fd, (off_t)bufsize);
	}

	addr = mmap(addr, bufsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if ((intptr_t)addr == -1) {
		perror("mmap error");
		return NULL;
	}
	return addr;
}