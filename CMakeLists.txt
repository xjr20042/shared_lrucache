cmake_minimum_required(VERSION 3.8)

project(shm_dict)


add_compile_options(
    -g
    -Wall
    -Wformat
    -D_XOPEN_SOURCE=500
    -D_GNU_SOURCE
    #-O2
    )
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

include_directories(
    ./
    )
link_directories(${PROJECT_SOURCE_DIR})
set(SRC_LIST
    jenkins_hash.c
    shm_lrucache.c
    shm_buffer.c
    lock_helper.c
    shm_pool.c
)
add_library(shm_lrucache SHARED ${SRC_LIST})

#test
add_executable(array_queue_test
    array_queue_test.cpp
)
target_link_libraries(array_queue_test
    gtest
)

add_executable(shm_pool_test
    shm_buffer.c
    shm_pool.c
    shm_pool_test.cpp
)
target_link_libraries(shm_pool_test
    pthread gtest
)

add_executable(lru_test
    lru_test.cpp
)
target_link_libraries(lru_test
    pthread shm_lrucache gtest
)

