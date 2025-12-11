/**
 * FX3U I/O 端口映射和管理（基于 Pico 物理引脚顺序）
 * 
 * 硬件约束：
 * - 所有 GPIO 工作电压：3.3V（绝对最大 3.6V）
 * - 单个 GPIO 驱动能力：推荐 ≤12mA，绝对最大 ~16mA
 * - 数字输入：建议外部或内部上拉（若接地闭合）
 * - ADC 输入范围：0..3.3V，建议源阻 <10k
 * - USB VBUS (5V)：仅用作电源，不可作 GPIO
 */

#ifndef __FX3U_IO_H__
#define __FX3U_IO_H__

#include <stdint.h>
#include <stdbool.h>

/* ===== 物理引脚映射（Raspberry Pi Pico Pin 1-40） ===== */

/* === 左侧引脚 (Pin 1 - Pin 20) === */
#define PICO_PIN1_UART_TX       0       /* GPIO0: UART0 TX，3.3V 推挽 */
#define PICO_PIN2_UART_RX       1       /* GPIO1: UART0 RX，3.3V 输入，需电平转换若外部 5V */
#define PICO_PIN3_GND           -1      /* 系统地 */
#define PICO_PIN4_RUN_LED       2       /* GPIO2: RUN_LED（运行指示灯），高电平点亮，串限流电阻 220-330Ω */
#define PICO_PIN5_ERR_LED       3       /* GPIO3: ERR_LED（错误指示灯），高电平点亮，串限流电阻 220-330Ω */
#define PICO_PIN6_Y7            4       /* GPIO4: Y7 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN7_Y8            5       /* GPIO5: Y8 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN8_GND           -1      /* 系统地 */
#define PICO_PIN9_X0            6       /* GPIO6: X0 数字输入，建议上拉 */
#define PICO_PIN10_X1           7       /* GPIO7: X1 数字输入，建议上拉 */
#define PICO_PIN11_X2           8       /* GPIO8: X2 数字输入，建议上拉 */
#define PICO_PIN12_X3           9       /* GPIO9: X3 数字输入，建议上拉 */
#define PICO_PIN13_GND          -1      /* 系统地 */
#define PICO_PIN14_X4           10      /* GPIO10: X4 数字输入，建议上拉 */
#define PICO_PIN15_X5           11      /* GPIO11: X5 数字输入，建议上拉 */
#define PICO_PIN16_X6           12      /* GPIO12: X6 数字输入，建议上拉 */
#define PICO_PIN17_X7           13      /* GPIO13: X7 数字输入，建议上拉 */
#define PICO_PIN18_GND          -1      /* 系统地 */
#define PICO_PIN19_X8           14      /* GPIO14: X8 数字输入，建议上拉 */
#define PICO_PIN20_X9           15      /* GPIO15: X9 数字输入，建议上拉 */

/* === 右侧引脚 (Pin 40 - Pin 21) === */
#define PICO_PIN40_VBUS         -1      /* USB VBUS (5V) - 仅电源，不可作 GPIO */
#define PICO_PIN39_VSYS         -1      /* 系统电源输入 - 不可作 GPIO */
#define PICO_PIN38_GND          -1      /* 系统地 */
#define PICO_PIN37_3V3_EN       -1      /* 3.3V 使能（板级控制）- 不可作 GPIO */
#define PICO_PIN36_3V3_OUT      -1      /* 3.3V 输出 - 不可作 GPIO */
#define PICO_PIN35_ADC_VREF     -1      /* ADC 参考（若外部参考，务必 ≤3.3V） */
#define PICO_PIN34_PVD          28      /* GPIO28: 掉电检测（可用 ADC/数字输入，外接监测电路） */
#define PICO_PIN33_AGND         -1      /* 模拟地（需单点接地） */
#define PICO_PIN32_AI1          27      /* GPIO27: AI1 模拟输入（ADC1），范围 0..3.3V */
#define PICO_PIN31_AI0          26      /* GPIO26: AI0 模拟输入（ADC0），范围 0..3.3V */
#define PICO_PIN30_RUN          23      /* GPIO23: RUN（运行/停止开关输入），需外部去抖与上拉 */
#define PICO_PIN29_Y0           22      /* GPIO22: Y0 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN28_GND          -1      /* 系统地 */
#define PICO_PIN27_Y1           21      /* GPIO21: Y1 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN26_Y2           20      /* GPIO20: Y2 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN25_Y3           19      /* GPIO19: Y3 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN24_Y4           18      /* GPIO18: Y4 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN23_GND          -1      /* 系统地 */
#define PICO_PIN22_Y5           17      /* GPIO17: Y5 输出（继电器/灯），需外部驱动大负载 */
#define PICO_PIN21_Y6           16      /* GPIO16: Y6 输出（继电器/灯），需外部驱动大负载 */

