/**
 * RS485驱动实现
 */

#include "rs485_driver.h"
#include "fx3u_io.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

/**
 * 初始化RS485接口
 */
void rs485_init(rs485_config_t *config)
{
    if (!config) return;
    
    uart_init(uart0, config->baudrate);
    gpio_set_function(PICO_UART_RX_GPIO, GPIO_FUNC_UART);
    gpio_set_function(PICO_UART_TX_GPIO, GPIO_FUNC_UART);
    uart_set_hw_flow(uart0, false, false);
}

/**
 * 设置为接收模式
 */
void rs485_set_receive_mode(void)
{
    io_rs485_set_de(false);  /* 驱动禁用 */
    io_rs485_set_re(true);   /* 接收使能 */
}

/**
 * 设置为发送模式
 */
void rs485_set_transmit_mode(void)
{
    io_rs485_set_re(false);  /* 接收禁用 */
    io_rs485_set_de(true);   /* 驱动使能 */
}

/**
 * 发送数据
 */
void rs485_send(uint8_t *buffer, uint16_t length)
{
    if (!buffer || length == 0) return;
    
    rs485_set_transmit_mode();
    sleep_ms(1);  /* 切换延迟 */
    
    for (int i = 0; i < length; i++) {
        uart_putc(uart0, buffer[i]);
    }
    
    sleep_ms(1);  /* 发送完成延迟 */
    rs485_set_receive_mode();
}

/**
 * 接收数据
 */
int rs485_receive(uint8_t *buffer, uint16_t max_len)
{
    if (!buffer || max_len == 0) return 0;
    
    rs485_set_receive_mode();
    
    int count = 0;
    while (uart_is_readable(uart0) && count < max_len) {
        buffer[count++] = uart_getc(uart0);
    }
    
    return count;
}
