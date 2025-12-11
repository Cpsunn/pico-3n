/**
 * FX3U 核心 PLC 引擎实现
 */

#include "fx3u_core.h"
#include <string.h>

static fx3u_core_t *g_plc_instance = NULL;

/**
 * 初始化PLC核心
 */
void fx3u_core_init(fx3u_core_t *plc)
{
    if (!plc) return;
    
    memset(plc, 0, sizeof(fx3u_core_t));
    
    plc->state = PLC_STOP;
    plc->scan_time_ms = 200;
    plc->cycle_count = 0;
    plc->program_counter = 0;
    plc->error_code = 0;
    
    /* 初始化特殊寄存器 */
    plc->registers[D8000 - 8000] = 200;   /* 扫描时间 */
    plc->registers[D8001 - 8000] = 0x5EF6; /* FX3U版本 */
    plc->registers[D8002 - 8000] = 16;    /* 内存容量 16KB */
    plc->registers[D8003 - 8000] = 0x0010; /* 存储模式 */
    plc->registers[D8006 - 8000] = 0;     /* CPU错误码 */
    plc->registers[D8010 - 8000] = 10;    /* 扫描次数 */
    plc->registers[D8120 - 8000] = 0x4096; /* 通信方式 */
    plc->registers[D8121 - 8000] = 0;     /* 站号 */
    
    g_plc_instance = plc;
}

/**
 * 启动PLC
 */
void fx3u_core_start(fx3u_core_t *plc)
{
    if (!plc) return;
    plc->state = PLC_RUN;
}

/**
 * 停止PLC
 */
void fx3u_core_stop(fx3u_core_t *plc)
{
    if (!plc) return;
    plc->state = PLC_STOP;
}

/**
 * 运行一个扫描周期
 */
void fx3u_core_run_cycle(fx3u_core_t *plc)
{
    if (!plc || plc->state != PLC_RUN) return;
    
    plc->cycle_count++;
    
    /* 执行用户程序 */
    fx3u_core_execute_program(plc);
    
    /* 更新定时器 */
    for (int i = 0; i < PLC_MAX_TIMERS; i++) {
        if (plc->timers[i].is_running) {
            if (plc->timers[i].current_value < plc->timers[i].preset_value) {
                plc->timers[i].current_value++;
            } else {
                plc->timers[i].is_done = true;
            }
        }
    }
    
    /* 更新计数器 */
    for (int i = 0; i < PLC_MAX_COUNTERS; i++) {
        if (plc->counters[i].is_running) {
            if (plc->counters[i].current_value < plc->counters[i].preset_value) {
                plc->counters[i].current_value++;
            } else {
                plc->counters[i].is_done = true;
            }
        }
    }
}

/**
 * 执行用户程序 (占位符)
 */
void fx3u_core_execute_program(fx3u_core_t *plc)
{
    if (!plc) return;
    
    /* 这里应该执行从Flash或RAM中加载的梯形图程序 */
    /* 由于Pico的Flash大小限制，需要优化程序存储方式 */
}

/**
 * 设置输入继电器
 */
void fx3u_set_input(fx3u_core_t *plc, uint16_t addr, uint8_t value)
{
    if (!plc || addr >= PLC_MAX_INPUTS) return;
    plc->inputs[addr] = value ? 1 : 0;
}

/**
 * 获取输入继电器
 */
uint8_t fx3u_get_input(fx3u_core_t *plc, uint16_t addr)
{
    if (!plc || addr >= PLC_MAX_INPUTS) return 0;
    return plc->inputs[addr];
}

/**
 * 设置输出继电器
 */
void fx3u_set_output(fx3u_core_t *plc, uint16_t addr, uint8_t value)
{
    if (!plc || addr >= PLC_MAX_OUTPUTS) return;
    plc->outputs[addr] = value ? 1 : 0;
}

/**
 * 获取输出继电器
 */
uint8_t fx3u_get_output(fx3u_core_t *plc, uint16_t addr)
{
    if (!plc || addr >= PLC_MAX_OUTPUTS) return 0;
    return plc->outputs[addr];
}

/**
 * 设置内部继电器
 */
void fx3u_set_internal(fx3u_core_t *plc, uint16_t addr, uint8_t value)
{
    if (!plc || addr >= PLC_MAX_INTERNALS) return;
    plc->internals[addr] = value ? 1 : 0;
}

/**
 * 获取内部继电器
 */
uint8_t fx3u_get_internal(fx3u_core_t *plc, uint16_t addr)
{
    if (!plc || addr >= PLC_MAX_INTERNALS) return 0;
    return plc->internals[addr];
}

/**
 * 设置数据寄存器
 */
void fx3u_set_register(fx3u_core_t *plc, uint16_t addr, int16_t value)
{
    if (!plc || addr >= PLC_MAX_REGISTERS) return;
    plc->registers[addr] = value;
}

/**
 * 获取数据寄存器
 */
int16_t fx3u_get_register(fx3u_core_t *plc, uint16_t addr)
{
    if (!plc || addr >= PLC_MAX_REGISTERS) return 0;
    return plc->registers[addr];
}

/**
 * 启动定时器
 */
void fx3u_timer_start(fx3u_core_t *plc, uint16_t timer_num, uint32_t preset)
{
    if (!plc || timer_num >= PLC_MAX_TIMERS) return;
    plc->timers[timer_num].preset_value = preset;
    plc->timers[timer_num].current_value = 0;
    plc->timers[timer_num].is_running = true;
    plc->timers[timer_num].is_done = false;
}

/**
 * 停止定时器
 */
void fx3u_timer_stop(fx3u_core_t *plc, uint16_t timer_num)
{
    if (!plc || timer_num >= PLC_MAX_TIMERS) return;
    plc->timers[timer_num].is_running = false;
    plc->timers[timer_num].current_value = 0;
    plc->timers[timer_num].is_done = false;
}

/**
 * 检查定时器是否完成
 */
bool fx3u_timer_done(fx3u_core_t *plc, uint16_t timer_num)
{
    if (!plc || timer_num >= PLC_MAX_TIMERS) return false;
    return plc->timers[timer_num].is_done;
}

/**
 * 启动计数器
 */
void fx3u_counter_start(fx3u_core_t *plc, uint16_t counter_num, int32_t preset)
{
    if (!plc || counter_num >= PLC_MAX_COUNTERS) return;
    plc->counters[counter_num].preset_value = preset;
    plc->counters[counter_num].current_value = 0;
    plc->counters[counter_num].is_running = true;
    plc->counters[counter_num].is_done = false;
}

/**
 * 复位计数器
 */
void fx3u_counter_reset(fx3u_core_t *plc, uint16_t counter_num)
{
    if (!plc || counter_num >= PLC_MAX_COUNTERS) return;
    plc->counters[counter_num].current_value = 0;
    plc->counters[counter_num].is_done = false;
}

/**
 * 检查计数器是否完成
 */
bool fx3u_counter_done(fx3u_core_t *plc, uint16_t counter_num)
{
    if (!plc || counter_num >= PLC_MAX_COUNTERS) return false;
    return plc->counters[counter_num].is_done;
}

/**
 * 设置错误代码
 */
void fx3u_set_error(fx3u_core_t *plc, uint16_t error_code)
{
    if (!plc) return;
    plc->error_code = error_code;
}

/**
 * 获取错误代码
 */
uint16_t fx3u_get_error(fx3u_core_t *plc)
{
    if (!plc) return 0xFFFF;
    return plc->error_code;
}

/**
 * 清除错误
 */
void fx3u_clear_error(fx3u_core_t *plc)
{
    if (!plc) return;
    plc->error_code = 0;
}
