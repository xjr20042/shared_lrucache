#ifndef CD2B0984_4DBC_4A61_A7DD_77995B2E0B39
#define CD2B0984_4DBC_4A61_A7DD_77995B2E0B39
/*

	memory pool used on shared memory

*/
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct shmp_buffer {
	uint16_t size;
	uint16_t block_idx;
	uint16_t page_idx;
	uint8_t  slic_idx;
	uint8_t  is_valid:1;
};

int   shmp_pool_init();

int   shmp_get_buffer(int buffer_size, struct shmp_buffer* buf);
int   shmp_put_buffer(struct shmp_buffer* buf);

char* shmp_buffer_addr(struct shmp_buffer* buf);
int   shmp_buffer_size(struct shmp_buffer* buf);
bool  shmp_buffer_is_valid(struct shmp_buffer* buf);

#ifdef __cplusplus
}
#endif

#endif /* CD2B0984_4DBC_4A61_A7DD_77995B2E0B39 */
