/**
 * 定时器实现（基于 Pico SDK）
 */

#include "timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"

static timer_config_t *g_timer_config = NULL;
static struct repeating_timer g_repeating_timer;
static uint64_t g_timer_ticks = 0;

static bool timer_callback_adapter(repeating_timer_t *rt)
{
    (void)rt;
    
    if (!g_timer_config || !g_timer_config->is_running) {
        return false;
    }
    
    g_timer_ticks++;
    
    if (g_timer_config->callback) {
        g_timer_config->callback();
    }
    
    return g_timer_config->is_running;
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
 * 启动定时器（在本实现中为占位符，使用主循环的定时方式）
 */
void timer_start(timer_config_t *config)
{
    if (!config || config->period_us == 0 || !config->callback) {
        return;
    }
    
    timer_stop(config);
    
    bool ok = add_repeating_timer_us((int64_t)(-((int64_t)config->period_us)),
                                     timer_callback_adapter, NULL,
                                     &g_repeating_timer);
    if (ok) {
        config->is_running = true;
    }
}

/**
 * 停止定时器
 */
void timer_stop(timer_config_t *config)
{
    if (!config) return;
    if (config->is_running) {
        cancel_repeating_timer(&g_repeating_timer);
    }
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
