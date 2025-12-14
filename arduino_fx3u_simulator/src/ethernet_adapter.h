/**
 * 以太网适配器接口 (W5500支持)
 */

#ifndef __ETHERNET_ADAPTER_H__
#define __ETHERNET_ADAPTER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t ip[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
    uint8_t mac[6];
    uint16_t port;
} ethernet_config_t;

void ethernet_init(ethernet_config_t *config);
bool ethernet_is_connected(void);
int ethernet_send(uint8_t socket, uint8_t *buffer, uint16_t length);
int ethernet_receive(uint8_t socket, uint8_t *buffer, uint16_t max_len);
void ethernet_set_socket_mode(uint8_t socket, uint8_t mode);

#endif
