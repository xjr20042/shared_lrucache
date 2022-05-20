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

char* shm_lru_get_buffer(char* file_path, int bufsize)
{
	int fd = open(file_path, O_CREAT|O_CLOEXEC|O_RDWR, 0644);

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

	char* addr = mmap(0, bufsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	if ((intptr_t)addr == -1) {
		perror("mmap error");
		return NULL;
	}
	close(fd);
	return addr;
}

char* shm_lru_extend_buffer(char* file_path, char* old_addr, size_t old_bufsize, size_t new_size)
{
    //    void *mremap(void *old_address, size_t old_size,
    //                 size_t new_size, int flags, ... /* void *new_address */);

	truncate(file_path, new_size);
	printf("extend buffer to %ld\n", new_size);
	return mremap(old_addr, old_bufsize, new_size, MREMAP_MAYMOVE);
}