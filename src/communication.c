/**
 * 串口通信接口实现
 */

#include "communication.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

static comm_config_t *g_comm_config = NULL;
static uint8_t rx_buffer[512];
static uint16_t rx_pos = 0;

/**
 * 初始化通信接口
 */
void comm_init(comm_config_t *config, comm_mode_t mode)
{
    if (!config) return;
    
    config->mode = mode;
    config->baud_rate = BAUD_9600;
    config->parity = PARITY_NONE;
    config->data_bits = 8;
    config->stop_bits = 1;
    config->station_id = 0;
    config->tx_count = 0;
    config->rx_count = 0;
    config->error_count = 0;
    config->is_ready = false;
    
    g_comm_config = config;
    
    /* 初始化UART */
    uart_init(uart0, BAUD_9600);
    uart_set_hw_flow(uart0, false, false);  /* 无硬件流控 */
    
    config->is_ready = true;
}

/**
 * 设置波特率
 */
void comm_set_baudrate(comm_config_t *config, baud_rate_t baud)
{
    if (!config) return;
    config->baud_rate = baud;
    uart_init(uart0, baud);
}

/**
 * 设置奇偶校验
 */
void comm_set_parity(comm_config_t *config, parity_t parity)
{
    if (!config) return;
    config->parity = parity;
}

/**
 * 应用配置
 */
void comm_apply_config(comm_config_t *config)
{
    if (!config) return;
    comm_set_baudrate(config, config->baud_rate);
    comm_set_parity(config, config->parity);
}

/**
 * 接收数据
 */
int comm_receive(uint8_t *buffer, uint16_t max_len)
{
    if (!buffer || max_len == 0) return 0;
    
    int count = 0;
    while (uart_is_readable(uart0) && count < max_len) {
        buffer[count++] = uart_getc(uart0);
    }
    
    if (g_comm_config && count > 0) {
        g_comm_config->rx_count++;
    }
    
    return count;
}

/**
 * 发送数据
 */
int comm_send(uint8_t *buffer, uint16_t len)
{
    if (!buffer || len == 0) return 0;
    
    for (int i = 0; i < len; i++) {
        uart_putc(uart0, buffer[i]);
    }
    
    if (g_comm_config) {
        g_comm_config->tx_count++;
    }
    
    return len;
}

/**
 * 检查是否有可用帧
 */
bool comm_frame_available(void)
{
    return uart_is_readable(uart0);
}

/**
 * 获取通信统计信息
 */
void comm_get_stats(comm_config_t *config)
{
    if (!config) return;
    
    /* 统计信息已在结构体中 */
}
