# Pico FX3U 模拟器 — I/O 端口配置报告

## 版本信息
- **日期**：2025-12-11
- **目标平台**：Raspberry Pi Pico (RP2040)
- **配置版本**：v1.0

---

## 1. 概述

本报告基于用户提供的物理引脚分配（Pin 1-40），为 Pico 上的 FX3U 模拟固件定义完整的 GPIO 映射、电气特性与初始化策略。该配置确保：
- 所有端口逻辑功能正确映射到 RP2040 GPIO
- 电气参数在安全范围内（3.3V, ≤12mA/GPIO）
- 输入去抖、输出驱动、ADC 采样得到妥善处理
- RS485 通信能够正常工作（半双工切换）

---

## 2. 物理引脚与 GPIO 映射详表

### 2.1 左侧引脚 (Pin 1-20)

| 物理引脚 | 标签 | GPIO | 方向 | 功能说明 | 电气特性 | 初始化 |
|---------|------|------|------|---------|---------|---------|
| Pin 1 | TX1 (UART) | GPIO0 | 输出 | UART0 TX (RS485 驱动输入) | 3.3V TTL, 推挽 | `gpio_set_function(0, GPIO_FUNC_UART)` |
| Pin 2 | RX1 (UART) | GPIO1 | 输入 | UART0 RX (RS485 反馈信号) | 3.3V TTL, 需缓冲若外部 5V | `gpio_set_function(1, GPIO_FUNC_UART)` |
| Pin 3 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 4 | RUN_LED | GPIO2 | 输出 | 运行指示灯（高电平点亮） | 3.3V, 推挽, 需串联 220-330Ω 限流电阻 | `gpio_set_dir(2, GPIO_OUT); gpio_pull_down(2)` |
| Pin 5 | ERR_LED | GPIO3 | 输出 | 错误指示灯（高电平点亮） | 3.3V, 推挽, 需串联 220-330Ω 限流电阻 | `gpio_set_dir(3, GPIO_OUT); gpio_pull_down(3)` |
| Pin 6 | Y7 | GPIO4 | 输出 | 继电器输出 Y7（需外部驱动电路） | 3.3V, 推挽, ≤12mA | `gpio_set_dir(4, GPIO_OUT); gpio_put(4, 0)` |
| Pin 7 | Y8 | GPIO5 | 输出 | 继电器输出 Y8（需外部驱动电路） | 3.3V, 推挽, ≤12mA | `gpio_set_dir(5, GPIO_OUT); gpio_put(5, 0)` |
| Pin 8 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 9 | X0 | GPIO6 | 输入 | 数字输入 X0（干接点） | 3.3V, 建议上拉 | `gpio_set_dir(6, GPIO_IN); gpio_pull_up(6)` |
| Pin 10 | X1 | GPIO7 | 输入 | 数字输入 X1 | 3.3V, 建议上拉 | `gpio_set_dir(7, GPIO_IN); gpio_pull_up(7)` |
| Pin 11 | X2 | GPIO8 | 输入 | 数字输入 X2 | 3.3V, 建议上拉 | `gpio_set_dir(8, GPIO_IN); gpio_pull_up(8)` |
| Pin 12 | X3 | GPIO9 | 输入 | 数字输入 X3 | 3.3V, 建议上拉 | `gpio_set_dir(9, GPIO_IN); gpio_pull_up(9)` |
| Pin 13 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 14 | X4 | GPIO10 | 输入 | 数字输入 X4 | 3.3V, 建议上拉 | `gpio_set_dir(10, GPIO_IN); gpio_pull_up(10)` |
| Pin 15 | X5 | GPIO11 | 输入 | 数字输入 X5 | 3.3V, 建议上拉 | `gpio_set_dir(11, GPIO_IN); gpio_pull_up(11)` |
| Pin 16 | X6 | GPIO12 | 输入 | 数字输入 X6 | 3.3V, 建议上拉 | `gpio_set_dir(12, GPIO_IN); gpio_pull_up(12)` |
| Pin 17 | X7 | GPIO13 | 输入 | 数字输入 X7 | 3.3V, 建议上拉 | `gpio_set_dir(13, GPIO_IN); gpio_pull_up(13)` |
| Pin 18 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 19 | X8 | GPIO14 | 输入 | 数字输入 X8 | 3.3V, 建议上拉 | `gpio_set_dir(14, GPIO_IN); gpio_pull_up(14)` |
| Pin 20 | X9 | GPIO15 | 输入 | 数字输入 X9 | 3.3V, 建议上拉 | `gpio_set_dir(15, GPIO_IN); gpio_pull_up(15)` |

