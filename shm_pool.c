#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shm_buffer.h"
#include "shm_def.h"
#include "shm_pool.h"

#define PAGE_BUF_SIZE	4096
#define MAX_PAGE_SIZE	256
#define MAX_BLOCK_SIZE	256
#define INITIAL_BLOCK_SIZE	8
#define POOL_MAGIC		0xa69ae353

enum {
	cache_type_none = 0,
	cache_type_fast_recycle  = 1,
	cache_type_last_get_new  = 2,
};

// 4KB
struct shmp_page {
	uint32_t  is_full:1; 
	uint32_t  __align; 
	// 按位表示对应的slic是否被使用
	uint64_t  is_used[4];
};
// 1MB = 256*page
struct shmp_block {
	//0, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096
	//0 代表未初始化
	uint16_t  block_type;	
	uint16_t  is_full:1;
	uint16_t  cache_type:3;
	struct shmp_buffer buf_cache;
	struct shmp_page pages[MAX_PAGE_SIZE];
};

struct shmp_pool {
	//当前有效的block个数
	uint32_t magic;
	uint32_t valid_block_number;
	int64_t  shm_buf_size;
	struct shmp_block blocks[MAX_BLOCK_SIZE];
};

static char* shmp_buf;
static const int bufsize_array[] = {16,32,64,128,256,512,1024,2048,4096};

static int bufsize_align(int bufsize) 
{
	for (size_t i = 0; i < sizeof(bufsize_array)/sizeof(bufsize_array[0]); i++)
	{
		if (bufsize <= bufsize_array[i])
			return bufsize_array[i];
	}
	return -1;
}

static bool check_slic_is_used(struct shmp_page* p, int slic_idx)
{
	const int bit_width = sizeof(uint64_t)*8;
	int array_idx = slic_idx / bit_width;
	int bit_idx = slic_idx % bit_width;

	return (p->is_used[array_idx] & ((uint64_t)1<<bit_idx) ) != 0;	
}

static void set_slic_is_used(struct shmp_page* p, int slic_idx, int used)
{
	const int bit_width = sizeof(uint64_t)*8;
	int array_idx = slic_idx / bit_width;
	int bit_idx = slic_idx % bit_width;
	
	if (used == 1) {
		p->is_used[array_idx] |= ((uint64_t)1<<bit_idx);
	} else {
		p->is_used[array_idx] &= ~((uint64_t)1<<bit_idx);
	}
}
static size_t calculate_bufsize(size_t block_number)
{
	//
	return sizeof(struct shmp_pool) + block_number*MAX_PAGE_SIZE*PAGE_BUF_SIZE;
}
int shmp_pool_init()
{
	size_t init_bufsize = calculate_bufsize(INITIAL_BLOCK_SIZE);
	if (shmp_buf == NULL) {
		printf("try to malloc buffer of %ld\n", init_bufsize);
		shmp_buf = shm_lru_get_buffer(MMAP_POOL_FILE_PATH, init_bufsize);
	}
	struct shmp_pool* p = (struct shmp_pool*)shmp_buf;

	if (p->magic != POOL_MAGIC) {
		memset(p, 0, sizeof(*p));
		p->magic = POOL_MAGIC;
		p->valid_block_number = INITIAL_BLOCK_SIZE;
		p->shm_buf_size = calculate_bufsize(p->valid_block_number);
	} else if (p->valid_block_number != INITIAL_BLOCK_SIZE) {
		shmp_buf = 
			shm_lru_extend_buffer(MMAP_POOL_FILE_PATH, shmp_buf, init_bufsize,
		 		p->shm_buf_size);
	}

	return 0;
}


