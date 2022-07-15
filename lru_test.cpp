#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "shm_def.h"
#include "shm_lrucache.h"
#include <gtest/gtest.h>

TEST(lru, set) {
	size_t test_count = 1000000;

    for (size_t i = 0; i < test_count; i++)
    {
        char key[32];
        snprintf(key, sizeof(key), "abc_%019ld", i);
        //printf("set key:%s\n", key);
        //int64_t new_value = 0;
        ASSERT_EQ(0, shm_lrucache_set(key, i));
    }
}

TEST(lru, get) {

	size_t test_count = 1000000;
    
    for (size_t i = 0; i < test_count; i++)
    {
        char key[32];
        snprintf(key, sizeof(key), "abc_%019ld", i);
        int64_t value =0;
        ASSERT_EQ(0, shm_lrucache_get(key, &value));
        ASSERT_EQ(value, i);
    }
}

int main(int argc, char *argv[])
{
    unlink(FLOCK_FILE_PATH);
    unlink(MMAP_FILE_PATH);
    unlink(MMAP_POOL_FILE_PATH);
	shm_lrucache_init(1000000);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
