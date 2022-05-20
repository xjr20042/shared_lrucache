#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "shm_lrucache.h"
#include "array_queue.h"
#include "shm_pool.h"

void test_pool3()
{
	shmp_pool_init();

    const int test_count = 1000000;
	struct shmp_buffer buf[test_count];

	int bufsize_array[] = {16,32,64,128};//,256,512,1024,2048,4096};
	srand(time(NULL));
	for (size_t i = 0; i < test_count; i++)
	{
		int buflen = bufsize_array[rand()%(sizeof(bufsize_array)/sizeof(int))];
		if (0 == shmp_get_buffer(buflen-1, &(buf[i]))) {
			//printf("ok %d-%d-%d %p\n", buf[i].block_idx, buf[i].page_idx, buf[i].slic_idx, shmp_buffer_addr(&(buf[i])));
			shmp_buffer_addr(&(buf[i]));
		} else {
			printf("no more memory! %ld\n", i);
			exit(0);
		}
		if (rand()%2 == 0) {
			shmp_put_buffer(&(buf[i]));
		}
	}
}
void test_pool2()
{
	shmp_pool_init();

#define TEST_COUNT  1000000
	static struct shmp_buffer buf[TEST_COUNT];

	srand(time(NULL));
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		char key[32] = "";
		sprintf(key, "key_%020ld", i);
		if (0 == shmp_get_buffer(strlen(key)+1, &(buf[i]))) {
			//printf("ok %d-%d-%d %p\n", buf[i].block_idx, buf[i].page_idx, buf[i].slic_idx, shmp_buffer_addr(&(buf[i])));
			strcpy(shmp_buffer_addr(&(buf[i])), key);
		} else {
			printf("no more memory! %ld\n", i);
			exit(0);
		}
	}
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		char key[32] = "";
		sprintf(key, "key_%020ld", i);

		if (strcmp(shmp_buffer_addr(&(buf[i])), key) != 0) {
			printf("cmp error! %s-%s\n", shmp_buffer_addr(&(buf[i])), key);
			exit(0);
		}
		shmp_put_buffer(&(buf[i]));
	}
#undef TEST_COUNT
}
void test_pool1()
{
#define TEST_COUNT  1000000
	shmp_pool_init();

	static struct shmp_buffer buf[TEST_COUNT];

	int bufsize_array[] = {16,32,64,128};//,256,512,1024,2048,4096};
	srand(time(NULL));
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		int buflen = bufsize_array[rand()%(sizeof(bufsize_array)/sizeof(int))];
		if (0 == shmp_get_buffer(buflen-1, &(buf[i]))) {
			//printf("ok %d-%d-%d %p\n", buf[i].block_idx, buf[i].page_idx, buf[i].slic_idx, shmp_buffer_addr(&(buf[i])));
			shmp_buffer_addr(&(buf[i]));
		} else {
			printf("no more memory! %ld\n", i);
			exit(0);
		}

	}
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		shmp_put_buffer(&(buf[i]));
	}
#undef TEST_COUNT
}

void test_queue() 
{
	struct Node {
		int value;
		STAILQ_ENTRY(Node) l;
	};

	STAILQ_HEAD(st_head, Node);

	struct st_head h;

	STAILQ_INIT(&h);

	struct Node nodes[10];

	for (size_t i = 0; i < 10; i++)
	{
		nodes[i].value = i+1;
		nodes[i].l.self_idx = i;

		//STAILQ_INSERT_HEAD(&h, &nodes[i], l);
		STAILQ_INSERT_TAIL(&h, nodes, &nodes[i], l);
	}

	for (size_t i = 0; i < 10; i++)
	{
		struct Node *n = &(STAILQ_FIRST(&h, nodes));
		STAILQ_REMOVE_HEAD(&h, nodes, l);
		STAILQ_INSERT_TAIL(&h, nodes, n, l);
	}
	
	STAILQ_FOREACH(idx, &h, nodes, l) {
		printf("%d\n", nodes[idx].value);
	}

	// STAILQ_REMOVE(&h, nodes, &nodes[0], l);
	// STAILQ_REMOVE(&h, nodes, &nodes[3], l);
	// STAILQ_REMOVE(&h, nodes, &nodes[9], l);
	// STAILQ_INSERT_TAIL(&h, nodes, &nodes[0], l);

	for (;!STAILQ_EMPTY(&h);) {
		printf("remove:%d\n",STAILQ_FIRST(&h, nodes).value);
		STAILQ_REMOVE_HEAD(&h, nodes, l);
	}

}

void test_lru(const char* action)
{
	shm_lrucache_init(1000000);
	int test_count = 2000000;

	if (strcmp(action, "set") == 0) {

		for (size_t i = 0; i < test_count; i++)
		{
			char key[32];
			snprintf(key, sizeof(key), "abc_%019ld", i);
			//printf("set key:%s\n", key);
            //int64_t new_value = 0;
			shm_lrucache_set(key, i);	
		}

		//shm_lrucache_incr_by("abc_00002", 100);
	} else {
		for (size_t i = 0; i < test_count; i++)
		{
			char key[32];
			snprintf(key, sizeof(key), "abc_%019ld", i);
			int64_t value =0;
            int ret = shm_lrucache_get(key, &value);
			//printf("get %s:%ld\n", key, value);
			//assert(ret == 0);
			if (ret == 0) {
				assert(value == i);
			}
		}
	}

}
int main(int argc, char const *argv[])
{
	test_pool1();
	//test_lru(argv[1]);
	return 0;
}