int shmp_get_buffer(int buffer_size, struct shmp_buffer* buf)
{
	struct shmp_pool* p = (struct shmp_pool*)shmp_buf;

	const int buf_type = bufsize_align(buffer_size);
	const int slic_count = PAGE_BUF_SIZE/buf_type;

	buf->is_valid = 1;
	//printf("slic_count:%d\n", slic_count);
	for (size_t i = 0; i < p->valid_block_number; i++) {
		struct shmp_block * b = &(p->blocks[i]);
		if (b->is_full)
			continue;

		if (b->block_type != buf_type && b->block_type !=0)
			continue;

		if (b->block_type == 0) 
			b->block_type = buf_type;

		if (b->cache_type == cache_type_fast_recycle) {
			//printf("%s-%d\n", __FILE__, __LINE__);
			struct shmp_page * p = &(b->pages[b->buf_cache.page_idx]);
			*buf = b->buf_cache;
			b->cache_type = cache_type_none;
			set_slic_is_used(p, b->buf_cache.slic_idx, 1);
			return 0;
		} else if (b->cache_type == cache_type_last_get_new) {
			//printf("%s-%d\n", __FILE__, __LINE__);
			struct shmp_page * p = &(b->pages[b->buf_cache.page_idx]);

			uint8_t next_slic_idx = b->buf_cache.slic_idx + 1;
			if (next_slic_idx < slic_count && !check_slic_is_used(p, next_slic_idx)) {
				set_slic_is_used(p, next_slic_idx, 1);
				b->buf_cache.slic_idx = next_slic_idx;
				*buf = b->buf_cache;

				if (next_slic_idx != slic_count) {
					b->cache_type = cache_type_last_get_new;
				} else {
					b->cache_type = cache_type_none;
				}
				return 0;
			}
		}

		buf->block_idx = i;
		buf->size = buf_type;
		for (size_t j = 0; j < MAX_PAGE_SIZE; j++) {
			struct shmp_page * p = &(b->pages[j]);
			if (p->is_full) {
				continue;
			}
			for (size_t s = 0; s < slic_count; s++) {
				if ( !check_slic_is_used(p, s) ) {
					buf->page_idx = j;
					buf->slic_idx = s;
					set_slic_is_used(p, s, 1);
					if (s == slic_count - 1) {
						p->is_full = 1;
					} else {
						b->buf_cache = *buf;
						b->cache_type = cache_type_last_get_new;
					}
					return 0;
				}
			}
			p->is_full = 1;
		}
		b->is_full = 1;
	}
	// 需要扩展内存
	if (p->valid_block_number < MAX_BLOCK_SIZE) {
		p->valid_block_number += INITIAL_BLOCK_SIZE;
		if (p->valid_block_number > MAX_BLOCK_SIZE)
			p->valid_block_number = MAX_BLOCK_SIZE;
		size_t new_size =  calculate_bufsize(p->valid_block_number);
		shmp_buf = shm_lru_extend_buffer(MMAP_POOL_FILE_PATH, shmp_buf, p->shm_buf_size, new_size);
		p = (struct shmp_pool*)shmp_buf;
		p->shm_buf_size = new_size;
		return shmp_get_buffer(buffer_size, buf);
	}
	buf->is_valid = 0;
	return -1;
}

int shmp_put_buffer(struct shmp_buffer* buf)
{
	struct shmp_pool* pl = (struct shmp_pool*)shmp_buf;

	struct shmp_block* b = &(pl->blocks[buf->block_idx]);
	struct shmp_page * p = &(b->pages[buf->page_idx]);

	set_slic_is_used(p, buf->slic_idx, 0);

	if (b->is_full) {
		b->is_full = 0;
	}
	if (p->is_full) {
		p->is_full = 0;
	}

	b->buf_cache = *buf;
	b->cache_type = cache_type_fast_recycle;
	buf->is_valid = 0;
	return 0;
}

char* shmp_buffer_addr(struct shmp_buffer* buf)
{
    return (char*)shmp_buf + sizeof(struct shmp_pool) +
            (size_t)(buf->block_idx)*(MAX_PAGE_SIZE*PAGE_BUF_SIZE) + 
            (size_t)(buf->page_idx)*(PAGE_BUF_SIZE) + 
            (size_t)(buf->slic_idx)*(buf->size);
}

int   shmp_buffer_size(struct shmp_buffer* buf)
{
	return buf->size;
}

bool  shmp_buffer_is_valid(struct shmp_buffer* buf)
{
	return buf->is_valid == 1;
}