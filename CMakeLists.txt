cmake_minimum_required(VERSION 2.8)

project(shm_dict)


add_compile_options(
    -g -Wall -Wformat -std=c99
    -D_XOPEN_SOURCE=500
    -D_GNU_SOURCE
    )
include_directories(
    ./
    )
#link_directories(${PROJECT_SOURCE_DIR}/libs)
add_executable(${PROJECT_NAME}
    jenkins_hash.c
    shm_lrucache.c
    shm_buffer.c
    test_main.c
    lock_helper.c
    )
target_link_libraries(${PROJECT_NAME}
    pthread
)

