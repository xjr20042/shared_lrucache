#ifndef FE2A809C_1DEA_44DC_A93E_307600E5CD8F
#define FE2A809C_1DEA_44DC_A93E_307600E5CD8F
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
	多进程共享的LRU key-value 缓存

	内部使用mmap 获取共享内存， 关联的文件: MMAP_FILE_PATH;
	内部使用内存池来存储key，内存池也是在共享内存上，关联的文件:MMAP_POOL_FILE_PATH
	内部使用flock 安全同步，关联的文件: FLOCK_FILE_PATH

	key 的长度必须小于4096
*/


/*
	此函数初始化缓存
	number_of_cache_entries: 缓存最大条数，超出时会删除最少使用的缓存来释放空间
	返回0成功，-1失败
*/
int shm_lrucache_init(int number_of_cache_entries);

/*
	return 0 表示成功, -1 表示失败
*/
int shm_lrucache_set(char* key, int64_t value);

/*
	return 0 表示成功, -1 表示失败
*/
int shm_lrucache_get(char* key, int64_t* value);

/*
	将key对应的值递增 1，key不存在时会自动创建并赋值为 1
    new_value 返回最新的值
	return 0 表示成功, -1 失败
*/
int shm_lrucache_incr(char* key, int64_t* new_value);

/*
	将key对应的值 += value, value 可以是负值
	key不存在时会自动创建并赋值为 value
    new_value 返回最新的值
	return 0 表示成功, -1 失败
*/
int shm_lrucache_incr_by(char* key, int64_t value, int64_t *new_value);

#ifdef __cplusplus
}
#endif

#endif /* FE2A809C_1DEA_44DC_A93E_307600E5CD8F */