/* ===== 按功能分类的宏定义 ===== */

/* UART0 (RS485 主串口) */
#define PICO_UART_TX_GPIO       PICO_PIN1_UART_TX      /* GPIO0 */
#define PICO_UART_RX_GPIO       PICO_PIN2_UART_RX      /* GPIO1 */
#define PICO_UART0_INSTANCE     uart0
#define PICO_UART_BAUDRATE      9600    /* 标准 RS485 波特率 */

/* RS485 DE/RE 控制（未列在用户引脚中，可用未分配 GPIO）*/
#define PICO_RS485_DE_GPIO      24      /* GPIO24: RS485 Driver Enable (建议) */
#define PICO_RS485_RE_GPIO      25      /* GPIO25: RS485 Receiver Enable (建议) */

/* 指示灯（LED） */
#define PICO_LED_RUN_GPIO       PICO_PIN4_RUN_LED      /* GPIO2 */
#define PICO_LED_ERR_GPIO       PICO_PIN5_ERR_LED      /* GPIO3 */
#define PICO_LED_ACTIVE_HIGH    true   /* LED 高电平点亮 */
#define PICO_LED_SERIES_R       220    /* 限流电阻(Ω)，目标 LED 电流 2-6mA @ 3.3V */

/* 数字输入组合 (X0-X9) */
#define PICO_INPUT_X0_GPIO      PICO_PIN9_X0           /* GPIO6 */
#define PICO_INPUT_X1_GPIO      PICO_PIN10_X1          /* GPIO7 */
#define PICO_INPUT_X2_GPIO      PICO_PIN11_X2          /* GPIO8 */
#define PICO_INPUT_X3_GPIO      PICO_PIN12_X3          /* GPIO9 */
#define PICO_INPUT_X4_GPIO      PICO_PIN14_X4          /* GPIO10 */
#define PICO_INPUT_X5_GPIO      PICO_PIN15_X5          /* GPIO11 */
#define PICO_INPUT_X6_GPIO      PICO_PIN16_X6          /* GPIO12 */
#define PICO_INPUT_X7_GPIO      PICO_PIN17_X7          /* GPIO13 */
#define PICO_INPUT_X8_GPIO      PICO_PIN19_X8          /* GPIO14 */
#define PICO_INPUT_X9_GPIO      PICO_PIN20_X9          /* GPIO15 */
#define PICO_INPUT_PULL_UP      true   /* 建议使用内部上拉（若接地闭合） */
#define PICO_INPUT_DEBOUNCE_MS  20     /* 去抖动延迟(ms) */

/* 数字输出组合 (Y0-Y8) */
#define PICO_OUTPUT_Y0_GPIO     PICO_PIN29_Y0          /* GPIO22 */
#define PICO_OUTPUT_Y1_GPIO     PICO_PIN27_Y1          /* GPIO21 */
#define PICO_OUTPUT_Y2_GPIO     PICO_PIN26_Y2          /* GPIO20 */
#define PICO_OUTPUT_Y3_GPIO     PICO_PIN25_Y3          /* GPIO19 */
#define PICO_OUTPUT_Y4_GPIO     PICO_PIN24_Y4          /* GPIO18 */
#define PICO_OUTPUT_Y5_GPIO     PICO_PIN22_Y5          /* GPIO17 */
#define PICO_OUTPUT_Y6_GPIO     PICO_PIN21_Y6          /* GPIO16 */
#define PICO_OUTPUT_Y7_GPIO     PICO_PIN6_Y7           /* GPIO4 */
#define PICO_OUTPUT_Y8_GPIO     PICO_PIN7_Y8           /* GPIO5 */
#define PICO_OUTPUT_COUNT       9      /* Y0-Y8 共 9 路输出 */
#define PICO_OUTPUT_DRIVE_MA    12     /* 单个 GPIO 推荐驱动 ≤12mA */

/* 开关输入 (RUN) */
#define PICO_SWITCH_RUN_GPIO    PICO_PIN30_RUN         /* GPIO23 */
#define PICO_SWITCH_PULL_UP     true   /* 建议内部上拉 */
#define PICO_SWITCH_DEBOUNCE_MS 50     /* 机械开关去抖动(ms) */

