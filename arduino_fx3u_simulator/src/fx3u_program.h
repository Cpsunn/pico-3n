/**
 * 预置 PLC 程序与辅助初始化
 */

#ifndef __FX3U_PROGRAM_H__
#define __FX3U_PROGRAM_H__

#include <stdint.h>
#include "fx3u_instructions.h"

void fx3u_program_get_default(const fx3u_instruction_t **program,
                              uint32_t *instruction_count);
void fx3u_program_apply_defaults(fx3u_core_t *plc);

#endif /* __FX3U_PROGRAM_H__ */
