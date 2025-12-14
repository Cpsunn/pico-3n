/**
 * 定时器管理
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t period_us;
    void (*callback)(void);
    bool is_running;
} timer_config_t;

void timer_init(timer_config_t *config);
void timer_start(timer_config_t *config);
void timer_stop(timer_config_t *config);
uint64_t timer_get_ticks(void);
void timer_delay_ms(uint32_t ms);

#endif
