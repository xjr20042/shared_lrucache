#include <stdio.h>
#include "array_queue.h"
#include <gtest/gtest.h>

#define INIT_ARRAY(n) \
    struct Node { \
        int value; \
        STAILQ_ENTRY(Node) l; \
	}; \
	STAILQ_HEAD(st_head, Node); \
	struct st_head h; \
	STAILQ_INIT(&h); \
	struct Node nodes[n]

TEST(array_queue, sanity) {

    const int len = 10;
    INIT_ARRAY(len);
	for (size_t i = 0; i < len; i++)
	{
		nodes[i].value = i+1;
		nodes[i].l.self_idx = i;

		//STAILQ_INSERT_HEAD(&h, &nodes[i], l);
		STAILQ_INSERT_TAIL(&h, nodes, &nodes[i], l);
	}

	for (size_t i = 0; i < len; i++)
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

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
