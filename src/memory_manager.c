/**
 * 内存管理实现
 */

#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>

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
    if (size > g_memory_stats.free_size) return NULL;
    
    void *ptr = malloc(size);
    if (ptr) {
        g_memory_stats.used_size += size;
        g_memory_stats.free_size -= size;
    }
    
    return ptr;
}

/**
 * 释放内存
 */
void memory_free(void *ptr)
{
    if (!ptr) return;
    free(ptr);
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