### 2.2 右侧引脚 (Pin 40-21)

| 物理引脚 | 标签 | GPIO | 方向 | 功能说明 | 电气特性 | 初始化 |
|---------|------|------|------|---------|---------|---------|
| Pin 40 | VBUS | — | — | USB 电源 (5V, 仅供电) | 5V, 非 GPIO | 勿接 GPIO |
| Pin 39 | VSYS | — | — | 系统电源输入 (≈5V) | 供电用, 非 GPIO | 勿接 GPIO |
| Pin 38 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 37 | 3V3_EN | — | — | 3.3V 使能控制（板级） | 非 GPIO（或保留）| 不作 GPIO 使用 |
| Pin 36 | 3V3(OUT) | — | — | 3.3V 输出（电源） | 供电用, 非 GPIO | 勿接 GPIO |
| Pin 35 | ADC_VREF | — | — | ADC 参考（若外部参考） | ≤3.3V, 需稳压 | 若无外部参考接到 3.3V |
| Pin 34 | PVD | GPIO28 | 输入 | 掉电检测（ADC2 通道） | 3.3V, 需分压+监测 | `adc_gpio_init(28)` → `adc_select_input(2)` |
| Pin 33 | AGND | — | — | 模拟地（单点接地） | 0V 参考, 与数字地连接 | — |
| Pin 32 | AI1 | GPIO27 | 输入 | 模拟输入 AI1（ADC1 通道） | 0..3.3V, 12-bit | `adc_gpio_init(27)` → `adc_select_input(1)` |
| Pin 31 | AI0 | GPIO26 | 输入 | 模拟输入 AI0（ADC0 通道） | 0..3.3V, 12-bit | `adc_gpio_init(26)` → `adc_select_input(0)` |
| Pin 30 | RUN | GPIO23 | 输入 | 运行/停止开关（需去抖） | 3.3V, 建议上拉, 机械开关 | `gpio_set_dir(23, GPIO_IN); gpio_pull_up(23)` |
| Pin 29 | Y0 | GPIO22 | 输出 | 继电器输出 Y0 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(22, GPIO_OUT); gpio_put(22, 0)` |
| Pin 28 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 27 | Y1 | GPIO21 | 输出 | 继电器输出 Y1 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(21, GPIO_OUT); gpio_put(21, 0)` |
| Pin 26 | Y2 | GPIO20 | 输出 | 继电器输出 Y2 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(20, GPIO_OUT); gpio_put(20, 0)` |
| Pin 25 | Y3 | GPIO19 | 输出 | 继电器输出 Y3 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(19, GPIO_OUT); gpio_put(19, 0)` |
| Pin 24 | Y4 | GPIO18 | 输出 | 继电器输出 Y4 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(18, GPIO_OUT); gpio_put(18, 0)` |
| Pin 23 | GND | — | — | 系统地 | 0V 参考 | — |
| Pin 22 | Y5 | GPIO17 | 输出 | 继电器输出 Y5 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(17, GPIO_OUT); gpio_put(17, 0)` |
| Pin 21 | Y6 | GPIO16 | 输出 | 继电器输出 Y6 | 3.3V, 推挽, ≤12mA | `gpio_set_dir(16, GPIO_OUT); gpio_put(16, 0)` |

---

## 3. 功能分组与统计

### 3.1 按功能分类

#### 数字输入 (X0-X9) — 10 路

| 标识 | 物理引脚 | GPIO | 说明 |
|-----|---------|------|------|
| X0 | 9 | 6 | 干接点输入，建议上拉至 3.3V，去抖时间 20ms |
| X1 | 10 | 7 | 同上 |
| X2 | 11 | 8 | 同上 |
| X3 | 12 | 9 | 同上 |
| X4 | 14 | 10 | 同上 |
| X5 | 15 | 11 | 同上 |
| X6 | 16 | 12 | 同上 |
| X7 | 17 | 13 | 同上 |
| X8 | 19 | 14 | 同上 |
| X9 | 20 | 15 | 同上 |

#### 数字输出 (Y0-Y8) — 9 路

