/**
 * FX3U I/O 管理实现（基于物理引脚映射）
 * 
 * 硬件映射：
 * - UART0 TX: GPIO0 (Pin 1)
 * - UART0 RX: GPIO1 (Pin 2)
 * - RUN_LED: GPIO2 (Pin 4)
 * - ERR_LED: GPIO3 (Pin 5)
 * - Y7/Y8: GPIO4/5 (Pin 6-7)
 * - X0-X9: GPIO6-15 (Pin 9-10, 11-12, 14-17, 19-20)
 * - RUN: GPIO23 (Pin 30)
 * - Y0-Y6: GPIO16-22 (Pin 21-29)
 * - AI0: GPIO26 (Pin 31, ADC0)
 * - AI1: GPIO27 (Pin 32, ADC1)
 * - PVD: GPIO28 (Pin 34, ADC2)
 * 
 * 电气特性：
 * - 工作电压：3.3V (3.0..3.6V)
 * - GPIO 驱动：单个 ≤12mA，推荐；绝对最大 16mA
 * - 输入去抖：20ms (可配置)
 * - ADC：12-bit, 0..3.3V, 3 通道
 */

#include "fx3u_io.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "pico/time.h"
#include <stdio.h>
#include <string.h>

static io_manager_t *g_io_mgr = NULL;

/* 去抖动计时器 */
static uint32_t g_input_debounce_time[PICO_TOTAL_INPUTS];
static uint8_t g_input_stable[PICO_TOTAL_INPUTS];

/**
 * io_manager_init - 初始化所有 I/O 端口
 * 
 * 功能：
 * - 初始化所有 GPIO（输入/输出）
 * - 设置方向与上拉/下拉
 * - 初始化 UART0 与 RS485 控制
 * - 初始化 ADC 通道
 * - 初始化 LED 与开关
 */
