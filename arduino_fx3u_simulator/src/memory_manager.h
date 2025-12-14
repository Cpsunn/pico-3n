/**
 * 内存管理器
 */

#ifndef __MEMORY_MANAGER_H__
#define __MEMORY_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t total_size;
    uint32_t used_size;
    uint32_t free_size;
} memory_stats_t;

void memory_init(void);
void* memory_alloc(uint32_t size);
void memory_free(void *ptr);
memory_stats_t* memory_get_stats(void);
void memory_dump_program(uint8_t *buffer, uint32_t size);
void memory_load_program(uint8_t *buffer, uint32_t size);

#endif