| 标识 | 物理引脚 | GPIO | 说明 |
|-----|---------|------|------|
| Y0 | 29 | 22 | 继电器驱动，需外部 MOSFET/晶体管（大负载） |
| Y1 | 27 | 21 | 同上 |
| Y2 | 26 | 20 | 同上 |
| Y3 | 25 | 19 | 同上 |
| Y4 | 24 | 18 | 同上 |
| Y5 | 22 | 17 | 同上 |
| Y6 | 21 | 16 | 同上 |
| Y7 | 6 | 4 | 同上 |
| Y8 | 7 | 5 | 同上 |

#### 模拟输入 (ADC) — 3 通道

| 标识 | 物理引脚 | GPIO | ADC 通道 | 范围 | 用途 |
|-----|---------|------|---------|------|------|
| AI0 | 31 | 26 | 0 | 0..3.3V | 模拟量输入（如温度、压力） |
| AI1 | 32 | 27 | 1 | 0..3.3V | 模拟量输入 |
| PVD | 34 | 28 | 2 | 0..3.3V | 掉电检测（需分压电路） |

#### 指示灯 (LED) — 2 个

| 标识 | 物理引脚 | GPIO | 用途 | 极性 |
|-----|---------|------|------|------|
| RUN_LED | 4 | 2 | 运行指示 | 高电平点亮 |
| ERR_LED | 5 | 3 | 故障指示 | 高电平点亮 |

#### 开关输入 — 1 个

| 标识 | 物理引脚 | GPIO | 用途 | 触发 |
|-----|---------|------|------|------|
| RUN 开关 | 30 | 23 | 启动/停止 | 接地闭合，需上拉+去抖 |

#### 通信接口

| 类型 | 物理引脚 | GPIO | 说明 |
|-----|---------|------|------|
| UART TX | 1 | 0 | RS485 驱动输入 (DI) |
| UART RX | 2 | 1 | RS485 反馈输出 (RO) |
| RS485 DE (额外) | — | 24 | 驱动使能（建议） |
| RS485 RE (额外) | — | 25 | 接收使能（建议） |

### 3.2 汇总统计

```
总 GPIO 使用量: 24 个（保留 GPIO25, 24 用于 RS485 DE/RE）
├─ 数字输入: 10 个 (GPIO6-15)
├─ 数字输出: 9 个 (GPIO4-5, GPIO16-22)
├─ 模拟输入: 3 个 (GPIO26-28)
├─ 指示灯: 2 个 (GPIO2-3)
├─ 开关输入: 1 个 (GPIO23)
└─ UART0: 2 个 (GPIO0-1)

电源/地引脚（非 GPIO）: 8 个
├─ VBUS (Pin 40)
├─ VSYS (Pin 39)
├─ 3V3_EN (Pin 37)
├─ 3V3(OUT) (Pin 36)
├─ ADC_VREF (Pin 35)
├─ GND (Pin 3, 8, 13, 18, 23, 28, 38)
└─ AGND (Pin 33)
```

---

## 4. 电气特性与约束

### 4.1 工作电压

| 参数 | 值 | 注释 |
|------|----|----|
| 逻辑电平（标准） | 3.3V | RP2040 IO 标准 |
| 最小工作电压 | 3.0V | 可靠性下限 |
| 绝对最大电压 | 3.6V | 超过会损坏 GPIO |
| USB VBUS | 5V | 仅用作电源，不可作 GPIO |

### 4.2 GPIO 驱动能力

| 参数 | 值 | 注释 |
|------|----|----|
| 单个 GPIO 推荐电流 | ≤12 mA | 安全工作范围 |
| 单个 GPIO 绝对最大 | ~16 mA | 不建议超过 |
| 芯片总 GPIO 电流限制 | ~200 mA | 参考值，需根据实际供电能力调整 |

**建议**：
- LED 驱动：串联 220-330Ω，目标电流 2-6 mA @ 3.3V
- 继电器驱动：必须使用外部 N-MOSFET（AO3400/2N7002 等）或光耦，不能直接驱动
- 大功率负载（24V 电磁阀等）：必须用中间驱动电路

### 4.3 输入特性

| 参数 | 配置 | 说明 |
|------|------|------|
| 数字输入上拉 | 内部/外部 10kΩ | 若接地闭合，启用上拉；若接 VCC 闭合，使用下拉 |
| 输入去抖延迟 | 20 ms | 软件去抖，可在 `io_manager_update()` 中调整 |
| 机械开关去抖 | 50 ms | RUN 开关推荐延迟 |

### 4.4 ADC 特性

