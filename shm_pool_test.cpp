#include <gtest/gtest.h>
#include <unistd.h>
#include "shm_def.h"
#include "shm_pool.h"

TEST(pool, sanity) {
    const int TEST_COUNT = 1000000;

	struct shmp_buffer buf[TEST_COUNT];

	int bufsize_array[] = {16,32,64,128};//,256,512,1024,2048,4096};
	srand(time(NULL));
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		int buflen = bufsize_array[rand()%(sizeof(bufsize_array)/sizeof(int))];
		ASSERT_EQ(0, shmp_get_buffer(buflen-1, &(buf[i])));
        shmp_buffer_addr(&(buf[i]));
	}
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		shmp_put_buffer(&(buf[i]));
	}
}

TEST(pool, strcmp) {
    const int TEST_COUNT = 1000000;

	struct shmp_buffer buf[TEST_COUNT];

	srand(time(NULL));
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		char key[32] = "";
		sprintf(key, "key_%020ld", i);
		ASSERT_EQ(0, shmp_get_buffer(strlen(key)+1, &(buf[i])));
        strcpy(shmp_buffer_addr(&(buf[i])), key);
	}
	for (size_t i = 0; i < TEST_COUNT; i++)
	{
		char key[32] = "";
		sprintf(key, "key_%020ld", i);

		ASSERT_EQ(0, strcmp(shmp_buffer_addr(&(buf[i])), key));
		shmp_put_buffer(&(buf[i]));
	}
}

TEST(loop, randomPut) {
    const int test_count = 1000000;
	struct shmp_buffer buf[test_count];

	int bufsize_array[] = {16,32,64,128};//,256,512,1024,2048,4096};
	srand(time(NULL));
	for (size_t i = 0; i < test_count; i++)
	{
		int buflen = bufsize_array[rand()%(sizeof(bufsize_array)/sizeof(int))];
		ASSERT_EQ(0, shmp_get_buffer(buflen-1, &(buf[i])));
        shmp_buffer_addr(&(buf[i]));
		if (rand()%2 == 0) {
			shmp_put_buffer(&(buf[i]));
		}
	}
}

int main(int argc, char* argv[]) {
    unlink(MMAP_POOL_FILE_PATH);
	shmp_pool_init();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
