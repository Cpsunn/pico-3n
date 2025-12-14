/**
 * MODBUS 协议实现 (RTU/ASCII)
 * 支持MODBUS主机和从机功能
 */

#ifndef __MODBUS_PROTOCOL_H__
#define __MODBUS_PROTOCOL_H__

#include <stdint.h>
#include <stdbool.h>
#include "fx3u_core.h"

/* ===== MODBUS 功能码 ===== */
#define MODBUS_READ_COIL_STATUS         0x01
#define MODBUS_READ_INPUT_STATUS        0x02
#define MODBUS_READ_HOLDING_REGISTERS   0x03
#define MODBUS_READ_INPUT_REGISTERS     0x04
#define MODBUS_WRITE_SINGLE_COIL        0x05
#define MODBUS_WRITE_SINGLE_REGISTER    0x06
#define MODBUS_WRITE_MULTIPLE_COILS     0x0F
#define MODBUS_WRITE_MULTIPLE_REGISTERS 0x10

/* ===== 异常码 ===== */
#define MODBUS_EXCEPTION_INVALID_FUNCTION   0x01
#define MODBUS_EXCEPTION_INVALID_ADDRESS    0x02
#define MODBUS_EXCEPTION_INVALID_VALUE      0x03
#define MODBUS_EXCEPTION_DEVICE_FAILURE     0x04
#define MODBUS_EXCEPTION_ACKNOWLEDGE        0x05
#define MODBUS_EXCEPTION_DEVICE_BUSY        0x06
#define MODBUS_EXCEPTION_NAK                0x07
#define MODBUS_EXCEPTION_MEMORY_ERROR       0x08

/* ===== MODBUS 帧结构 ===== */
typedef struct {
    uint8_t slave_id;
    uint8_t function_code;
    uint16_t start_address;
    uint16_t quantity;
    uint8_t byte_count;
    uint8_t *data;
    uint16_t crc;
    uint16_t lrc;
} modbus_frame_t;

/* ===== MODBUS 主机/从机状态 ===== */
typedef enum {
    MODBUS_IDLE = 0,
    MODBUS_WAIT_RESPONSE = 1,
    MODBUS_ERROR = 2
} modbus_state_t;

/* ===== MODBUS 配置 ===== */
typedef struct {
    uint8_t slave_id;
    bool is_master;
    uint16_t timeout_ms;
    uint16_t frame_delay_ms;
    modbus_state_t state;
} modbus_config_t;

/* ===== 函数声明 ===== */
void modbus_init(modbus_config_t *config, uint8_t slave_id);
void modbus_set_master(modbus_config_t *config, bool is_master);

/* 帧处理 */
bool modbus_parse_frame(uint8_t *buffer, uint16_t length, modbus_frame_t *frame);
int modbus_build_frame(modbus_frame_t *frame, uint8_t *buffer);
bool modbus_validate_frame(uint8_t *buffer, uint16_t length);

/* CRC计算 */
uint16_t modbus_crc16(uint8_t *data, uint16_t length);

/* 从机操作 */
int modbus_slave_process(fx3u_core_t *plc, uint8_t *rx_buffer, uint16_t rx_len, 
                         uint8_t *tx_buffer);

/* 主机操作 */
int modbus_master_read_coils(uint8_t *buffer, uint8_t slave_id, 
                             uint16_t start_addr, uint16_t quantity);
int modbus_master_read_registers(uint8_t *buffer, uint8_t slave_id, 
                                 uint16_t start_addr, uint16_t quantity);
int modbus_master_write_register(uint8_t *buffer, uint8_t slave_id, 
                                 uint16_t address, uint16_t value);
int modbus_master_write_registers(uint8_t *buffer, uint8_t slave_id, 
                                  uint16_t start_addr, uint16_t *values, uint16_t quantity);

/* 错误处理 */
void modbus_send_exception(uint8_t *buffer, uint8_t slave_id, uint8_t function_code, 
                           uint8_t exception_code);
bool modbus_is_exception(uint8_t *buffer, uint16_t length);

#endif /* __MODBUS_PROTOCOL_H__ */