void io_manager_init(io_manager_t *io_mgr)
{
    if (!io_mgr) return;
    
    memset(io_mgr, 0, sizeof(*io_mgr));
    g_io_mgr = io_mgr;
    
    /* ===== 初始化数字输入 (X0-X9) ===== */
    uint8_t input_gpios[] = {
        PICO_INPUT_X0_GPIO,  PICO_INPUT_X1_GPIO,  PICO_INPUT_X2_GPIO,
        PICO_INPUT_X3_GPIO,  PICO_INPUT_X4_GPIO,  PICO_INPUT_X5_GPIO,
        PICO_INPUT_X6_GPIO,  PICO_INPUT_X7_GPIO,  PICO_INPUT_X8_GPIO,
        PICO_INPUT_X9_GPIO
    };
    
    for (int i = 0; i < PICO_TOTAL_INPUTS; i++) {
        uint8_t gpio = input_gpios[i];
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_IN);
        
        if (PICO_INPUT_PULL_UP) {
            gpio_pull_up(gpio);  /* 建议上拉，若接地闭合 */
        } else {
            gpio_pull_down(gpio);
        }
        
        g_io_mgr->input_gpio_mask |= (1 << gpio);
        g_input_stable[i] = 0;
        g_input_debounce_time[i] = 0;
    }
    
    /* ===== 初始化数字输出 (Y0-Y8) ===== */
    uint8_t output_gpios[] = {
        PICO_OUTPUT_Y0_GPIO,  PICO_OUTPUT_Y1_GPIO,  PICO_OUTPUT_Y2_GPIO,
        PICO_OUTPUT_Y3_GPIO,  PICO_OUTPUT_Y4_GPIO,  PICO_OUTPUT_Y5_GPIO,
        PICO_OUTPUT_Y6_GPIO,  PICO_OUTPUT_Y7_GPIO,  PICO_OUTPUT_Y8_GPIO
    };
    
    for (int i = 0; i < PICO_OUTPUT_COUNT; i++) {
        uint8_t gpio = output_gpios[i];
        gpio_init(gpio);
        gpio_set_dir(gpio, GPIO_OUT);
        gpio_put(gpio, 0);  /* 初始化为低电平 */
        g_io_mgr->output_gpio_mask |= (1 << gpio);
    }
    
    /* ===== 初始化 RUN LED (GPIO2) ===== */
    gpio_init(PICO_LED_RUN_GPIO);
    gpio_set_dir(PICO_LED_RUN_GPIO, GPIO_OUT);
    gpio_put(PICO_LED_RUN_GPIO, PICO_LED_ACTIVE_HIGH ? 0 : 1);  /* 初始关闭 */
    g_io_mgr->led_run_state = false;
    
    /* ===== 初始化 ERR LED (GPIO3) ===== */
    gpio_init(PICO_LED_ERR_GPIO);
    gpio_set_dir(PICO_LED_ERR_GPIO, GPIO_OUT);
    gpio_put(PICO_LED_ERR_GPIO, PICO_LED_ACTIVE_HIGH ? 0 : 1);  /* 初始关闭 */
    g_io_mgr->led_err_state = false;
    
    /* ===== 初始化 RUN 开关 (GPIO23) ===== */
    gpio_init(PICO_SWITCH_RUN_GPIO);
    gpio_set_dir(PICO_SWITCH_RUN_GPIO, GPIO_IN);
    if (PICO_SWITCH_PULL_UP) {
        gpio_pull_up(PICO_SWITCH_RUN_GPIO);
    } else {
        gpio_pull_down(PICO_SWITCH_RUN_GPIO);
    }
    g_io_mgr->switch_run_state = false;
    
    /* ===== 初始化 UART0 (GPIO0/1) ===== */
    uart_init(PICO_UART0_INSTANCE, PICO_UART_BAUDRATE);
    gpio_set_function(PICO_UART_TX_GPIO, GPIO_FUNC_UART);
    gpio_set_function(PICO_UART_RX_GPIO, GPIO_FUNC_UART);
    uart_set_hw_flow(PICO_UART0_INSTANCE, false, false);  /* 禁用硬件流控 */
    
    /* ===== 初始化 RS485 DE/RE 控制 (GPIO24/25) ===== */
    gpio_init(PICO_RS485_DE_GPIO);
    gpio_set_dir(PICO_RS485_DE_GPIO, GPIO_OUT);
    gpio_put(PICO_RS485_DE_GPIO, 0);  /* 初始接收模式 */
    
    gpio_init(PICO_RS485_RE_GPIO);
    gpio_set_dir(PICO_RS485_RE_GPIO, GPIO_OUT);
    gpio_put(PICO_RS485_RE_GPIO, 0);  /* 初始接收使能 */
    
    /* ===== 初始化 ADC ===== */
    adc_init();
    
    /* ADC0 (GPIO26) - AI0 */
    adc_gpio_init(PICO_ADC_AI0_GPIO);
    g_io_mgr->adc_gpio_mask |= (1 << PICO_ADC_AI0_GPIO);
    
    /* ADC1 (GPIO27) - AI1 */
    adc_gpio_init(PICO_ADC_AI1_GPIO);
    g_io_mgr->adc_gpio_mask |= (1 << PICO_ADC_AI1_GPIO);
    
    /* ADC2 (GPIO28) - PVD (掉电检测) */
    adc_gpio_init(PICO_ADC_PVD_GPIO);
    g_io_mgr->adc_gpio_mask |= (1 << PICO_ADC_PVD_GPIO);
    
    /* 设置 ADC 采样速度（可根据需求调整） */
    adc_set_temp_sensor_enabled(false);  /* 禁用内部温度传感器 */
    
    g_io_mgr->initialized = true;
    
    printf("[IO] I/O 管理器初始化完成\n");
    printf("[IO] 数字输入: %d 路 (X0-X9)\n", PICO_TOTAL_INPUTS);
    printf("[IO] 数字输出: %d 路 (Y0-Y8)\n", PICO_OUTPUT_COUNT);
    printf("[IO] 模拟输入: %d 路 (AI0, AI1, PVD)\n", PICO_TOTAL_ADC);
    printf("[IO] LED: %d 个 (RUN, ERR)\n", PICO_TOTAL_LEDS);
    printf("[IO] UART: %d bps @ GPIO%d/GPIO%d\n", PICO_UART_BAUDRATE, PICO_UART_TX_GPIO, PICO_UART_RX_GPIO);
}

/**
 * io_set_gpio_output - 设置单个 GPIO 输出
 */
void io_set_gpio_output(uint8_t gpio_num, bool value)
{
    if (gpio_num >= 30) return;  /* 检查有效范围 */
    gpio_put(gpio_num, value);
}

