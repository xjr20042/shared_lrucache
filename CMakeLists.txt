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
link_directories(${PROJECT_SOURCE_DIR})
set(SRC_LIST
    jenkins_hash.c
    shm_lrucache.c
    shm_buffer.c
    lock_helper.c
    shm_pool.c
)
add_library(objs OBJECT ${SRC_LIST})
set_property(TARGET objs PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(lib_shared SHARED $<TARGET_OBJECTS:objs>)
set_property(TARGET lib_shared PROPERTY OUTPUT_NAME "shm_lrucache")

add_executable(${PROJECT_NAME}
    test_main.c
)
target_link_libraries(${PROJECT_NAME}
    pthread shm_lrucache
)

