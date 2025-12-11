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
#include "fx3u_program.h"

/* 全局PLC实例 */
static fx3u_core_t g_plc;
static comm_config_t g_comm_config;
static modbus_config_t g_modbus_config;
static rs485_config_t g_rs485_config;
static io_manager_t g_io_mgr;
static timer_config_t g_cycle_timer_cfg = {
    .period_us = 200000,
    .callback = NULL,
    .is_running = false
};

/* 通信缓冲区 */
static uint8_t rx_buffer[256];
static uint8_t tx_buffer[256];
static uint16_t rx_len = 0;

/**
 * 从物理I/O刷新PLC输入
 */
static void refresh_plc_inputs_from_io(void)
{
    io_manager_update();
    
    for (uint8_t i = 0; i < PICO_TOTAL_INPUTS && i < PLC_MAX_INPUTS; i++) {
        uint8_t value = io_read_input_relay(i);
        fx3u_set_input(&g_plc, i, value);
    }
    
    /* 将模拟输入映射到数据寄存器，便于通过 MODBUS 读取 */
    int16_t ai0_mv = (int16_t)io_adc_to_millivolts(io_read_adc_ai0());
    int16_t ai1_mv = (int16_t)io_adc_to_millivolts(io_read_adc_ai1());
    int16_t pvd_mv = (int16_t)io_adc_to_millivolts(io_read_adc_pvd());
    fx3u_set_register(&g_plc, 110, ai0_mv);
    fx3u_set_register(&g_plc, 111, ai1_mv);
    fx3u_set_register(&g_plc, 112, pvd_mv);
    
    static bool last_run_switch = false;
    static bool switch_initialized = false;
    bool run_switch = io_get_switch_run();
    
    if (!switch_initialized) {
        switch_initialized = true;
        last_run_switch = run_switch;
    }
    
    if (run_switch != last_run_switch) {
        if (run_switch) {
            fx3u_core_start(&g_plc);
            printf("[IO] RUN开关闭合，PLC启动\r\n");
        } else {
            fx3u_core_stop(&g_plc);
            printf("[IO] RUN开关断开，PLC停止\r\n");
        }
        last_run_switch = run_switch;
    } else if (!run_switch && g_plc.state == PLC_RUN) {
        /* 未检测到切换但RUN开关断开，强制停止使物理开关优先 */
        fx3u_core_stop(&g_plc);
    } else if (run_switch && g_plc.state != PLC_RUN) {
        fx3u_core_start(&g_plc);
    }
}

/**
 * 将PLC输出同步到物理I/O
 */
static void apply_plc_outputs_to_io(void)
{
    for (uint8_t i = 0; i < PICO_OUTPUT_COUNT && i < PLC_MAX_OUTPUTS; i++) {
        uint8_t value = fx3u_get_output(&g_plc, i);
        io_write_output_relay(i, value);
    }
    
    io_set_led_run(g_plc.state == PLC_RUN);
    io_set_led_err(g_plc.error_code != 0);
}

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
    
    const fx3u_instruction_t *program = NULL;
    uint32_t instruction_count = 0;
    fx3u_program_get_default(&program, &instruction_count);
    if (fx3u_core_load_program(&g_plc, program, instruction_count)) {
        fx3u_program_apply_defaults(&g_plc);
        printf("Loaded default ladder program (%lu instructions)\r\n",
               (unsigned long)instruction_count);
    } else {
        printf("Warning: no PLC program loaded\r\n");
    }
    
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
    g_cycle_timer_cfg.callback = plc_cycle_callback;
    timer_init(&g_cycle_timer_cfg);
    timer_start(&g_cycle_timer_cfg);
    
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
        /* 刷新物理输入 */
        refresh_plc_inputs_from_io();
        
        /* 处理通信 */
        process_communication();
        
        /* 处理本地命令 */
        process_local_command();
        
        /* 将PLC输出映射到GPIO */
        apply_plc_outputs_to_io();
        
        /* 低优先级处理 */
        tight_loop_contents();
    }
    
    return 0;
}