/**
 * io_get_gpio_input - 读取单个 GPIO 输入
 */
bool io_get_gpio_input(uint8_t gpio_num)
{
    if (gpio_num >= 30) return false;
    return gpio_get(gpio_num);
}

/**
 * io_write_output_relay - 写 FX3U 输出继电器 (Y0-Y8)
 * 
 * 参数：relay_num (0-8) -> GPIO 输出，value (0/1)
 */
void io_write_output_relay(uint8_t relay_num, uint8_t value)
{
    if (!g_io_mgr || relay_num >= PICO_OUTPUT_COUNT) return;
    
    uint8_t output_gpios[] = {
        PICO_OUTPUT_Y0_GPIO,  PICO_OUTPUT_Y1_GPIO,  PICO_OUTPUT_Y2_GPIO,
        PICO_OUTPUT_Y3_GPIO,  PICO_OUTPUT_Y4_GPIO,  PICO_OUTPUT_Y5_GPIO,
        PICO_OUTPUT_Y6_GPIO,  PICO_OUTPUT_Y7_GPIO,  PICO_OUTPUT_Y8_GPIO
    };
    
    uint8_t gpio = output_gpios[relay_num];
    io_set_gpio_output(gpio, value ? true : false);
    
    /* 更新缓冲 */
    if (value) {
        g_io_mgr->output_state[relay_num / 8] |= (1 << (relay_num % 8));
    } else {
        g_io_mgr->output_state[relay_num / 8] &= ~(1 << (relay_num % 8));
    }
}

/**
 * io_read_input_relay - 读 FX3U 输入继电器 (X0-X9)
 * 
 * 返回：0 或 1
 */
uint8_t io_read_input_relay(uint8_t relay_num)
{
    if (relay_num >= PICO_TOTAL_INPUTS) return 0;
    
    uint8_t input_gpios[] = {
        PICO_INPUT_X0_GPIO,  PICO_INPUT_X1_GPIO,  PICO_INPUT_X2_GPIO,
        PICO_INPUT_X3_GPIO,  PICO_INPUT_X4_GPIO,  PICO_INPUT_X5_GPIO,
        PICO_INPUT_X6_GPIO,  PICO_INPUT_X7_GPIO,  PICO_INPUT_X8_GPIO,
        PICO_INPUT_X9_GPIO
    };
    
    uint8_t gpio = input_gpios[relay_num];
    return io_get_gpio_input(gpio) ? 1 : 0;
}

/**
 * io_set_led_run - 控制 RUN 指示灯
 */
void io_set_led_run(bool state)
{
    if (!g_io_mgr) return;
    gpio_put(PICO_LED_RUN_GPIO, PICO_LED_ACTIVE_HIGH ? state : !state);
    g_io_mgr->led_run_state = state;
}

/**
 * io_set_led_err - 控制 ERR 指示灯
 */
void io_set_led_err(bool state)
{
    if (!g_io_mgr) return;
    gpio_put(PICO_LED_ERR_GPIO, PICO_LED_ACTIVE_HIGH ? state : !state);
    g_io_mgr->led_err_state = state;
}

/**
 * io_get_switch_run - 读取 RUN 开关状态
 */
bool io_get_switch_run(void)
{
    return gpio_get(PICO_SWITCH_RUN_GPIO);
}

/**
 * io_write_output_word - 批量写输出 (Y0-Y8 as uint16_t)
 * 
 * value 的 bit[0..8] 对应 Y0..Y8
 */
void io_write_output_word(uint16_t value)
{
    for (int i = 0; i < PICO_OUTPUT_COUNT; i++) {
        uint8_t bit_val = (value >> i) & 1;
        io_write_output_relay(i, bit_val);
    }
}

/**
 * io_read_input_word - 批量读输入 (X0-X9 as uint16_t)
 * 
 * 返回值：bit[0..9] 对应 X0..X9
 */
uint16_t io_read_input_word(void)
{
    if (!g_io_mgr) return 0;
    uint16_t value = 0;
    for (int i = 0; i < PICO_TOTAL_INPUTS; i++) {
        if (g_input_stable[i]) {  /* 使用去抖后的值 */
            value |= (1 << i);
        }
    }
    return value;
}