| 参数 | 值 | 说明 |
|------|----|----|
| 分辨率 | 12-bit | 0..4095 步 |
| 参考电压 | 3.3V | 板载或外部稳压 |
| 量程 | 0..3.3V | 超过会限制或损坏 |
| 推荐源阻 | <10 kΩ | 高阻源需缓冲放大 |
| 转换时间 | ~2 µs | 每次采样约 2 微秒 |

**注意**：
- 若需测量高于 3.3V 的电压（如 5V 或 12V），必须使用分压电阻或模拟隔离器
- PVD（GPIO28）用于掉电检测，建议外接分压网络 + 比较器

### 4.5 UART / RS485 接口

| 参数 | 值 | 说明 |
|------|----|----|
| 波特率 | 9600 bps | 标准 FX3U/MODBUS 速率 |
| 数据位 | 8 | MODBUS RTU 标准 |
| 停止位 | 1 | MODBUS RTU 标准 |
| 校验 | 无 | MODBUS 使用 CRC16 |
| 电平 | 3.3V TTL | 需 RS232/RS485 收发器 |

**硬件配置**：
```
Pico TX (GPIO0) ──→ RS485 收发器 (如 MAX485)
Pico RX (GPIO1) ←── RS485 收发器
Pico DE (GPIO24) ──→ RS485 收发器 DE（驱动使能）
Pico RE (GPIO25) ──→ RS485 收发器 RE（接收使能）
```

---

## 5. 上拉/下拉配置建议

### 5.1 数字输入 (X0-X9)

**当前配置**：内部上拉（`gpio_pull_up()`）

**使用场景**：
- ✅ 接干接点到 GND（最常见）
- ✅ 接光电开关、接近开关（常开触点）

**若需改为下拉**：
- ❌ 若接入信号常闭到 3.3V（较少见）
- 需改为 `gpio_pull_down()`

### 5.2 RUN 开关 (GPIO23)

**当前配置**：内部上拉

**使用场景**：
- 机械开关按钮接地闭合
- 需外接 10 µF 去耦电容到地改善抗干扰性能

### 5.3 UART RX (GPIO1)

**当前配置**：无上拉（标准 UART 不需要）

**若连接 5V 设备**：
- 必须添加分压或电平转换（3.3V 设备接 5V 数据线会损坏 GPIO）
- 推荐：使用 1N4148 二极管 + 10kΩ 下拉，或专用电平转换芯片

---

## 6. 高电平有效冲突检查与防护

### 6.1 当前配置中的潜在问题

| 潜在冲突 | 风险 | 防护措施 | 状态 |
|---------|------|---------|------|
| 多个 GPIO 同时驱动同一线 | 总线争用，硬件损坏 | 确保单一驱动源；若需多驱动使用线与逻辑 + 上拉 | ✅ 无冲突 |
| 输入接 5V 信号 | GPIO 过压损坏 | 使用电平转换或分压网络 | ⚠️ 需用户实现 |
| LED 过载驱动 | LED 损坏/GPIO 损坏 | 必须串联限流电阻（220-330Ω） | ✅ 已说明 |
| 继电器线圈反向浪涌 | GPIO 损坏 | 必须外接续流二极管（1N4148） | ⚠️ 需用户实现 |
| ADC 输入超过 3.3V | ADC 数据不准或损坏 | 必须分压或隔离 | ⚠️ 需用户实现 |
| RS485 半双工冲突 | 数据碰撞，通信失败 | 控制 DE/RE GPIO，协议层处理 | ✅ 已实现 |

### 6.2 建议的外围电路

#### 6.2.1 LED 驱动

```
GPIO2 ──→ [220Ω] ──→ [LED] ──→ GND
            (限流电阻，目标 3-6mA)
```

#### 6.2.2 继电器驱动

```
GPIO4 ──→ [10kΩ] ──→ [N-MOSFET 栅极]
               [AO3400 或 2N7002]
               
    漏极 ──→ [继电器线圈] ──→ +12V/+24V
    源极 ──→ [续流二极管 1N4148] ──→ +12V/+24V
              GND
```

#### 6.2.3 数字输入（干接点）

```
3.3V ──→ [10kΩ] ──→ GPIO6 ──→ [开关接地] ──→ GND
         (上拉电阻)
         
[0.1µF] (去耦)
  ││
  └─ GND
```

#### 6.2.4 ADC 输入（模拟信号）

