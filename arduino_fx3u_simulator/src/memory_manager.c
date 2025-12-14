/**
 * 内存管理实现
 */

#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t size;
} memory_block_header_t;

static memory_stats_t g_memory_stats = {
    .total_size = 258 * 1024,  /* Pico的RAM大小 */
    .used_size = 0,
    .free_size = 258 * 1024
};

/**
 * 初始化内存管理器
 */
void memory_init(void)
{
    g_memory_stats.used_size = 0;
    g_memory_stats.free_size = g_memory_stats.total_size;
}

/**
 * 分配内存
 */
void* memory_alloc(uint32_t size)
{
    if (size == 0 || size > g_memory_stats.free_size) {
        return NULL;
    }
    
    memory_block_header_t *block = malloc(sizeof(memory_block_header_t) + size);
    if (!block) {
        return NULL;
    }
    
    block->size = size;
    g_memory_stats.used_size += size;
    g_memory_stats.free_size -= size;
    
    return (void *)(block + 1);
}

/**
 * 释放内存
 */
void memory_free(void *ptr)
{
    if (!ptr) return;
    
    memory_block_header_t *block = ((memory_block_header_t *)ptr) - 1;
    uint32_t size = block->size;
    
    if (size <= g_memory_stats.used_size) {
        g_memory_stats.used_size -= size;
        g_memory_stats.free_size += size;
    }
    
    free(block);
}

/**
 * 获取内存统计
 */
memory_stats_t* memory_get_stats(void)
{
    return &g_memory_stats;
}

/**
 * 保存程序到Flash
 */
void memory_dump_program(uint8_t *buffer, uint32_t size)
{
    if (!buffer || size == 0) return;
    
    /* Pico Flash操作 - 需要使用pico_flash_ops库 */
}

/**
 * 从Flash加载程序
 */
void memory_load_program(uint8_t *buffer, uint32_t size)
{
    if (!buffer || size == 0) return;
    
    /* Pico Flash操作 - 需要使用pico_flash_ops库 */
}
