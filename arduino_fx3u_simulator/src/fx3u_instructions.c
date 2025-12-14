/**
 * FX3U 指令集实现
 */

#include "fx3u_instructions.h"
#include <stddef.h>

/* 指令执行状态机 */
static struct {
    bool bus_state;  /* 当前逻辑线状态 */
    bool block_out;  /* 逻辑块输出 */
} g_exec_context;

/**
 * 执行单条指令
 */
inst_result_t fx3u_execute_instruction(fx3u_core_t *plc, fx3u_instruction_t *inst)
{
    if (!plc || !inst) return INST_INVALID;
    
    switch (inst->opcode) {
        case OP_LD:
            return fx3u_ld(plc, inst->operand1);
        case OP_AND:
            return fx3u_and(plc, inst->operand1);
        case OP_OR:
            return fx3u_or(plc, inst->operand1);
        case OP_NOT:
            return fx3u_not(plc);
        case OP_OUT:
            return fx3u_out(plc, inst->operand1);
        case OP_TMR:
            return fx3u_tmr(plc, inst->operand1, inst->operand2);
        case OP_CNT:
            return fx3u_cnt(plc, inst->operand1, inst->operand2);
        case OP_MOV:
            return fx3u_mov(plc, inst->operand1, inst->operand2);
        case OP_ADD:
            return fx3u_add(plc, inst->operand1, inst->operand2, inst->operand3);
        case OP_SUB:
            return fx3u_sub(plc, inst->operand1, inst->operand2, inst->operand3);
        case OP_MUL:
            return fx3u_mul(plc, inst->operand1, inst->operand2, inst->operand3);
        case OP_DIV:
            return fx3u_div(plc, inst->operand1, inst->operand2, inst->operand3);
        case OP_CMP:
            return fx3u_cmp(plc, inst->operand1, inst->operand2);
        case OP_SET:
            return fx3u_set(plc, inst->operand1);
        case OP_RST:
            return fx3u_rst(plc, inst->operand1);
        case OP_PLS:
            return fx3u_pls(plc, inst->operand1);
        case OP_NOP:
            return INST_OK;
        default:
            return INST_INVALID;
    }
}

/**
 * LD 装入指令 - 从继电器获取一个新的逻辑线
 */
inst_result_t fx3u_ld(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    g_exec_context.bus_state = fx3u_get_bit(plc, address) ? true : false;
    return INST_OK;
}

/**
 * AND 与指令 - 并联
 */
inst_result_t fx3u_and(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    uint8_t bit_value = fx3u_get_bit(plc, address);
    g_exec_context.bus_state = g_exec_context.bus_state && (bit_value ? true : false);
    return INST_OK;
}

/**
 * OR 或指令 - 串联
 */
inst_result_t fx3u_or(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    uint8_t bit_value = fx3u_get_bit(plc, address);
    g_exec_context.bus_state = g_exec_context.bus_state || (bit_value ? true : false);
    return INST_OK;
}

/**
 * NOT 反向 - 逻辑反向
 */
inst_result_t fx3u_not(fx3u_core_t *plc)
{
    if (!plc) return INST_INVALID;
    g_exec_context.bus_state = !g_exec_context.bus_state;
    return INST_OK;
}

/**
 * OUT 输出指令
 */
inst_result_t fx3u_out(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    fx3u_set_bit(plc, address, g_exec_context.bus_state ? 1 : 0);
    return INST_OK;
}

/**
 * TMR 定时器指令
 */
inst_result_t fx3u_tmr(fx3u_core_t *plc, uint16_t timer_num, uint16_t preset_addr)
{
    if (!plc || timer_num >= PLC_MAX_TIMERS) return INST_INVALID;
    
    int16_t preset = fx3u_get_word(plc, preset_addr);
    
    if (g_exec_context.bus_state) {
        if (!plc->timers[timer_num].is_running) {
            fx3u_timer_start(plc, timer_num, preset);
        }
    } else {
        fx3u_timer_stop(plc, timer_num);
    }
    
    /* 输出定时器状态 */
    g_exec_context.bus_state = fx3u_timer_done(plc, timer_num);
    return INST_OK;
}

/**
 * CNT 计数器指令
 */
inst_result_t fx3u_cnt(fx3u_core_t *plc, uint16_t counter_num, uint16_t preset_addr)
{
    if (!plc || counter_num >= PLC_MAX_COUNTERS) return INST_INVALID;
    
    int16_t preset = fx3u_get_word(plc, preset_addr);
    
    if (g_exec_context.bus_state) {
        if (!plc->counters[counter_num].is_running) {
            fx3u_counter_start(plc, counter_num, preset);
        }
    } else {
        fx3u_counter_reset(plc, counter_num);
    }
    
    /* 输出计数器状态 */
    g_exec_context.bus_state = fx3u_counter_done(plc, counter_num);
    return INST_OK;
}

/**
 * MOV 数据传送指令
 */
inst_result_t fx3u_mov(fx3u_core_t *plc, uint16_t src_addr, uint16_t dst_addr)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        int16_t value = fx3u_get_word(plc, src_addr);
        fx3u_set_word(plc, dst_addr, value);
    }
    
    return INST_OK;
}

/**
 * ADD 加法指令
 */
inst_result_t fx3u_add(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        int16_t val1 = fx3u_get_word(plc, addr1);
        int16_t val2 = fx3u_get_word(plc, addr2);
        int16_t result = val1 + val2;
        fx3u_set_word(plc, result_addr, result);
    }
    
    return INST_OK;
}

/**
 * SUB 减法指令
 */