/**
 * io_read_adc_ai0 - 读 AI0 (GPIO26, ADC0)
 */
uint16_t io_read_adc_ai0(void)
{
    adc_select_input(0);  /* ADC channel 0 */
    return adc_read();
}

/**
 * io_read_adc_ai1 - 读 AI1 (GPIO27, ADC1)
 */
uint16_t io_read_adc_ai1(void)
{
    adc_select_input(1);  /* ADC channel 1 */
    return adc_read();
}

/**
 * io_read_adc_pvd - 读 PVD (GPIO28, ADC2, 掉电检测)
 */
uint16_t io_read_adc_pvd(void)
{
    adc_select_input(2);  /* ADC channel 2 */
    return adc_read();
}

/**
 * io_read_adc_raw - 读原始 ADC 数值 (0-4095 for 12-bit)
 */
uint16_t io_read_adc_raw(uint8_t channel)
{
    if (channel > 2) return 0;
    adc_select_input(channel);
    return adc_read();
}

/**
 * io_adc_to_millivolts - 将 ADC 读数转换为毫伏 (0..3300 mV)
 */
uint16_t io_adc_to_millivolts(uint16_t adc_value)
{
    /* 12-bit ADC: 4096 step = 3300 mV */
    return (uint16_t)((adc_value * PICO_ADC_VREF) / 4096);
}

/**
 * io_rs485_set_de - 设置 RS485 驱动使能 (DE)
 * 
 * enable=true: 发送模式 (DE=1)
 * enable=false: 接收模式 (DE=0)
 */
void io_rs485_set_de(bool enable)
{
    gpio_put(PICO_RS485_DE_GPIO, enable ? 1 : 0);
}

/**
 * io_rs485_set_re - 设置 RS485 接收使能 (RE)
 * 
 * enable=true: 接收使能 (RE=0, 低电平有效)
 * enable=false: 接收禁用 (RE=1)
 */
void io_rs485_set_re(bool enable)
{
    gpio_put(PICO_RS485_RE_GPIO, enable ? 0 : 1);  /* RE 低有效 */
}

/**
 * io_enable_pwm - 启用 PWM 输出
 */
void io_enable_pwm(uint8_t gpio_num, uint16_t frequency_hz, uint8_t duty_percent)
{
    if (gpio_num >= 30) return;
    if (duty_percent > 100) duty_percent = 100;
    
    gpio_set_function(gpio_num, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio_num);
    
    pwm_config config = pwm_get_default_config();
    
    /* 设置分频与计数值以达到目标频率 */
    /* Pico 系统时钟 125MHz */
    uint32_t period = 125000000 / frequency_hz;  /* 计数周期 */
    
    if (period > 65535) {
        /* 需要分频 */
        pwm_config_set_clkdiv(&config, 16.0f);  /* 125MHz / 16 = 7.8125 MHz */
        period = 7812500 / frequency_hz;
    }
    
    pwm_config_set_wrap(&config, period - 1);
    pwm_init(slice_num, &config, true);
    
    uint16_t level = (uint32_t)(period * duty_percent) / 100;
    pwm_set_gpio_level(gpio_num, level);
    pwm_set_enabled(slice_num, true);
}

/**
 * io_disable_pwm - 禁用 PWM 输出
 */
void io_disable_pwm(uint8_t gpio_num)
{
    if (gpio_num >= 30) return;
    
    uint slice_num = pwm_gpio_to_slice_num(gpio_num);
    pwm_set_enabled(slice_num, false);
    gpio_set_function(gpio_num, GPIO_FUNC_SIO);
    gpio_put(gpio_num, 0);
}

/**
 * io_manager_update - 定期扫描输入与 ADC（在主循环中调用）
 * 
 * 功能：
 * - 去抖动数字输入
 * - 采样 ADC 值
 * - 更新输入缓冲
 */