/* ADC 模拟输入 */
#define PICO_ADC_AI0_GPIO       PICO_PIN31_AI0         /* GPIO26 -> ADC0 */
#define PICO_ADC_AI1_GPIO       PICO_PIN32_AI1         /* GPIO27 -> ADC1 */
#define PICO_ADC_PVD_GPIO       PICO_PIN34_PVD         /* GPIO28 -> ADC2 (掉电检测) */
#define PICO_ADC_RESOLUTION     12     /* 12-bit ADC */
#define PICO_ADC_VREF           3300   /* 参考电压 3.3V (mV) */
#define PICO_ADC_SOURCE_Z_MAX   10000  /* 建议源阻 <10k 欧 */

/* ===== 总体计数与验证 ===== */
#define PICO_GPIO_COUNT_USED    24     /* 实际使用的 GPIO 数量 */
#define PICO_TOTAL_INPUTS       10     /* X0-X9 */
#define PICO_TOTAL_OUTPUTS      9      /* Y0-Y8 */
#define PICO_TOTAL_LEDS         2      /* RUN + ERR */
#define PICO_TOTAL_ADC          3      /* AI0 + AI1 + PVD */

/* 电压与驱动检查 */
#define PICO_IO_VOLTAGE_MIN_MV  3000   /* 3.0V 最小工作 */
#define PICO_IO_VOLTAGE_MAX_MV  3600   /* 3.6V 绝对最大 */
#define PICO_GPIO_DRIVE_MAX_MA  16     /* 单个 GPIO 绝对最大 16mA */
#define PICO_TOTAL_DRIVE_MAX_MA 200    /* 芯片级总电流限制(参考) */

/* ===== I/O 管理结构体 ===== */
typedef struct {
    uint32_t input_gpio_mask;   /* 输入 GPIO 掩码 (X0-X9) */
    uint32_t output_gpio_mask;  /* 输出 GPIO 掩码 (Y0-Y8) */
    uint32_t adc_gpio_mask;     /* ADC GPIO 掩码 (AI0, AI1, PVD) */
    uint8_t input_state[2];     /* 输入缓冲 (X0-X9 状态) */
    uint8_t output_state[2];    /* 输出缓冲 (Y0-Y8 状态) */
    bool led_run_state;         /* RUN LED 状态 */
    bool led_err_state;         /* ERR LED 状态 */
    bool switch_run_state;      /* RUN 开关状态 */
    uint16_t adc_values[3];     /* ADC 读数 [AI0, AI1, PVD] */
    bool initialized;
} io_manager_t;

/* ===== 函数声明 ===== */

/* 初始化与维护 */
void io_manager_init(io_manager_t *io_mgr);
void io_manager_update(void);      /* 定期扫描输入与 ADC */
void io_manager_shutdown(void);

/* 底层 GPIO 控制 */
void io_set_gpio_output(uint8_t gpio_num, bool value);
bool io_get_gpio_input(uint8_t gpio_num);

/* 高级 FX3U 继电器映射 */
void io_write_output_relay(uint8_t relay_num, uint8_t value);  /* Y0-Y8 */
uint8_t io_read_input_relay(uint8_t relay_num);              /* X0-X9 */

/* LED 控制 */
void io_set_led_run(bool state);
void io_set_led_err(bool state);

/* 开关输入 (RUN) */
bool io_get_switch_run(void);

/* 数字输入/输出 (批量) */
void io_write_output_word(uint16_t value);  /* 写 Y0-Y8 (9 bit) */
uint16_t io_read_input_word(void);          /* 读 X0-X9 (10 bit) */

/* ADC (模拟输入) */
uint16_t io_read_adc_ai0(void);     /* 读 AI0 (GPIO26) */
uint16_t io_read_adc_ai1(void);     /* 读 AI1 (GPIO27) */
uint16_t io_read_adc_pvd(void);     /* 读 PVD (GPIO28) - 掉电检测 */
uint16_t io_read_adc_raw(uint8_t channel);  /* 原始 ADC 读数 */
uint16_t io_adc_to_millivolts(uint16_t adc_value);  /* ADC 转换为 mV */

/* RS485 控制 */
void io_rs485_set_de(bool enable);  /* 驱动使能 (发送/接收切换) */
void io_rs485_set_re(bool enable);  /* 接收使能 */

/* 脉冲输出 (PWM) */
void io_enable_pwm(uint8_t gpio_num, uint16_t frequency_hz, uint8_t duty_percent);
void io_disable_pwm(uint8_t gpio_num);

/* 电气诊断 */
bool io_check_voltage_safe(void);   /* 检查电压是否在安全范围 */
void io_diagnostic_report(void);    /* 打印诊断信息 */

#endif /* __FX3U_IO_H__ */