inst_result_t fx3u_sub(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        int16_t val1 = fx3u_get_word(plc, addr1);
        int16_t val2 = fx3u_get_word(plc, addr2);
        int16_t result = val1 - val2;
        fx3u_set_word(plc, result_addr, result);
    }
    
    return INST_OK;
}

/**
 * MUL 乘法指令
 */
inst_result_t fx3u_mul(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        int16_t val1 = fx3u_get_word(plc, addr1);
        int16_t val2 = fx3u_get_word(plc, addr2);
        int32_t result = (int32_t)val1 * val2;
        fx3u_set_word(plc, result_addr, (int16_t)(result & 0xFFFF));
    }
    
    return INST_OK;
}

/**
 * DIV 除法指令
 */
inst_result_t fx3u_div(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        int16_t val1 = fx3u_get_word(plc, addr1);
        int16_t val2 = fx3u_get_word(plc, addr2);
        
        if (val2 != 0) {
            int16_t result = val1 / val2;
            fx3u_set_word(plc, result_addr, result);
        } else {
            fx3u_set_error(plc, 0x0001);  /* 除以零错误 */
        }
    }
    
    return INST_OK;
}

/**
 * CMP 比较指令
 */
inst_result_t fx3u_cmp(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2)
{
    if (!plc) return INST_INVALID;
    
    int16_t val1 = fx3u_get_word(plc, addr1);
    int16_t val2 = fx3u_get_word(plc, addr2);
    g_exec_context.bus_state = (val1 == val2);
    
    return INST_OK;
}

/**
 * SET 置位指令 - 将继电器置为1
 */
inst_result_t fx3u_set(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        fx3u_set_bit(plc, address, 1);
    }
    
    return INST_OK;
}

/**
 * RST 复位指令 - 将继电器复位为0
 */
inst_result_t fx3u_rst(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    
    if (g_exec_context.bus_state) {
        fx3u_set_bit(plc, address, 0);
    }
    
    return INST_OK;
}

/**
 * PLS 脉冲指令 - 在上升沿执行一次
 */
inst_result_t fx3u_pls(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return INST_INVALID;
    
    static uint8_t last_state = 0;
    
    if (g_exec_context.bus_state && !last_state) {
        /* 上升沿 */
        fx3u_set_bit(plc, address, 1);
    } else {
        fx3u_set_bit(plc, address, 0);
    }
    
    last_state = g_exec_context.bus_state;
    return INST_OK;
}

/**
 * 获取位值 (通用)
 */
uint8_t fx3u_get_bit(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return 0;
    
    /* 解析地址格式 XXX */
    uint8_t type = (address >> 12) & 0x0F;
    uint16_t num = address & 0x0FFF;
    
    switch (type) {
        case 0: /* X */
            if (num < PLC_MAX_INPUTS)
                return fx3u_get_input(plc, num);
            break;
        case 1: /* Y */
            if (num < PLC_MAX_OUTPUTS)
                return fx3u_get_output(plc, num);
            break;
        case 2: /* M */
            if (num < PLC_MAX_INTERNALS)
                return fx3u_get_internal(plc, num);
            break;
        case 3: /* T */
            if (num < PLC_MAX_TIMERS)
                return fx3u_timer_done(plc, num) ? 1 : 0;
            break;
        case 4: /* C */
            if (num < PLC_MAX_COUNTERS)
                return fx3u_counter_done(plc, num) ? 1 : 0;
            break;
    }
    
    return 0;
}

/**
 * 设置位值 (通用)
 */
void fx3u_set_bit(fx3u_core_t *plc, uint16_t address, uint8_t value)
{
    if (!plc) return;
    
    uint8_t type = (address >> 12) & 0x0F;
    uint16_t num = address & 0x0FFF;
    
    switch (type) {
        case 1: /* Y */
            if (num < PLC_MAX_OUTPUTS)
                fx3u_set_output(plc, num, value);
            break;
        case 2: /* M */
            if (num < PLC_MAX_INTERNALS)
                fx3u_set_internal(plc, num, value);
            break;
    }
}

/**
 * 获取字值 (通用)
 */
int16_t fx3u_get_word(fx3u_core_t *plc, uint16_t address)
{
    if (!plc) return 0;
    
    uint8_t type = (address >> 12) & 0x0F;
    uint16_t num = address & 0x0FFF;
    
    if (type == 5 && num < PLC_MAX_REGISTERS) {
        return fx3u_get_register(plc, num);
    }
    
    return 0;
}

/**
 * 设置字值 (通用)
 */
void fx3u_set_word(fx3u_core_t *plc, uint16_t address, int16_t value)
{
    if (!plc) return;
    
    uint8_t type = (address >> 12) & 0x0F;
    uint16_t num = address & 0x0FFF;
    
    if (type == 5 && num < PLC_MAX_REGISTERS) {
        fx3u_set_register(plc, num, value);
    }
}

/**
 * 指令译码 (占位符 - 可根据编码方式自定义)
 */
fx3u_instruction_t* fx3u_decode_instruction(uint8_t *code_buffer, uint32_t offset)
{
    if (!code_buffer) return NULL;
    
    static fx3u_instruction_t inst;
    
    /* 简单的指令解码示例 */
    inst.opcode = code_buffer[offset];
    inst.operand1 = (code_buffer[offset + 1] << 8) | code_buffer[offset + 2];
    inst.operand2 = (code_buffer[offset + 3] << 8) | code_buffer[offset + 4];
    inst.operand3 = (code_buffer[offset + 5] << 8) | code_buffer[offset + 6];
    
    return &inst;
}
