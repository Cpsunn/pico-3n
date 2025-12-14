/**
 * MODBUS 协议实现
 */

#include "modbus_protocol.h"
#include <string.h>

static bool modbus_check_address(uint16_t start, uint16_t quantity, uint16_t max_count);

/* CRC16查询表 */
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4000
};

static bool modbus_check_address(uint16_t start, uint16_t quantity, uint16_t max_count)
{
    if (quantity == 0) {
        return false;
    }
    if (start >= max_count) {
        return false;
    }
    uint32_t end = (uint32_t)start + (uint32_t)quantity;
    return end <= max_count;
}

/**
 * 初始化MODBUS
 */
void modbus_init(modbus_config_t *config, uint8_t slave_id)
{
    if (!config) return;
    
    config->slave_id = slave_id;
    config->is_master = false;
    config->timeout_ms = 2000;
    config->frame_delay_ms = 100;
    config->state = MODBUS_IDLE;
}

/**
 * 设置主从模式
 */
void modbus_set_master(modbus_config_t *config, bool is_master)
{
    if (!config) return;
    config->is_master = is_master;
}

/**
 * 计算MODBUS CRC16校验码
 */
uint16_t modbus_crc16(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint16_t i;
    
    if (!data || length == 0) return crc;
    
    for (i = 0; i < length; i++) {
        uint8_t index = (uint8_t)(crc ^ data[i]);
        crc = (crc >> 8) ^ crc16_table[index];
    }
    
    return crc;
}

/**
 * 验证MODBUS帧
 */
bool modbus_validate_frame(uint8_t *buffer, uint16_t length)
{
    if (!buffer || length < 4) return false;
    
    /* 检查最小长度 */
    if (length < 4) return false;
    
    /* 计算CRC */
    uint16_t crc_calc = modbus_crc16(buffer, length - 2);
    uint16_t crc_recv = (uint16_t)buffer[length - 2] |
                        ((uint16_t)buffer[length - 1] << 8);
    
    return crc_calc == crc_recv;
}

/**
 * 解析MODBUS帧
 */
bool modbus_parse_frame(uint8_t *buffer, uint16_t length, modbus_frame_t *frame)
{
    if (!buffer || !frame || length < 8) return false;
    
    if (!modbus_validate_frame(buffer, length)) return false;
    
    frame->slave_id = buffer[0];
    frame->function_code = buffer[1];
    frame->start_address = ((uint16_t)buffer[2] << 8) | buffer[3];
    frame->quantity = ((uint16_t)buffer[4] << 8) | buffer[5];
    
    if (length > 6) {
        frame->byte_count = buffer[6];
    }
    
    frame->crc = (uint16_t)buffer[length - 2] |
                 ((uint16_t)buffer[length - 1] << 8);
    frame->is_valid = true;
    
    return true;
}

/**
 * 构建MODBUS帧
 */
int modbus_build_frame(modbus_frame_t *frame, uint8_t *buffer)
{
    if (!frame || !buffer) return 0;
    
    int idx = 0;
    
    buffer[idx++] = frame->slave_id;
    buffer[idx++] = frame->function_code;
    buffer[idx++] = (frame->start_address >> 8) & 0xFF;
    buffer[idx++] = frame->start_address & 0xFF;
    buffer[idx++] = (frame->quantity >> 8) & 0xFF;
    buffer[idx++] = frame->quantity & 0xFF;
    
    if (frame->data && frame->byte_count > 0) {
        buffer[idx++] = frame->byte_count;
        memcpy(&buffer[idx], frame->data, frame->byte_count);
        idx += frame->byte_count;
    }
    
    /* 计算CRC */
    uint16_t crc = modbus_crc16(buffer, idx);
    buffer[idx++] = crc & 0xFF;
    buffer[idx++] = (crc >> 8) & 0xFF;
    
    return idx;
}

/**
 * MODBUS从机处理 - 处理接收到的请求并返回响应
 */
