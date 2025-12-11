/**
 * FX3U Core Simulator - 为 Raspberry Pi Pico 优化
 * 基于 STM32 FX3U 模拟固件的 Pico 适配版本
 * 
 * 核心PLC执行引擎
 */

#ifndef __FX3U_CORE_H__
#define __FX3U_CORE_H__

#include <stdint.h>
#include <stdbool.h>

/* 前置声明，避免头文件循环依赖 */
struct fx3u_instruction_t;
typedef struct fx3u_instruction_t fx3u_instruction_t;

/* ===== PLC 内存映射 ===== */
#define PLC_MAX_INPUTS      256     /* 输入继电器 X */
#define PLC_MAX_OUTPUTS     256     /* 输出继电器 Y */
#define PLC_MAX_INTERNALS   2048    /* 内部继电器 M */
#define PLC_MAX_TIMERS      128     /* 定时器 T */
#define PLC_MAX_COUNTERS    128     /* 计数器 C */
#define PLC_MAX_REGISTERS   4096    /* 数据寄存器 D */

/* 特殊寄存器定义 */
#define D8000   8000    /* 扫描时间 */
#define D8001   8001    /* PLC版本 */
#define D8002   8002    /* 内存容量 */
#define D8003   8003    /* 存储模式 */
#define D8006   8006    /* CPU错误代码 */
#define D8010   8010    /* 扫描次数 */
#define D8011   8011    /* 扫描最小时间 */
#define D8012   8012    /* 扫描最大时间 */
#define D8120   8120    /* 通信方式 */
#define D8121   8121    /* 站号 */
#define D8127   8127    /* 数据长度设置 */
#define D8128   8128    /* 校验码设置 */

#define M8126   8126    /* 全部清除标志 */
#define M8127   8127    /* 通信校验错误信号 */
#define M8128   8128    /* 存储完成标志 */
#define M8129   8129    /* 通信故障标志 */

/* ===== 定时器和计数器状态 ===== */
typedef struct {
    uint64_t elapsed_us;
    uint32_t preset_ms;
    bool is_running;
    bool is_done;
} fx3u_timer_t;

typedef struct {
    int32_t current_value;
    int32_t preset_value;
    bool is_running;
    bool is_done;
} fx3u_counter_t;

/* ===== PLC运行状态 ===== */
typedef enum {
    PLC_STOP = 0,
    PLC_RUN = 1,
    PLC_PAUSE = 2
} plc_state_t;

/* ===== 指令执行结果 ===== */
typedef enum {
    INST_OK = 0,
    INST_ERROR = 1,
    INST_INVALID = 2,
    INST_OVERFLOW = 3
} inst_result_t;

/* ===== PLC核心结构体 ===== */
typedef struct {
    /* 运行状态 */
    plc_state_t state;
    uint32_t scan_time_ms;
    uint32_t cycle_count;
    const fx3u_instruction_t *program;
    
    /* 继电器内存 */
    uint8_t inputs[PLC_MAX_INPUTS];
    uint8_t outputs[PLC_MAX_OUTPUTS];
    uint8_t internals[PLC_MAX_INTERNALS];
    
    /* 定时器和计数器 */
    fx3u_timer_t timers[PLC_MAX_TIMERS];
    fx3u_counter_t counters[PLC_MAX_COUNTERS];
    
    /* 数据寄存器 */
    int16_t registers[PLC_MAX_REGISTERS];
    
    /* 执行位置 */
    uint32_t program_counter;
    uint32_t program_size;
    
    /* 错误处理 */
    uint16_t error_code;
    
    /* 时间管理 */
    uint32_t last_scan_time_us;
    uint32_t min_scan_time_us;
    uint32_t max_scan_time_us;
} fx3u_core_t;

/* ===== 函数声明 ===== */
void fx3u_core_init(fx3u_core_t *plc);
void fx3u_core_start(fx3u_core_t *plc);
void fx3u_core_stop(fx3u_core_t *plc);
void fx3u_core_run_cycle(fx3u_core_t *plc);
void fx3u_core_execute_program(fx3u_core_t *plc);
bool fx3u_core_load_program(fx3u_core_t *plc,
                            const fx3u_instruction_t *program,
                            uint32_t instruction_count);
bool fx3u_core_has_program(const fx3u_core_t *plc);

/* 继电器访问函数 */
void fx3u_set_input(fx3u_core_t *plc, uint16_t addr, uint8_t value);
uint8_t fx3u_get_input(fx3u_core_t *plc, uint16_t addr);
void fx3u_set_output(fx3u_core_t *plc, uint16_t addr, uint8_t value);
uint8_t fx3u_get_output(fx3u_core_t *plc, uint16_t addr);
void fx3u_set_internal(fx3u_core_t *plc, uint16_t addr, uint8_t value);
uint8_t fx3u_get_internal(fx3u_core_t *plc, uint16_t addr);

/* 数据寄存器访问函数 */
void fx3u_set_register(fx3u_core_t *plc, uint16_t addr, int16_t value);
int16_t fx3u_get_register(fx3u_core_t *plc, uint16_t addr);

/* 定时器和计数器函数 */
void fx3u_timer_start(fx3u_core_t *plc, uint16_t timer_num, uint32_t preset);
void fx3u_timer_stop(fx3u_core_t *plc, uint16_t timer_num);
bool fx3u_timer_done(fx3u_core_t *plc, uint16_t timer_num);
void fx3u_counter_start(fx3u_core_t *plc, uint16_t counter_num, int32_t preset);
void fx3u_counter_reset(fx3u_core_t *plc, uint16_t counter_num);
bool fx3u_counter_done(fx3u_core_t *plc, uint16_t counter_num);

/* 错误管理 */
void fx3u_set_error(fx3u_core_t *plc, uint16_t error_code);
uint16_t fx3u_get_error(fx3u_core_t *plc);
void fx3u_clear_error(fx3u_core_t *plc);

#endif /* __FX3U_CORE_H__ */
