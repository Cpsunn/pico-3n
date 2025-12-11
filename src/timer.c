/**
 * 定时器实现（基于 Pico SDK）
 */

#include "timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"

static timer_config_t *g_timer_config = NULL;
static uint64_t g_timer_ticks = 0;

/**
 * 初始化定时器
 */
void timer_init(timer_config_t *config)
{
    if (!config) return;
    
    g_timer_config = config;
    g_timer_ticks = 0;
    config->is_running = false;
}

/**
 * 启动定时器（在本实现中为占位符，使用主循环的定时方式）
 */
void timer_start(timer_config_t *config)
{
    if (!config) return;
    config->is_running = true;
}

/**
 * 停止定时器
 */
void timer_stop(timer_config_t *config)
{
    if (!config) return;
    config->is_running = false;
}

/**
 * 获取定时器计数
 */
uint64_t timer_get_ticks(void)
{
    return g_timer_ticks;
}

/**
 * 延时函数（毫秒）
 */
void timer_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}
