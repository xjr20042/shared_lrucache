#ifndef JENKINS_HASH_H
#define    JENKINS_HASH_H
#include <stdint.h>
#include <stddef.h>
#define ENDIAN_LITTLE 1 
uint32_t jenkins_hash(const void *key, size_t length);


#endif    /* JENKINS_HASH_H */

