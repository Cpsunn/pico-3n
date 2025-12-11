/**
 * Pico FX3U 模拟器主程序
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/uart.h"
#include "fx3u_core.h"
#include "fx3u_instructions.h"
#include "fx3u_io.h"
#include "communication.h"
#include "modbus_protocol.h"
#include "rs485_driver.h"
#include "timer.h"

/* 全局PLC实例 */
static fx3u_core_t g_plc;
static comm_config_t g_comm_config;
static modbus_config_t g_modbus_config;
static rs485_config_t g_rs485_config;
static io_manager_t g_io_mgr;

/* 通信缓冲区 */
static uint8_t rx_buffer[256];
static uint8_t tx_buffer[256];
static uint16_t rx_len = 0;

/**
 * PLC循环扫描定时器回调
 */
static void plc_cycle_callback(void)
{
    if (g_plc.state == PLC_RUN) {
        fx3u_core_run_cycle(&g_plc);
    }
}

/**
 * 初始化所有模块
 */
static void system_init(void)
{
    /* 标准输入输出初始化 */
    stdio_init_all();
    sleep_ms(1000);
    printf("=== Pico FX3U Simulator v1.0 ===\r\n");
    
    /* PLC核心初始化 */
    printf("Initializing PLC core...\r\n");
    fx3u_core_init(&g_plc);
    
    /* I/O管理器初始化 */
    printf("Initializing I/O manager...\r\n");
    io_manager_init(&g_io_mgr);
    
    /* RS485通信初始化 */
    printf("Initializing RS485 communication...\r\n");
    g_rs485_config.baudrate = 9600;
    g_rs485_config.data_bits = 8;
    g_rs485_config.stop_bits = 1;
    g_rs485_config.parity = 0;  /* NONE */
    g_rs485_config.rts_enabled = true;
    rs485_init(&g_rs485_config);
    
    /* 通信配置初始化 */
    printf("Initializing communication interface...\r\n");
    comm_init(&g_comm_config, COMM_MODE_RS485_MODBUS);
    g_comm_config.station_id = 1;
    
    /* MODBUS协议初始化 */
    printf("Initializing MODBUS protocol...\r\n");
    modbus_init(&g_modbus_config, 1);
    modbus_set_master(&g_modbus_config, false);  /* 从机模式 */
    
    /* PLC定时器回调 */
    timer_config_t timer_cfg = {
        .period_us = 200000,  /* 200ms扫描周期 */
        .callback = plc_cycle_callback,
        .is_running = false
    };
    timer_init(&timer_cfg);
    
    printf("System initialization completed.\r\n");
    printf("PLC Status: Ready\r\n");
    printf("RS485 Baudrate: 9600, Station ID: 1\r\n\r\n");
}

/**
 * 处理接收到的通信帧
 */
static void process_communication(void)
{
    /* 接收RS485数据 */
    rx_len = rs485_receive(rx_buffer, sizeof(rx_buffer));
    
    if (rx_len > 0) {
        printf("Received %d bytes\r\n", rx_len);
        
        /* 处理MODBUS帧 */
        int tx_len = modbus_slave_process(&g_plc, rx_buffer, rx_len, tx_buffer);
        
        if (tx_len > 0) {
            printf("Sending %d bytes\r\n", tx_len);
            rs485_send(tx_buffer, tx_len);
        }
    }
}

/**
 * 处理本地命令 (来自USB)
 */
static void process_local_command(void)
{
    if (stdio_usb_connected()) {
        int c = getchar();
        if (c != EOF) {
            switch (c) {
                case 's':  /* Start PLC */
                    fx3u_core_start(&g_plc);
                    printf("PLC started\r\n");
                    break;
                    
                case 't':  /* Stop PLC */
                    fx3u_core_stop(&g_plc);
                    printf("PLC stopped\r\n");
                    break;
                    
                case 'd':  /* Dump status */
                    printf("PLC Status: %s\r\n", g_plc.state == PLC_RUN ? "RUNNING" : "STOPPED");
                    printf("Cycle Count: %lu\r\n", g_plc.cycle_count);
                    printf("Error Code: 0x%04X\r\n", g_plc.error_code);
                    printf("Scan Time: %lums\r\n", g_plc.scan_time_ms);
                    break;
                    
                case 'r':  /* Reset */
                    memset(&g_plc, 0, sizeof(fx3u_core_t));
                    fx3u_core_init(&g_plc);
                    printf("PLC reset\r\n");
                    break;
                    
                case '?':  /* Help */
                    printf("Commands:\r\n");
                    printf("  s - Start PLC\r\n");
                    printf("  t - Stop PLC\r\n");
                    printf("  d - Dump status\r\n");
                    printf("  r - Reset PLC\r\n");
                    printf("  ? - Help\r\n");
                    break;
                    
                default:
                    printf("Unknown command: %c\r\n", (char)c);
                    break;
            }
        }
    }
}

/**
 * 主循环
 */
int main(void)
{
    system_init();
    
    /* 启动PLC */
    fx3u_core_start(&g_plc);
    
    printf("Entering main loop...\r\n");
    
    while (1) {
        /* 处理通信 */
        process_communication();
        
        /* 处理本地命令 */
        process_local_command();
        
        /* PLC定时运行 */
        if (g_plc.state == PLC_RUN) {
            static uint32_t last_cycle_time = 0;
            uint32_t now = to_ms_since_boot();
            
            if (now - last_cycle_time >= 200) {  /* 200ms扫描周期 */
                fx3u_core_run_cycle(&g_plc);
                last_cycle_time = now;
            }
        }
        
        /* 低优先级处理 */
        tight_loop_contents();
    }
    
    return 0;
}
