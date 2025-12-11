# API 参考文档

## 目录

1. [PLC 核心 API](#plc-核心-api)
2. [指令集 API](#指令集-api)
3. [I/O 管理 API](#io-管理-api)
4. [通信 API](#通信-api)
5. [MODBUS 协议 API](#modbus-协议-api)
6. [完整示例](#完整示例)

## PLC 核心 API

### fx3u_core_t 结构体

```c
typedef struct {
    plc_state_t state;                          // PLC运行状态
    uint32_t scan_time_ms;                      // 扫描周期(ms)
    uint32_t cycle_count;                       // 扫描周期计数
    
    uint8_t inputs[PLC_MAX_INPUTS];            // 输入继电器 X
    uint8_t outputs[PLC_MAX_OUTPUTS];          // 输出继电器 Y
    uint8_t internals[PLC_MAX_INTERNALS];      // 内部继电器 M
    
    fx3u_timer_t timers[PLC_MAX_TIMERS];       // 定时器 T
    fx3u_counter_t counters[PLC_MAX_COUNTERS]; // 计数器 C
    
    int16_t registers[PLC_MAX_REGISTERS];      // 数据寄存器 D
    
    uint32_t program_counter;                  // 程序计数器
    uint32_t program_size;                     // 程序大小
    
    uint16_t error_code;                       // 错误代码
} fx3u_core_t;
```

### 初始化和控制

```c
/**
 * 初始化 PLC 核心
 * @param plc: PLC 实例指针
 */
void fx3u_core_init(fx3u_core_t *plc);

/**
 * 启动 PLC
 * @param plc: PLC 实例指针
 */
void fx3u_core_start(fx3u_core_t *plc);

/**
 * 停止 PLC
 * @param plc: PLC 实例指针
 */
void fx3u_core_stop(fx3u_core_t *plc);

/**
 * 执行一个扫描周期
 * @param plc: PLC 实例指针
 */
void fx3u_core_run_cycle(fx3u_core_t *plc);
```

### 输入/输出继电器

```c
/**
 * 设置输入继电器
 * @param plc: PLC 实例指针
 * @param addr: 地址 (0-255)
 * @param value: 值 (0 或 1)
 */
void fx3u_set_input(fx3u_core_t *plc, uint16_t addr, uint8_t value);

/**
 * 获取输入继电器状态
 * @param plc: PLC 实例指针
 * @param addr: 地址
 * @return: 0 或 1
 */
uint8_t fx3u_get_input(fx3u_core_t *plc, uint16_t addr);

/**
 * 设置输出继电器
 * @param plc: PLC 实例指针
 * @param addr: 地址 (0-255)
 * @param value: 值 (0 或 1)
 */
void fx3u_set_output(fx3u_core_t *plc, uint16_t addr, uint8_t value);

/**
 * 获取输出继电器状态
 * @param plc: PLC 实例指针
 * @param addr: 地址
 * @return: 0 或 1
 */
uint8_t fx3u_get_output(fx3u_core_t *plc, uint16_t addr);

/**
 * 设置内部继电器
 * @param plc: PLC 实例指针
 * @param addr: 地址 (0-2047)
 * @param value: 值 (0 或 1)
 */
void fx3u_set_internal(fx3u_core_t *plc, uint16_t addr, uint8_t value);

/**
 * 获取内部继电器状态
 * @param plc: PLC 实例指针
 * @param addr: 地址
 * @return: 0 或 1
 */
uint8_t fx3u_get_internal(fx3u_core_t *plc, uint16_t addr);
```

### 数据寄存器

```c
/**
 * 设置数据寄存器值
 * @param plc: PLC 实例指针
 * @param addr: 地址 (0-4095)
 * @param value: 16位有符号整数值
 */
void fx3u_set_register(fx3u_core_t *plc, uint16_t addr, int16_t value);

/**
 * 获取数据寄存器值
 * @param plc: PLC 实例指针
 * @param addr: 地址
 * @return: 寄存器值
 */
int16_t fx3u_get_register(fx3u_core_t *plc, uint16_t addr);
```

### 定时器和计数器

```c
/**
 * 启动定时器
 * @param plc: PLC 实例指针
 * @param timer_num: 定时器编号 (0-127)
 * @param preset: 预设值 (ms)
 */
void fx3u_timer_start(fx3u_core_t *plc, uint16_t timer_num, uint32_t preset);

/**
 * 停止定时器
 * @param plc: PLC 实例指针
 * @param timer_num: 定时器编号
 */
void fx3u_timer_stop(fx3u_core_t *plc, uint16_t timer_num);

/**
 * 检查定时器是否完成
 * @param plc: PLC 实例指针
 * @param timer_num: 定时器编号
 * @return: true 表示完成，false 表示未完成
 */
bool fx3u_timer_done(fx3u_core_t *plc, uint16_t timer_num);

/**
 * 启动计数器
 * @param plc: PLC 实例指针
 * @param counter_num: 计数器编号 (0-127)
 * @param preset: 预设值
 */
void fx3u_counter_start(fx3u_core_t *plc, uint16_t counter_num, int32_t preset);

/**
 * 复位计数器
 * @param plc: PLC 实例指针
 * @param counter_num: 计数器编号
 */
void fx3u_counter_reset(fx3u_core_t *plc, uint16_t counter_num);

/**
 * 检查计数器是否完成
 * @param plc: PLC 实例指针
 * @param counter_num: 计数器编号
 * @return: true 表示完成
 */
bool fx3u_counter_done(fx3u_core_t *plc, uint16_t counter_num);
```

### 错误管理

```c
/**
 * 设置错误代码
 * @param plc: PLC 实例指针
 * @param error_code: 错误代码
 */
void fx3u_set_error(fx3u_core_t *plc, uint16_t error_code);

/**
 * 获取错误代码
 * @param plc: PLC 实例指针
 * @return: 错误代码
 */
uint16_t fx3u_get_error(fx3u_core_t *plc);

/**
 * 清除错误
 * @param plc: PLC 实例指针
 */
void fx3u_clear_error(fx3u_core_t *plc);
```

---

## 指令集 API

### 基础逻辑指令

```c
/**
 * LD 装入 - 从继电器获取新逻辑线
 * @param plc: PLC 实例
 * @param address: 继电器地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_ld(fx3u_core_t *plc, uint16_t address);

/**
 * AND 并联逻辑与
 * @param plc: PLC 实例
 * @param address: 继电器地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_and(fx3u_core_t *plc, uint16_t address);

/**
 * OR 串联逻辑或
 * @param plc: PLC 实例
 * @param address: 继电器地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_or(fx3u_core_t *plc, uint16_t address);

/**
 * NOT 逻辑反向
 * @param plc: PLC 实例
 * @return: 指令执行结果
 */
inst_result_t fx3u_not(fx3u_core_t *plc);

/**
 * OUT 输出
 * @param plc: PLC 实例
 * @param address: 输出地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_out(fx3u_core_t *plc, uint16_t address);
```

### 数据处理指令

```c
/**
 * MOV 数据传送
 * @param plc: PLC 实例
 * @param src_addr: 源地址
 * @param dst_addr: 目标地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_mov(fx3u_core_t *plc, uint16_t src_addr, uint16_t dst_addr);

/**
 * ADD 加法
 * @param plc: PLC 实例
 * @param addr1: 第一个操作数地址
 * @param addr2: 第二个操作数地址
 * @param result_addr: 结果存储地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_add(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);

/**
 * SUB 减法
 */
inst_result_t fx3u_sub(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);

/**
 * MUL 乘法
 */
inst_result_t fx3u_mul(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);

/**
 * DIV 除法
 */
inst_result_t fx3u_div(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2, uint16_t result_addr);

/**
 * CMP 比较
 * @param plc: PLC 实例
 * @param addr1: 比较值1
 * @param addr2: 比较值2
 * @return: 指令执行结果
 */
inst_result_t fx3u_cmp(fx3u_core_t *plc, uint16_t addr1, uint16_t addr2);
```

### 位操作指令

```c
/**
 * SET 置位
 * @param plc: PLC 实例
 * @param address: 地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_set(fx3u_core_t *plc, uint16_t address);

/**
 * RST 复位
 * @param plc: PLC 实例
 * @param address: 地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_rst(fx3u_core_t *plc, uint16_t address);

/**
 * PLS 脉冲 (上升沿触发)
 * @param plc: PLC 实例
 * @param address: 地址
 * @return: 指令执行结果
 */
inst_result_t fx3u_pls(fx3u_core_t *plc, uint16_t address);
```

---

## I/O 管理 API

```c
/**
 * 初始化 I/O 管理器
 * @param io_mgr: I/O 管理器指针
 */
void io_manager_init(io_manager_t *io_mgr);

/**
 * 设置 GPIO 输出
 * @param gpio_num: GPIO 编号 (0-27)
 * @param value: true 为高，false 为低
 */
void io_set_gpio_output(uint8_t gpio_num, bool value);

/**
 * 读取 GPIO 输入
 * @param gpio_num: GPIO 编号
 * @return: true 为高，false 为低
 */
bool io_get_gpio_input(uint8_t gpio_num);

/**
 * 写入输出继电器 (Y)
 * @param relay_addr: 继电器地址 (0-7)
 * @param value: 0 或 1
 */
void io_write_output_relay(uint8_t relay_addr, uint8_t value);

/**
 * 读取输入继电器 (X)
 * @param relay_addr: 继电器地址 (0-7)
 * @return: 0 或 1
 */
uint8_t io_read_input_relay(uint8_t relay_addr);

/**
 * 写入输出字节
 * @param byte_index: 字节索引
 * @param value: 字节值 (8位)
 */
void io_write_output_byte(uint8_t byte_index, uint8_t value);

/**
 * 读取输入字节
 * @param byte_index: 字节索引
 * @return: 字节值
 */
uint8_t io_read_input_byte(uint8_t byte_index);

/**
 * 读取 ADC 通道
 * @param channel: 通道编号 (0-3)
 * @return: 12位 ADC 值
 */
uint16_t io_read_adc_channel(uint8_t channel);

/**
 * 启用 PWM 输出
 * @param gpio_num: GPIO 编号
 * @param frequency: 频率 (Hz)
 * @param duty_cycle: 占空比 (0-100%)
 */
void io_enable_pwm(uint8_t gpio_num, uint16_t frequency, uint8_t duty_cycle);

/**
 * 禁用 PWM 输出
 * @param gpio_num: GPIO 编号
 */
void io_disable_pwm(uint8_t gpio_num);
```

---

## 通信 API

```c
/**
 * 初始化通信接口
 * @param config: 通信配置指针
 * @param mode: 通信模式
 */
void comm_init(comm_config_t *config, comm_mode_t mode);

/**
 * 设置波特率
 * @param config: 通信配置
 * @param baud: 波特率
 */
void comm_set_baudrate(comm_config_t *config, baud_rate_t baud);

/**
 * 设置奇偶校验
 * @param config: 通信配置
 * @param parity: 校验方式
 */
void comm_set_parity(comm_config_t *config, parity_t parity);

/**
 * 接收数据
 * @param buffer: 接收缓冲区
 * @param max_len: 最大接收长度
 * @return: 实际接收长度
 */
int comm_receive(uint8_t *buffer, uint16_t max_len);

/**
 * 发送数据
 * @param buffer: 发送缓冲区
 * @param len: 发送长度
 * @return: 实际发送长度
 */
int comm_send(uint8_t *buffer, uint16_t len);

/**
 * 检查是否有可用的数据帧
 * @return: true 有可用数据
 */
bool comm_frame_available(void);
```

---

## MODBUS 协议 API

```c
/**
 * 初始化 MODBUS
 * @param config: MODBUS 配置
 * @param slave_id: 从机地址
 */
void modbus_init(modbus_config_t *config, uint8_t slave_id);

/**
 * 设置主从模式
 * @param config: MODBUS 配置
 * @param is_master: true 为主机，false 为从机
 */
void modbus_set_master(modbus_config_t *config, bool is_master);

/**
 * 计算 CRC16 校验码
 * @param data: 数据指针
 * @param length: 数据长度
 * @return: CRC16 值
 */
uint16_t modbus_crc16(uint8_t *data, uint16_t length);

/**
 * 验证 MODBUS 帧
 * @param buffer: 帧数据
 * @param length: 帧长度
 * @return: true 为有效
 */
bool modbus_validate_frame(uint8_t *buffer, uint16_t length);

/**
 * 从机处理请求
 * @param plc: PLC 实例
 * @param rx_buffer: 接收缓冲区
 * @param rx_len: 接收长度
 * @param tx_buffer: 发送缓冲区
 * @return: 发送长度
 */
int modbus_slave_process(fx3u_core_t *plc, uint8_t *rx_buffer, uint16_t rx_len,
                         uint8_t *tx_buffer);
```

---

## 完整示例

### 示例1: 基本的继电器控制

```c
#include "fx3u_core.h"
#include "fx3u_io.h"

int main(void) {
    // 初始化
    fx3u_core_t plc;
    io_manager_t io_mgr;
    
    fx3u_core_init(&plc);
    io_manager_init(&io_mgr);
    
    fx3u_core_start(&plc);
    
    // 设置输入
    fx3u_set_input(&plc, 0, 1);  // X0 = ON
    
    // 设置输出
    fx3u_set_output(&plc, 0, 1);  // Y0 = ON
    
    // 同步到GPIO
    io_write_output_relay(0, fx3u_get_output(&plc, 0));
    
    return 0;
}
```

### 示例2: 定时器应用

```c
// 启动100ms定时器
fx3u_timer_start(&plc, 0, 100);

// 在每个扫描周期检查
for (int i = 0; i < 10; i++) {
    fx3u_core_run_cycle(&plc);
    
    if (fx3u_timer_done(&plc, 0)) {
        printf("Timer T0 completed!\r\n");
        fx3u_timer_stop(&plc, 0);
    }
    
    sleep_ms(50);
}
```

### 示例3: 数据寄存器计算

```c
// 设置操作数
fx3u_set_register(&plc, 0, 100);  // D0 = 100
fx3u_set_register(&plc, 1, 50);   // D1 = 50

// 执行加法: D0 + D1 -> D2
fx3u_add(&plc, 0, 1, 2);

// 读结果
int16_t result = fx3u_get_register(&plc, 2);
printf("Result: %d\r\n", result);  // 输出: 150
```

### 示例4: MODBUS 通信

```c
#include "modbus_protocol.h"
#include "rs485_driver.h"

int main(void) {
    fx3u_core_t plc;
    modbus_config_t modbus_cfg;
    rs485_config_t rs485_cfg;
    
    // 初始化
    fx3u_core_init(&plc);
    modbus_init(&modbus_cfg, 1);
    
    rs485_cfg.baudrate = 9600;
    rs485_init(&rs485_cfg);
    
    uint8_t rx_buffer[256];
    uint8_t tx_buffer[256];
    
    // 主循环
    while (1) {
        // 接收数据
        int rx_len = rs485_receive(rx_buffer, 256);
        
        if (rx_len > 0) {
            // 处理请求
            int tx_len = modbus_slave_process(&plc, rx_buffer, rx_len, tx_buffer);
            
            // 发送响应
            if (tx_len > 0) {
                rs485_send(tx_buffer, tx_len);
            }
        }
        
        // 运行 PLC
        fx3u_core_run_cycle(&plc);
        
        sleep_ms(200);
    }
    
    return 0;
}
```

---

更多信息请参考源代码中的详细注释。