int modbus_slave_process(fx3u_core_t *plc, uint8_t *rx_buffer, uint16_t rx_len,
                         uint8_t *tx_buffer)
{
    if (!plc || !rx_buffer || !tx_buffer || rx_len < 8) return 0;
    
    modbus_frame_t frame;
    if (!modbus_parse_frame(rx_buffer, rx_len, &frame)) {
        return 0;
    }
    
    uint8_t function_code = rx_buffer[1];
    int tx_len = 0;
    
    switch (function_code) {
        case MODBUS_READ_COIL_STATUS: {
            if (!modbus_check_address(frame.start_address, frame.quantity,
                                      PLC_MAX_OUTPUTS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            
            /* 读输出继电器 */
            tx_buffer[0] = rx_buffer[0];
            tx_buffer[1] = function_code;
            uint8_t byte_count = (frame.quantity + 7) / 8;
            tx_buffer[2] = byte_count;
            
            for (int i = 0; i < byte_count; i++) {
                uint8_t byte_val = 0;
                for (int j = 0; j < 8; j++) {
                    uint16_t addr = frame.start_address + i * 8 + j;
                    if (addr < PLC_MAX_OUTPUTS) {
                        if (fx3u_get_output(plc, addr)) {
                            byte_val |= (1 << j);
                        }
                    }
                }
                tx_buffer[3 + i] = byte_val;
            }
            
            uint16_t crc = modbus_crc16(tx_buffer, 3 + byte_count);
            tx_buffer[3 + byte_count] = crc & 0xFF;
            tx_buffer[4 + byte_count] = (crc >> 8) & 0xFF;
            tx_len = 5 + byte_count;
            break;
        }
        
        case MODBUS_READ_INPUT_STATUS: {
            if (!modbus_check_address(frame.start_address, frame.quantity,
                                      PLC_MAX_INPUTS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            
            /* 读输入继电器 */
            tx_buffer[0] = rx_buffer[0];
            tx_buffer[1] = function_code;
            uint8_t byte_count = (frame.quantity + 7) / 8;
            tx_buffer[2] = byte_count;
            
            for (int i = 0; i < byte_count; i++) {
                uint8_t byte_val = 0;
                for (int j = 0; j < 8; j++) {
                    uint16_t addr = frame.start_address + i * 8 + j;
                    if (addr < PLC_MAX_INPUTS) {
                        if (fx3u_get_input(plc, addr)) {
                            byte_val |= (1 << j);
                        }
                    }
                }
                tx_buffer[3 + i] = byte_val;
            }
            
            uint16_t crc = modbus_crc16(tx_buffer, 3 + byte_count);
            tx_buffer[3 + byte_count] = crc & 0xFF;
            tx_buffer[4 + byte_count] = (crc >> 8) & 0xFF;
            tx_len = 5 + byte_count;
            break;
        }
        
        case MODBUS_READ_HOLDING_REGISTERS: {
            if (!modbus_check_address(frame.start_address, frame.quantity,
                                      PLC_MAX_REGISTERS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            
            /* 读数据寄存器 */
            tx_buffer[0] = rx_buffer[0];
            tx_buffer[1] = function_code;
            uint8_t byte_count = frame.quantity * 2;
            tx_buffer[2] = byte_count;
            
            int idx = 3;
            for (int i = 0; i < frame.quantity; i++) {
                uint16_t addr = frame.start_address + i;
                int16_t value = fx3u_get_register(plc, addr);
                tx_buffer[idx++] = (value >> 8) & 0xFF;
                tx_buffer[idx++] = value & 0xFF;
            }
            
            uint16_t crc = modbus_crc16(tx_buffer, 3 + byte_count);
            tx_buffer[idx++] = crc & 0xFF;
            tx_buffer[idx++] = (crc >> 8) & 0xFF;
            tx_len = idx;
            break;
        }
        
        case MODBUS_READ_INPUT_REGISTERS: {
            if (!modbus_check_address(frame.start_address, frame.quantity,
                                      PLC_MAX_REGISTERS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            
            tx_buffer[0] = rx_buffer[0];
            tx_buffer[1] = function_code;
            uint8_t byte_count = frame.quantity * 2;
            tx_buffer[2] = byte_count;
            
            int idx = 3;
            for (int i = 0; i < frame.quantity; i++) {
                uint16_t addr = frame.start_address + i;
                int16_t value = fx3u_get_register(plc, addr);
                tx_buffer[idx++] = (value >> 8) & 0xFF;
                tx_buffer[idx++] = value & 0xFF;
            }
            
            uint16_t crc = modbus_crc16(tx_buffer, 3 + byte_count);
            tx_buffer[idx++] = crc & 0xFF;
            tx_buffer[idx++] = (crc >> 8) & 0xFF;
            tx_len = idx;
            break;
        }
        
        case MODBUS_WRITE_SINGLE_COIL: {
            if (!modbus_check_address(frame.start_address, 1, PLC_MAX_OUTPUTS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            if (rx_len < 8) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_VALUE);
                return 5;
            }
            
            /* 写单个输出继电器 */
            uint16_t raw = ((uint16_t)rx_buffer[4] << 8) | rx_buffer[5];
            if (raw != 0xFF00 && raw != 0x0000) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_VALUE);
                return 5;
            }
            fx3u_set_output(plc, frame.start_address, raw == 0xFF00 ? 1 : 0);
            
            /* 回送相同的请求 */
            memcpy(tx_buffer, rx_buffer, 6);
            uint16_t crc = modbus_crc16(tx_buffer, 6);
            tx_buffer[6] = crc & 0xFF;
            tx_buffer[7] = (crc >> 8) & 0xFF;
            tx_len = 8;
            break;
        }
        
        case MODBUS_WRITE_SINGLE_REGISTER: {
            if (!modbus_check_address(frame.start_address, 1, PLC_MAX_REGISTERS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            if (rx_len < 8) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_VALUE);
                return 5;
            }
            
            /* 写单个寄存器 */
            int16_t value = ((int16_t)rx_buffer[4] << 8) | rx_buffer[5];
            fx3u_set_register(plc, frame.start_address, value);
            
            /* 回送相同的请求 */
            memcpy(tx_buffer, rx_buffer, 6);
            uint16_t crc = modbus_crc16(tx_buffer, 6);
            tx_buffer[6] = crc & 0xFF;
            tx_buffer[7] = (crc >> 8) & 0xFF;
            tx_len = 8;
            break;
        }
        
        case MODBUS_WRITE_MULTIPLE_COILS: {
            if (!modbus_check_address(frame.start_address, frame.quantity,
                                      PLC_MAX_OUTPUTS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            if (rx_len < 9 || rx_buffer[6] == 0 ||
                rx_len != (uint16_t)(rx_buffer[6] + 9)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_VALUE);
                return 5;
            }
            
            /* 写多个输出继电器 */
            uint8_t byte_count = rx_buffer[6];
            
            for (int i = 0; i < byte_count; i++) {
                for (int j = 0; j < 8; j++) {
                    uint16_t addr = frame.start_address + i * 8 + j;
                    if (addr < PLC_MAX_OUTPUTS) {
                        uint8_t value = (rx_buffer[7 + i] >> j) & 1;
                        fx3u_set_output(plc, addr, value);
                    }
                }
            }
            
            /* 返回确认 */
            tx_buffer[0] = rx_buffer[0];
            tx_buffer[1] = function_code;
            tx_buffer[2] = (frame.start_address >> 8) & 0xFF;
            tx_buffer[3] = frame.start_address & 0xFF;
            tx_buffer[4] = (frame.quantity >> 8) & 0xFF;
            tx_buffer[5] = frame.quantity & 0xFF;
            
            uint16_t crc = modbus_crc16(tx_buffer, 6);
            tx_buffer[6] = crc & 0xFF;
            tx_buffer[7] = (crc >> 8) & 0xFF;
            tx_len = 8;
            break;
        }
        
        case MODBUS_WRITE_MULTIPLE_REGISTERS: {
            if (!modbus_check_address(frame.start_address, frame.quantity,
                                      PLC_MAX_REGISTERS)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_ADDRESS);
                return 5;
            }
            if (rx_len < 9 || rx_buffer[6] != frame.quantity * 2 ||
                rx_len != (uint16_t)(frame.quantity * 2 + 9)) {
                modbus_send_exception(tx_buffer, frame.slave_id, function_code,
                                      MODBUS_EXCEPTION_INVALID_VALUE);
                return 5;
            }
            
            /* 写多个寄存器 */
            uint8_t byte_count = rx_buffer[6];
            int qty = byte_count / 2;
            
            for (int i = 0; i < qty; i++) {
                uint16_t addr = frame.start_address + i;
                int16_t value = ((int16_t)rx_buffer[7 + i * 2] << 8) | rx_buffer[8 + i * 2];
                fx3u_set_register(plc, addr, value);
            }
            
            /* 返回确认 */
            tx_buffer[0] = rx_buffer[0];
            tx_buffer[1] = function_code;
            tx_buffer[2] = (frame.start_address >> 8) & 0xFF;
            tx_buffer[3] = frame.start_address & 0xFF;
            tx_buffer[4] = (frame.quantity >> 8) & 0xFF;
            tx_buffer[5] = frame.quantity & 0xFF;
            
            uint16_t crc = modbus_crc16(tx_buffer, 6);
            tx_buffer[6] = crc & 0xFF;
            tx_buffer[7] = (crc >> 8) & 0xFF;
            tx_len = 8;
            break;
        }
        
        default: {
            /* 不支持的功能码 */
            modbus_send_exception(tx_buffer, rx_buffer[0], function_code,
                                 MODBUS_EXCEPTION_INVALID_FUNCTION);
            tx_len = 5;
            break;
        }
    }
    
    return tx_len;
}

/**
 * 发送异常响应
 */
void modbus_send_exception(uint8_t *buffer, uint8_t slave_id, uint8_t function_code,
                           uint8_t exception_code)
{
    if (!buffer) return;
    
    buffer[0] = slave_id;
    buffer[1] = function_code | 0x80;  /* 设置异常位 */
    buffer[2] = exception_code;
    
    uint16_t crc = modbus_crc16(buffer, 3);
    buffer[3] = crc & 0xFF;
    buffer[4] = (crc >> 8) & 0xFF;
}

/**
 * 检查是否为异常响应
 */
bool modbus_is_exception(uint8_t *buffer, uint16_t length)
{
    if (!buffer || length < 3) return false;
    return (buffer[1] & 0x80) != 0;
}

/**
 * MODBUS主机操作 - 读线圈状态
 */
int modbus_master_read_coils(uint8_t *buffer, uint8_t slave_id,
                             uint16_t start_addr, uint16_t quantity)
{
    if (!buffer) return 0;
    
    buffer[0] = slave_id;
    buffer[1] = MODBUS_READ_COIL_STATUS;
    buffer[2] = (start_addr >> 8) & 0xFF;
    buffer[3] = start_addr & 0xFF;
    buffer[4] = (quantity >> 8) & 0xFF;
    buffer[5] = quantity & 0xFF;
    
    uint16_t crc = modbus_crc16(buffer, 6);
    buffer[6] = crc & 0xFF;
    buffer[7] = (crc >> 8) & 0xFF;
    
    return 8;
}

/**
 * MODBUS主机操作 - 读寄存器
 */
int modbus_master_read_registers(uint8_t *buffer, uint8_t slave_id,
                                 uint16_t start_addr, uint16_t quantity)
{
    if (!buffer) return 0;
    
    buffer[0] = slave_id;
    buffer[1] = MODBUS_READ_HOLDING_REGISTERS;
    buffer[2] = (start_addr >> 8) & 0xFF;
    buffer[3] = start_addr & 0xFF;
    buffer[4] = (quantity >> 8) & 0xFF;
    buffer[5] = quantity & 0xFF;
    
    uint16_t crc = modbus_crc16(buffer, 6);
    buffer[6] = crc & 0xFF;
    buffer[7] = (crc >> 8) & 0xFF;
    
    return 8;
}

/**
 * MODBUS主机操作 - 写单个寄存器
 */
int modbus_master_write_register(uint8_t *buffer, uint8_t slave_id,
                                 uint16_t address, uint16_t value)
{
    if (!buffer) return 0;
    
    buffer[0] = slave_id;
    buffer[1] = MODBUS_WRITE_SINGLE_REGISTER;
    buffer[2] = (address >> 8) & 0xFF;
    buffer[3] = address & 0xFF;
    buffer[4] = (value >> 8) & 0xFF;
    buffer[5] = value & 0xFF;
    
    uint16_t crc = modbus_crc16(buffer, 6);
    buffer[6] = crc & 0xFF;
    buffer[7] = (crc >> 8) & 0xFF;
    
    return 8;
}

/**
 * MODBUS主机操作 - 写多个寄存器
 */
int modbus_master_write_registers(uint8_t *buffer, uint8_t slave_id,
                                  uint16_t start_addr, uint16_t *values, uint16_t quantity)
{
    if (!buffer || !values) return 0;
    
    buffer[0] = slave_id;
    buffer[1] = MODBUS_WRITE_MULTIPLE_REGISTERS;
    buffer[2] = (start_addr >> 8) & 0xFF;
    buffer[3] = start_addr & 0xFF;
    buffer[4] = (quantity >> 8) & 0xFF;
    buffer[5] = quantity & 0xFF;
    buffer[6] = quantity * 2;  /* 字节数 */
    
    int idx = 7;
    for (int i = 0; i < quantity; i++) {
        buffer[idx++] = (values[i] >> 8) & 0xFF;
        buffer[idx++] = values[i] & 0xFF;
    }
    
    uint16_t crc = modbus_crc16(buffer, idx);
    buffer[idx++] = crc & 0xFF;
    buffer[idx++] = (crc >> 8) & 0xFF;
    
    return idx;
}