```
信号源 ──→ [100Ω] ──→ GPIO26 (AI0)
                       │
                    [0.1µF] ──→ GND
           (RC 低通滤波)
```

#### 6.2.5 RS485 收发器连接

```
GPIO0 (TX) ──→ MAX485 DI
GPIO1 (RX) ←── MAX485 RO
GPIO24 (DE) ──→ MAX485 DE
GPIO25 (RE) ──→ MAX485 RE

RS485 A/B ──→ [120Ω 终端电阻（仅在两端）] ──→ 对方设备
```

---

## 7. 初始化顺序与代码实现

### 7.1 初始化流程

```c
// 在 main() 中调用：
int main() {
    stdio_init_all();
    
    // 1. 初始化 I/O 管理器
    io_manager_init(&g_io_mgr);
    
    // 2. 启动指示灯
    io_set_led_run(true);
    
    // 3. 初始化 PLC 核心
    fx3u_core_t *plc = fx3u_init();
    fx3u_start(plc);
    
    // 4. 主循环
    while (1) {
        // 定期更新输入与 ADC
        io_manager_update();
        
        // 运行 PLC 扫描周期
        fx3u_scan_cycle(plc);
        
        sleep_ms(200);  // PLC 扫描周期 200ms
    }
}
```

### 7.2 关键初始化函数

#### 数字输入初始化
```c
// 在 include/fx3u_io.h 中：
#define PICO_INPUT_X0_GPIO  6
#define PICO_INPUT_PULL_UP  true
#define PICO_INPUT_DEBOUNCE_MS  20

// 在 src/fx3u_io.c 的 io_manager_init() 中：
for (int i = 0; i < PICO_TOTAL_INPUTS; i++) {
    gpio_init(input_gpios[i]);
    gpio_set_dir(input_gpios[i], GPIO_IN);
    if (PICO_INPUT_PULL_UP) {
        gpio_pull_up(input_gpios[i]);  // 建议上拉
    }
}
```

#### 数字输出初始化
```c
for (int i = 0; i < PICO_OUTPUT_COUNT; i++) {
    gpio_init(output_gpios[i]);
    gpio_set_dir(output_gpios[i], GPIO_OUT);
    gpio_put(output_gpios[i], 0);  // 初始关闭
}
```

#### UART0 与 RS485 初始化
```c
uart_init(PICO_UART0_INSTANCE, PICO_UART_BAUDRATE);  // 9600 bps
gpio_set_function(0, GPIO_FUNC_UART);  // TX
gpio_set_function(1, GPIO_FUNC_UART);  // RX

// RS485 DE/RE 控制
gpio_init(24);
gpio_set_dir(24, GPIO_OUT);
gpio_put(24, 0);  // 初始接收模式
```

#### ADC 初始化
```c
adc_init();
adc_gpio_init(26);  // AI0
adc_gpio_init(27);  // AI1
adc_gpio_init(28);  // PVD
```

---

## 8. 诊断与测试

### 8.1 硬件自检命令

```c
// 在串口调试时调用：
io_diagnostic_report();

// 输出示例：
// ===== I/O 诊断报告 =====
// 初始化状态: OK
// RUN LED: ON
// ERR LED: OFF
// RUN 开关: 释放
// 
// 数字输入 (X0-X9):
//   X0: 1
//   X1: 0
//   ... (省略)
// 
// 模拟输入:
//   AI0: 1650 mV
//   AI1: 0 mV
//   PVD: 3300 mV
// 
// RS485 状态:
//   DE (发送): 0
//   RE (接收): 0
// ========================
```

### 8.2 测试步骤

| 测试项 | 步骤 | 预期结果 |
|-------|------|---------|
| LED 闪烁 | 调用 `io_set_led_run(true/false)` | LED 按高电平打开/关闭 |
| 数字输入 | 接地 X0，读取 `io_read_input_relay(0)` | 返回 0 或 1（取决于极性） |
| 数字输出 | 调用 `io_write_output_relay(0, 1)` | Y0 GPIO22 置高 |
| ADC 采样 | 连接 0-3.3V 电源到 AI0，读取 `io_read_adc_ai0()` | 返回 0-4095 |
| RS485 发送 | 调用 `io_rs485_set_de(true)` | GPIO24 置高 |

---

## 9. 注意事项与常见错误

### 9.1 常见硬件错误 ❌

1. **直接驱动继电器线圈** → GPIO 烧毁  
   💡 必须用外部 MOSFET/驱动器

