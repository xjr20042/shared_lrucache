#ifndef D8C5C477_FBA2_4CBA_9BC5_7625D7C82B1F
#define D8C5C477_FBA2_4CBA_9BC5_7625D7C82B1F

#ifdef __cplusplus
extern "C" {
#endif

char* shm_lru_get_buffer(char* file_path, int bufsize);

char* shm_lru_extend_buffer(char* file_path, char* old_addr, size_t old_bufsize, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif /* D8C5C477_FBA2_4CBA_9BC5_7625D7C82B1F */
