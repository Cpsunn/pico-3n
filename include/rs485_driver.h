/**
 * RS485驱动程序
 */

#ifndef __RS485_DRIVER_H__
#define __RS485_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t baudrate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    bool rts_enabled;
} rs485_config_t;

void rs485_init(rs485_config_t *config);
void rs485_send(uint8_t *buffer, uint16_t length);
int rs485_receive(uint8_t *buffer, uint16_t max_len);
void rs485_set_receive_mode(void);
void rs485_set_transmit_mode(void);

#endif
