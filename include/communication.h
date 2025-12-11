/**
 * 通信接口管理 - RS485和以太网适配
 * 支持MODBUS协议和三菱通信协议
 */

#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdint.h>
#include <stdbool.h>

/* ===== 通信模式定义 ===== */
typedef enum {
    COMM_MODE_RS485_MODBUS = 0x00,
    COMM_MODE_RS485_MITSUBISHI = 0x01,
    COMM_MODE_ETHERNET_TCP = 0x02,
    COMM_MODE_ETHERNET_UDP = 0x03
} comm_mode_t;

/* ===== 串口配置 ===== */
typedef enum {
    BAUD_300 = 300,
    BAUD_600 = 600,
    BAUD_1200 = 1200,
    BAUD_2400 = 2400,
    BAUD_4800 = 4800,
    BAUD_9600 = 9600,
    BAUD_19200 = 19200,
    BAUD_38400 = 38400,
    BAUD_56000 = 56000,
    BAUD_57600 = 57600,
    BAUD_115200 = 115200
} baud_rate_t;

typedef enum {
    PARITY_NONE = 0,
    PARITY_ODD = 1,
    PARITY_EVEN = 2
} parity_t;

/* ===== 通信帧结构 ===== */
typedef struct {
    uint8_t *buffer;
    uint16_t length;
    uint16_t max_size;
    bool is_valid;
} comm_frame_t;

/* ===== 通信管理结构体 ===== */
typedef struct {
    comm_mode_t mode;
    baud_rate_t baud_rate;
    parity_t parity;
    uint8_t data_bits;
    uint8_t stop_bits;
    
    uint8_t station_id;
    
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
    
    bool is_ready;
} comm_config_t;

/* ===== 函数声明 ===== */
void comm_init(comm_config_t *config, comm_mode_t mode);
void comm_set_baudrate(comm_config_t *config, baud_rate_t baud);
void comm_set_parity(comm_config_t *config, parity_t parity);

/* 接收和发送 */
int comm_receive(uint8_t *buffer, uint16_t max_len);
int comm_send(uint8_t *buffer, uint16_t len);
bool comm_frame_available(void);

/* 配置函数 */
void comm_apply_config(comm_config_t *config);

/* 统计信息 */
void comm_get_stats(comm_config_t *config);

#endif /* __COMMUNICATION_H__ */