void io_manager_update(void)
{
    if (!g_io_mgr || !g_io_mgr->initialized) return;
    
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    /* 扫描数字输入并去抖动 */
    uint8_t input_gpios[] = {
        PICO_INPUT_X0_GPIO,  PICO_INPUT_X1_GPIO,  PICO_INPUT_X2_GPIO,
        PICO_INPUT_X3_GPIO,  PICO_INPUT_X4_GPIO,  PICO_INPUT_X5_GPIO,
        PICO_INPUT_X6_GPIO,  PICO_INPUT_X7_GPIO,  PICO_INPUT_X8_GPIO,
        PICO_INPUT_X9_GPIO
    };
    
    for (int i = 0; i < PICO_TOTAL_INPUTS; i++) {
        bool pin_state = gpio_get(input_gpios[i]);
        
        if (pin_state != g_input_stable[i]) {
            /* 状态改变，启动去抖定时器 */
            if (g_input_debounce_time[i] == 0) {
                g_input_debounce_time[i] = now;
            } else if (now - g_input_debounce_time[i] >= PICO_INPUT_DEBOUNCE_MS) {
                /* 去抖完成，确认新状态 */
                g_input_stable[i] = pin_state ? 1 : 0;
                g_input_debounce_time[i] = 0;
                
                /* 可在此处添加状态变化回调 */
            }
        } else {
            g_input_debounce_time[i] = 0;
        }
        
        /* 更新输入缓冲 */
        if (g_input_stable[i]) {
            g_io_mgr->input_state[i / 8] |= (1 << (i % 8));
        } else {
            g_io_mgr->input_state[i / 8] &= ~(1 << (i % 8));
        }
    }
    
    /* 采样 ADC 值 */
    g_io_mgr->adc_values[0] = io_read_adc_ai0();
    g_io_mgr->adc_values[1] = io_read_adc_ai1();
    g_io_mgr->adc_values[2] = io_read_adc_pvd();
    
    /* 更新 RUN 开关状态 */
    g_io_mgr->switch_run_state = gpio_get(PICO_SWITCH_RUN_GPIO);
}

/**
 * io_check_voltage_safe - 检查电压是否在安全范围
 * 
 * 注：此函数为占位符；实际电压监测需要专用 ADC 输入与分压电路
 */
bool io_check_voltage_safe(void)
{
    /* TODO: 通过 PVD (GPIO28) 或其他监测 */
    return true;
}

/**
 * io_diagnostic_report - 打印诊断报告
 */
void io_diagnostic_report(void)
{
    if (!g_io_mgr) {
        printf("\n===== I/O 诊断报告 =====\n未初始化 I/O 管理器\n========================\n\n");
        return;
    }
    printf("\n===== I/O 诊断报告 =====\n");
    printf("初始化状态: %s\n", g_io_mgr->initialized ? "OK" : "未初始化");
    printf("RUN LED: %s\n", g_io_mgr->led_run_state ? "ON" : "OFF");
    printf("ERR LED: %s\n", g_io_mgr->led_err_state ? "ON" : "OFF");
    printf("RUN 开关: %s\n", g_io_mgr->switch_run_state ? "按下" : "释放");
    
    printf("\n数字输入 (X0-X9):\n");
    for (int i = 0; i < PICO_TOTAL_INPUTS; i++) {
        printf("  X%d: %d\n", i, g_input_stable[i]);
    }
    
    printf("\n模拟输入:\n");
    printf("  AI0 (PVD 毫伏): %u mV\n", io_adc_to_millivolts(g_io_mgr->adc_values[0]));
    printf("  AI1 (毫伏): %u mV\n", io_adc_to_millivolts(g_io_mgr->adc_values[1]));
    printf("  PVD (毫伏): %u mV\n", io_adc_to_millivolts(g_io_mgr->adc_values[2]));
    
    printf("\nRS485 状态:\n");
    printf("  DE (发送): %d\n", gpio_get(PICO_RS485_DE_GPIO));
    printf("  RE (接收): %d\n", gpio_get(PICO_RS485_RE_GPIO));
    
    printf("========================\n\n");
}

/**
 * io_manager_shutdown - 关闭 I/O 管理器
 */
void io_manager_shutdown(void)
{
    if (!g_io_mgr || !g_io_mgr->initialized) return;
    
    /* 关闭所有输出 */
    for (int i = 0; i < PICO_OUTPUT_COUNT; i++) {
        io_write_output_relay(i, 0);
    }
    
    /* 关闭 LED */
    io_set_led_run(false);
    io_set_led_err(false);
    
    /* 禁用 ADC */
    adc_init();  /* 重置 */
    
    printf("[IO] I/O 管理器已关闭\n");
}
