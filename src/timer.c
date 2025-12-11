/**
 * 定时器实现
 */

#include "timer.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"

static timer_config_t *g_timer_config = NULL;
static uint64_t g_timer_ticks = 0;

/**
 * 定时器中断处理
 */
static int64_t timer_callback(alarm_id_t id, void *user_data)
{
    g_timer_ticks++;
    
    if (g_timer_config && g_timer_config->callback) {
        g_timer_config->callback();
    }
    
    /* 重新安排下一个中断 */
    return g_timer_config->period_us;
}

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
 * 启动定时器
 */
void timer_start(timer_config_t *config)
{
    if (!config) return;
    
    config->is_running = true;
    
    /* 使用Pico的alarm功能 */
    add_alarm_in_us(config->period_us, timer_callback, NULL, true);
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
 * 延时函数
 */
void timer_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}
