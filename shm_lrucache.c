#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "array_queue.h"
#include "jenkins_hash.h"

#include "shm_lrucache.h"
#include "shm_buffer.h"
#include "lock_helper.h"
#include "shm_def.h"

#define LRU_KEY_SIZE 256
#define LRU_MAGIC	0xABCD1234

static char *shm_buf;
static int flock_fd = -1;

struct lru_node {
	char key[LRU_KEY_SIZE];
	int64_t value;
	STAILQ_ENTRY(lru_node) lru;
	STAILQ_ENTRY(lru_node) hash;
};

STAILQ_HEAD(list_head, lru_node);
struct lrucache {
	uint32_t magic;
	uint32_t size;
	uint32_t buckets;
	uint32_t __align;
	struct list_head lru_list;
	struct list_head hash_list[0];
};

typedef struct lrucache lrucache_t;

static struct lru_node *lru_array;

static int calculate_bufsize(int size, int buckets)
{
	return sizeof(lrucache_t) + buckets*sizeof(struct list_head) + size*sizeof(struct lru_node);
}
static bool try_lock()
{
	return file_lock(flock_fd);
}
static void unlock() 
{
	file_unlock(flock_fd);
}

static uint32_t hash_str(char* key)
{
	return jenkins_hash(key, strlen(key));
}

static struct lru_node* find_node(lrucache_t* ctx, char* key)
{
	const int idx = hash_str(key) % ctx->buckets;

	STAILQ_FOREACH(n, &ctx->hash_list[idx], lru_array, hash) {
		if (strcmp(lru_array[n].key, key) == 0) {
			return &(lru_array[n]);
		}
	}
	return NULL;
}

static void hash_list_insert(lrucache_t* ctx, char* key, struct lru_node* n)
{
	int idx = hash_str(key) % ctx->buckets;

	STAILQ_INSERT_HEAD(&ctx->hash_list[idx], n, hash);
}
static void hash_list_remove(lrucache_t* ctx, char* key, struct lru_node* n)
{
	int idx = hash_str(key) % ctx->buckets;

	STAILQ_REMOVE(&ctx->hash_list[idx], lru_array, n, hash);
}

static struct lru_node* get_one_lru_node(struct list_head* list)
{
	struct lru_node* n = &STAILQ_FIRST(list, lru_array);
	STAILQ_REMOVE_HEAD(list, lru_array, lru);
	STAILQ_INSERT_TAIL(list, lru_array, n, lru);
	return n;
}

static struct lru_node* 
get_new_node_and_insert(lrucache_t *ctx, char* key, int64_t value)
{
	struct lru_node* n = get_one_lru_node(&(ctx->lru_list));

	if (n->key[0] != '\0') {
		hash_list_remove(ctx, n->key, n);
	}
	strncpy(n->key, key, sizeof(n->key));
	n->value = value;
	hash_list_insert(ctx, key, n);
	return n;
}

/*
export functions
*/
int shm_lrucache_init(int elem_nums)
{
	/*
		共享内存的结构图:
		-------------------------------------------------------------
		|struct lrucache | hash bucket[...] | struct lru_node [...] |
		-------------------------------------------------------------
	*/
	const int buckets = elem_nums;
	if (shm_buf == NULL) {
		const int bufsize = calculate_bufsize(elem_nums, buckets);
		printf("try to malloc buffer of %d bytes\n", bufsize);
		shm_buf = shm_lru_get_buffer(shm_buf, bufsize);
	}
	lrucache_t *ctx = (lrucache_t*)shm_buf;

	lru_array = (struct lru_node*)(shm_buf + sizeof(lrucache_t) + buckets*sizeof(struct list_head));

	if (flock_fd < 0) {
		flock_fd = open(FLOCK_FILE_PATH, O_CREAT|O_CLOEXEC|O_RDWR, 0644);
		if (flock_fd < 0) {
			perror("open");
			exit(errno);
		}
	}

	if (ctx->magic != LRU_MAGIC || elem_nums != ctx->size || buckets != ctx->buckets) {
		printf("init lrucache...\n");
		if (try_lock()) {
			memset(ctx, 0, sizeof(*ctx));

			ctx->magic = LRU_MAGIC;
			ctx->size = elem_nums;
			ctx->buckets = buckets;

			STAILQ_INIT(&ctx->lru_list);
			for (int i = 0; i < ctx->buckets; i++) {
				STAILQ_INIT(&ctx->hash_list[i]);
			}
			for (int i = 0; i < ctx->size; i++) {
				lru_array[i].key[0] = '\0';
				lru_array[i].lru.self_idx = i;
				lru_array[i].hash.self_idx = i;
				STAILQ_INSERT_TAIL(&ctx->lru_list, lru_array, &lru_array[i], lru);
			}
			unlock();
		} else {
			printf("%s:try_lock error!\n", __func__);
			return -1;
		}
	} 
	return 0;
}
int shm_lrucache_set(char* key, int64_t value)
{
	lrucache_t *ctx = (lrucache_t*)shm_buf;

	if (try_lock()) {
		struct lru_node * n = find_node(ctx, key);

		if (n) {
			n->value = value;
		} else {
			n = get_new_node_and_insert(ctx, key, value);
		}
		unlock();
		return 0;
	} else {
		printf("try_lock error!\n");
		return -1;
	}
}

int64_t shm_lrucache_get(char* key)
{
	lrucache_t *ctx = (lrucache_t*)shm_buf;

	int64_t value = 0;
	if (try_lock()) {
		struct lru_node * n = find_node(ctx, key);
		if (n) {
			value = n->value;
		}
		unlock();
	} else {
		printf("try_lock error!\n");
	}

	return value;
}

int64_t shm_lruache_incr_by(char* key, int64_t value)
{
	lrucache_t *ctx = (lrucache_t*)shm_buf;

	int64_t new_value = 0;

	if (try_lock()) {
		struct lru_node * n = find_node(ctx, key);
		if (n) {
			n->value += value;
		} else {
			n = get_new_node_and_insert(ctx, key, value);
		}
		new_value = n->value;
		unlock();
	}
	return new_value;
}

int64_t shm_lruache_incr(char* key)
{
	return shm_lruache_incr_by(key, 1);
}



