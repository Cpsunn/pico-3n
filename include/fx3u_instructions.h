/**
 * FX3U 指令集定义和执行函数
 * 支持基本逻辑指令、定时器、计数器、数据处理指令
 */

#ifndef __FX3U_INSTRUCTIONS_H__
#define __FX3U_INSTRUCTIONS_H__

#include <stdint.h>
#include <stdbool.h>
#include "fx3u_core.h"

/* ===== 地址编码辅助宏 ===== */
#define FX3U_ADDR_MASK          0x0FFF
#define FX3U_ADDR(type, index)  ((uint16_t)(((type) << 12) | ((index) & FX3U_ADDR_MASK)))
#define FX3U_ADDR_X(index)      FX3U_ADDR(0x0, index)
#define FX3U_ADDR_Y(index)      FX3U_ADDR(0x1, index)
#define FX3U_ADDR_M(index)      FX3U_ADDR(0x2, index)
#define FX3U_ADDR_T(index)      FX3U_ADDR(0x3, index)
#define FX3U_ADDR_C(index)      FX3U_ADDR(0x4, index)
#define FX3U_ADDR_D(index)      FX3U_ADDR(0x5, index)

/* ===== 指令操作码 ===== */
typedef enum {
    /* 基础逻辑指令 */
    OP_LD = 0x01,      /* 装入 */
    OP_AND = 0x02,     /* 与 */
    OP_OR = 0x03,      /* 或 */
    OP_NOT = 0x04,     /* 非 */
    OP_OUT = 0x05,     /* 输出 */
    
    /* 定时器和计数器 */
    OP_TMR = 0x10,     /* 定时器 */
    OP_CNT = 0x11,     /* 计数器 */
    
    /* 数据处理 */
    OP_MOV = 0x20,     /* 数据传送 */
    OP_ADD = 0x21,     /* 加法 */
    OP_SUB = 0x22,     /* 减法 */
    OP_MUL = 0x23,     /* 乘法 */
    OP_DIV = 0x24,     /* 除法 */
    OP_CMP = 0x25,     /* 比较 */
    
    /* 位操作 */
    OP_SET = 0x30,     /* 置位 */
    OP_RST = 0x31,     /* 复位 */
    OP_PLS = 0x32,     /* 脉冲 */
    
    /* 其他 */
    OP_NOP = 0xFF      /* 无操作 */
} fx3u_opcode_t;

/* ===== 指令格式 ===== */
typedef struct {
    uint8_t opcode;
    uint16_t operand1;  /* 第一个操作数 (继电器地址等) */
    uint16_t operand2;  /* 第二个操作数 */
    uint16_t operand3;  /* 第三个操作数 */
} fx3u_instruction_t;

/* ===== 寄存器类型 ===== */
typedef enum {
    REG_INPUT = 0x00,   /* 输入 X */
    REG_OUTPUT = 0x01,  /* 输出 Y */
    REG_INTERNAL = 0x02,/* 内部 M */
    REG_TIMER = 0x03,   /* 定时器 T */
    REG_COUNTER = 0x04, /* 计数器 C */
    REG_DATA = 0x05,    /* 数据寄存器 D */
    REG_PULSE = 0x06    /* 脉冲 P */
} fx3u_register_type_t;

/* ===== 函数声明 ===== */
inst_result_t fx3u_execute_instruction(fx3u_core_t *plc, fx3u_instruction_t *inst);

/* 基础逻辑指令 */
inst_result_t fx3u_ld(fx3u_core_t *plc, uint16_t address);
inst_result_t fx3u_and(fx3u_core_t *plc, uint16_t address);
inst_result_t fx3u_or(fx3u_core_t *plc, uint16_t address);
inst_result_t fx3u_not(fx3u_core_t *plc);
inst_result_t fx3u_out(fx3u_core_t *plc, uint16_t address);

/* 定时器和计数器指令 */
inst_result_t fx3u_tmr(fx3u_core_t *plc, uint16_t timer_num, uint16_t preset_addr);
inst_result_t fx3u_cnt(fx3u_core_t *plc, uint16_t counter_num, uint16_t preset_addr);

/* 数据处理指令 */
inst_result_t fx3u_mov(fx3u_core_t *plc, uint16_t src_addr, uint16_t dst_addr);
inst_result_t fx3u_add(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);
inst_result_t fx3u_sub(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);
inst_result_t fx3u_mul(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);
inst_result_t fx3u_div(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);
inst_result_t fx3u_cmp(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2);

/* 位操作指令 */
inst_result_t fx3u_set(fx3u_core_t *plc, uint16_t address);
inst_result_t fx3u_rst(fx3u_core_t *plc, uint16_t address);
inst_result_t fx3u_pls(fx3u_core_t *plc, uint16_t address);

/* 辅助函数 */
uint8_t fx3u_get_bit(fx3u_core_t *plc, uint16_t address);
void fx3u_set_bit(fx3u_core_t *plc, uint16_t address, uint8_t value);
int16_t fx3u_get_word(fx3u_core_t *plc, uint16_t address);
void fx3u_set_word(fx3u_core_t *plc, uint16_t address, int16_t value);

/* 指令译码 */
fx3u_instruction_t* fx3u_decode_instruction(uint8_t *code_buffer, uint32_t offset);

#endif /* __FX3U_INSTRUCTIONS_H__ */
