/**
 * 默认 PLC 程序与初始化
 */

#include "fx3u_program.h"

static const fx3u_instruction_t g_default_program[] = {
    /* Y0 直接跟随 X0 */
    {OP_LD,  FX3U_ADDR_X(0), 0, 0},
    {OP_OUT, FX3U_ADDR_Y(0), 0, 0},
    
    /* X1 触发定时器 T0，定时时间存储在 D100，超时后驱动 Y1 */
    {OP_LD,  FX3U_ADDR_X(1), 0, 0},
    {OP_TMR, 0, FX3U_ADDR_D(100), 0},
    {OP_OUT, FX3U_ADDR_Y(1), 0, 0},
    
    /* X2/X3 控制的保持线圈 M0 -> Y2 */
    {OP_LD,  FX3U_ADDR_X(2), 0, 0},
    {OP_SET, FX3U_ADDR_M(0), 0, 0},
    {OP_LD,  FX3U_ADDR_X(3), 0, 0},
    {OP_RST, FX3U_ADDR_M(0), 0, 0},
    {OP_LD,  FX3U_ADDR_M(0), 0, 0},
    {OP_OUT, FX3U_ADDR_Y(2), 0, 0},
    
    /* X4 置位时，将 ADC 采样 (D110) 拷贝到 D120 */
    {OP_LD,  FX3U_ADDR_X(4), 0, 0},
    {OP_MOV, FX3U_ADDR_D(110), FX3U_ADDR_D(120), 0},
    
    /* X5 置位时，将 D120 + D121 运算结果写入 D122 */
    {OP_LD,  FX3U_ADDR_X(5), 0, 0},
    {OP_ADD, FX3U_ADDR_D(120), FX3U_ADDR_D(121), FX3U_ADDR_D(122)},
    
    /* X6 检测上升沿，驱动脉冲 Y3 */
    {OP_LD,  FX3U_ADDR_X(6), 0, 0},
    {OP_PLS, FX3U_ADDR_Y(3), 0, 0}
};

void fx3u_program_get_default(const fx3u_instruction_t **program,
                              uint32_t *instruction_count)
{
    if (program) {
        *program = g_default_program;
    }
    if (instruction_count) {
        *instruction_count = (uint32_t)(sizeof(g_default_program) /
                                        sizeof(g_default_program[0]));
    }
}

void fx3u_program_apply_defaults(fx3u_core_t *plc)
{
    if (!plc) {
        return;
    }
    
    fx3u_set_register(plc, 100, 5);    /* D100: 定时器预置值 (5 个扫描周期) */
    fx3u_set_register(plc, 120, 0);    /* D120: ADC 镜像初始值 */
    fx3u_set_register(plc, 121, 50);   /* D121: 累加偏移 */
    fx3u_set_register(plc, 122, 0);    /* D122: 运算结果 */
}