2. **连接 5V 信号到 RX (GPIO1)** → 芯片损坏  
   💡 需电平转换或分压

3. **ADC 输入超过 3.3V** → ADC 数据不准  
   💡 必须分压或隔离

4. **继电器反向浪涌无续流二极管** → GPIO 尖峰损坏  
   💡 在线圈两端并联 1N4148

5. **LED 无限流电阻** → LED 和 GPIO 烧毁  
   💡 必须串联 220-330Ω

### 9.2 初始化错误 ❌

6. **忘记初始化 UART RX 的 GPIO 功能** → 串口无法接收  
   💡 必须 `gpio_set_function(1, GPIO_FUNC_UART)`

7. **ADC 通道没有初始化** → 读数不稳定  
   💡 必须 `adc_gpio_init(gpio_num)`

8. **RS485 DE/RE 极性反了** → 无法通信  
   💡 DE 高有效（发送），RE 低有效（接收）

### 9.3 软件错误 ❌

9. **输入去抖时间过短** → 接触抖动导致误触  
   💡 建议 ≥20 ms

10. **忘记定期调用 `io_manager_update()`** → 输入数据陈旧  
    💡 在主循环中每 200 ms 调用一次

---

## 10. 参考文档

- **Raspberry Pi Pico 数据手册**：https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf
- **RP2040 数据手册**：https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
- **Pico SDK 文档**：https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf
- **FX3U 通信手册**（参考原始项目）
- **MODBUS RTU 规范**：IEC 61131-3 / Modicon

---

## 11. 变更历史

| 版本 | 日期 | 描述 |
|------|------|------|
| v1.0 | 2025-12-11 | 初版，基于用户提供的物理引脚分配 |

---

## 附录 A：完整 GPIO 映射一览表

```
GPIO  | 功能           | 物理引脚 | 方向   | 上拉 | 说明
------|----------------|---------|--------|------|---
0     | UART0 TX       | Pin 1   | 输出   | —    | RS485 驱动输入
1     | UART0 RX       | Pin 2   | 输入   | —    | RS485 反馈输出
2     | RUN_LED        | Pin 4   | 输出   | 下   | 运行指示灯
3     | ERR_LED        | Pin 5   | 输出   | 下   | 错误指示灯
4     | Y7             | Pin 6   | 输出   | —    | 继电器输出
5     | Y8             | Pin 7   | 输出   | —    | 继电器输出
6     | X0             | Pin 9   | 输入   | 上   | 数字输入
7     | X1             | Pin 10  | 输入   | 上   | 数字输入
8     | X2             | Pin 11  | 输入   | 上   | 数字输入
9     | X3             | Pin 12  | 输入   | 上   | 数字输入
10    | X4             | Pin 14  | 输入   | 上   | 数字输入
11    | X5             | Pin 15  | 输入   | 上   | 数字输入
12    | X6             | Pin 16  | 输入   | 上   | 数字输入
13    | X7             | Pin 17  | 输入   | 上   | 数字输入
14    | X8             | Pin 19  | 输入   | 上   | 数字输入
15    | X9             | Pin 20  | 输入   | 上   | 数字输入
16    | Y6             | Pin 21  | 输出   | —    | 继电器输出
17    | Y5             | Pin 22  | 输出   | —    | 继电器输出
18    | Y4             | Pin 24  | 输出   | —    | 继电器输出
19    | Y3             | Pin 25  | 输出   | —    | 继电器输出
20    | Y2             | Pin 26  | 输出   | —    | 继电器输出
21    | Y1             | Pin 27  | 输出   | —    | 继电器输出
22    | Y0             | Pin 29  | 输出   | —    | 继电器输出
23    | RUN 开关       | Pin 30  | 输入   | 上   | 运行/停止开关
24    | RS485 DE       | —       | 输出   | 下   | 驱动使能（建议额外）
25    | RS485 RE       | —       | 输出   | 下   | 接收使能（建议额外）
26    | AI0 / ADC0     | Pin 31  | 输入   | —    | 模拟输入（12-bit）
27    | AI1 / ADC1     | Pin 32  | 输入   | —    | 模拟输入（12-bit）
28    | PVD / ADC2     | Pin 34  | 输入   | —    | 掉电检测（12-bit）
```

---

**报告编制者**：FX3U Pico 模拟器开发团队  
**审核状态**：✅ 已审核并导入源码  
**最后更新**：2025-12-11
