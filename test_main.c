#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "shm_lrucache.h"
#include "array_queue.h"


struct Node {
	int value;
	STAILQ_ENTRY(Node) l;
};

STAILQ_HEAD(st_head, Node);
int main(int argc, char const *argv[])
{
	// struct st_head h;

	// STAILQ_INIT(&h);

	// struct Node nodes[10];

	// for (size_t i = 0; i < 10; i++)
	// {
	// 	nodes[i].value = i+1;
	// 	nodes[i].l.self_idx = i;

	// 	//STAILQ_INSERT_HEAD(&h, &nodes[i], l);
	// 	STAILQ_INSERT_TAIL(&h, nodes, &nodes[i], l);
	// }

	// for (size_t i = 0; i < 10; i++)
	// {
	// 	struct Node *n = &(STAILQ_FIRST(&h, nodes));
	// 	STAILQ_REMOVE_HEAD(&h, nodes, l);
	// 	STAILQ_INSERT_TAIL(&h, nodes, n, l);
	// }
	
	// STAILQ_FOREACH(idx, &h, nodes, l) {
	// 	printf("%d\n", nodes[idx].value);
	// }

	// // STAILQ_REMOVE(&h, nodes, &nodes[0], l);
	// // STAILQ_REMOVE(&h, nodes, &nodes[3], l);
	// // STAILQ_REMOVE(&h, nodes, &nodes[9], l);
	// // STAILQ_INSERT_TAIL(&h, nodes, &nodes[0], l);

	// for (;!STAILQ_EMPTY(&h);) {
	// 	printf("remove:%d\n",STAILQ_FIRST(&h, nodes).value);
	// 	STAILQ_REMOVE_HEAD(&h, nodes, l);
	// }




	
	shm_lrucache_init(1000000);
	int test_count = 1000000;

	if (strcmp(argv[1], "set") == 0) {

		for (size_t i = 0; i < test_count; i++)
		{
			char key[32];
			snprintf(key, sizeof(key), "abc_%08ld", i);
			//printf("set key:%s\n", key);
			shm_lrucache_set(key, i);	
		}

		//shm_lruache_incr_by("abc_00002", 100);
	} else {
		for (size_t i = 0; i < test_count; i++)
		{
			char key[32];
			snprintf(key, sizeof(key), "abc_%08ld", i);
			int64_t value = shm_lrucache_get(key);
			//printf("get %s:%ld\n", key, value);
			if (value != 0) {
				assert(value == i);
			}
		}
	}

	return 0;
}
