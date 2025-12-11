# Raspberry Pi Pico FX3U PLC 模拟器

## 简介

这是一个基于 Raspberry Pi Pico 的三菱 FX3U PLC 模拟器实现，用于在资源受限的微控制器上模拟 FX3U PLC 的核心功能。

### 核心特性

1. **完整的FX3U PLC核心引擎**
   - 继电器管理 (输入X、输出Y、内部M)
   - 定时器和计数器
   - 数据寄存器
   - 特殊寄存器支持

2. **基本指令集**
   - 逻辑指令 (LD, AND, OR, NOT, OUT)
   - 定时器和计数器指令 (TMR, CNT)
   - 数据处理指令 (MOV, ADD, SUB, MUL, DIV, CMP)
   - 位操作指令 (SET, RST, PLS)

3. **通信接口**
   - RS485 MODBUS RTU 协议
   - 支持主机/从机模式
   - 完整的MODBUS功能码实现

4. **I/O映射**
   - GPIO 0-7: 输入继电器 (X0-X7)
   - GPIO 8-15: 输出继电器 (Y0-Y7)
   - GPIO 20-22: RS485控制
   - ADC通道支持
   - PWM输出支持

5. **优化的硬件支持**
   - 针对Pico的2MB Flash优化
   - 258KB RAM管理
   - UART通信
   - GPIO控制
   - 定时器管理

## 硬件要求

### 最小配置
- Raspberry Pi Pico (RP2040)
- Micro-USB电源
- RS485收发器模块 (MAX485等)

### 推荐配置
- Pico + RS485模块
- 可选: W5500以太网模块
- 可选: 16x2 LCD显示屏

## 硬件连接

### RS485连接
```
Pico GPIO20 (RX) -> RS485 RO (接收输出)
Pico GPIO21 (TX) -> RS485 DI (驱动输入)
Pico GPIO22      -> RS485 RE/DE (接收使能/驱动使能)
GND              -> RS485 GND
```

### I/O连接
```
GPIO0-GPIO7   -> 输入继电器 (X0-X7)
GPIO8-GPIO15  -> 输出继电器 (Y0-Y7)
GPIO16-GPIO17 -> I2C (可选)
GPIO18-GPIO19 -> SPI (可选)
```

## 项目结构

```
pico-fx3u-simulator/
├── CMakeLists.txt              # CMake构建配置
├── include/                    # 头文件目录
│   ├── fx3u_core.h            # PLC核心接口
│   ├── fx3u_instructions.h     # 指令集定义
│   ├── fx3u_io.h              # I/O管理接口
│   ├── communication.h         # 通信接口
│   ├── modbus_protocol.h       # MODBUS协议
│   ├── rs485_driver.h          # RS485驱动
│   ├── ethernet_adapter.h      # 以太网适配器
│   ├── memory_manager.h        # 内存管理
│   └── timer.h                 # 定时器管理
├── src/                        # 源文件目录
│   ├── main.c                  # 主程序
│   ├── fx3u_core.c            # PLC核心实现
│   ├── fx3u_instructions.c     # 指令执行
│   ├── fx3u_io.c              # I/O实现
│   ├── communication.c         # 通信实现
│   ├── modbus_protocol.c       # MODBUS实现
│   ├── rs485_driver.c          # RS485实现
│   ├── ethernet_adapter.c      # 以太网实现
│   ├── memory_manager.c        # 内存管理实现
│   └── timer.c                 # 定时器实现
└── lib/                        # 第三方库 (可选)
```

## 构建说明

### 前置条件

1. 安装 Pico SDK:
```bash
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
```

2. 设置环境变量:
```bash
export PICO_SDK_PATH=~/pico-sdk
```

3. 安装构建工具:
```bash
# macOS
brew install cmake arm-none-eabi-gcc

# Linux (Ubuntu/Debian)
sudo apt-get install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

# Windows (使用WSL或MSYS2)
```

### 编译步骤

```bash
cd /path/to/pico-fx3u-simulator
mkdir build
cd build
cmake ..
make
```

编译成功后会生成:
- `pico_fx3u_simulator.elf` - ELF文件
- `pico_fx3u_simulator.uf2` - UF2格式固件 (用于Pico)

### 烧录固件

1. 按住Pico上的BOOTSEL按钮，连接USB到计算机
2. Pico将以USB Mass Storage模式挂载
3. 将 `pico_fx3u_simulator.uf2` 拖到挂载的驱动器
4. Pico自动重启运行新固件

或使用命令:
```bash
cp pico_fx3u_simulator.uf2 /media/USERNAME/RPI-RP2/
```

## 使用说明

### 通过串口通信

1. 连接Pico到计算机的USB端口
2. 使用串口工具连接 (波特率: 115200)
3. 可用命令:
   - `s` - 启动PLC
   - `t` - 停止PLC
   - `d` - 显示PLC状态
   - `r` - 复位PLC
   - `?` - 显示帮助

### 通过MODBUS通信

使用MODBUS主站软件或工具与Pico通信:

```c
/* C语言示例 */
#include "modbus_protocol.h"

uint8_t buffer[256];

/* 读寄存器 (地址0, 数量10) */
int len = modbus_master_read_registers(buffer, 1, 0, 10);
// 发送buffer到Pico...

/* 写寄存器 (地址0, 值100) */
len = modbus_master_write_register(buffer, 1, 0, 100);
// 发送buffer到Pico...
```

### Python示例

```python
import serial
from pymodbus.client.sync import ModbusSerialClient as ModbusClient

# 连接Pico
client = ModbusClient(method='rtu', port='COM3', baudrate=9600)
client.connect()

# 读寄存器 (地址0, 数量10)
result = client.read_holding_registers(0, 10, unit=1)
print(result.registers)

# 写寄存器 (地址0, 值100)
result = client.write_register(0, 100, unit=1)

client.close()
```

## API 文档

### PLC核心

```c
/* 初始化 */
void fx3u_core_init(fx3u_core_t *plc);

/* 运行控制 */
void fx3u_core_start(fx3u_core_t *plc);
void fx3u_core_stop(fx3u_core_t *plc);
void fx3u_core_run_cycle(fx3u_core_t *plc);

/* 继电器访问 */
void fx3u_set_input(fx3u_core_t *plc, uint16_t addr, uint8_t value);
uint8_t fx3u_get_input(fx3u_core_t *plc, uint16_t addr);
void fx3u_set_output(fx3u_core_t *plc, uint16_t addr, uint8_t value);
uint8_t fx3u_get_output(fx3u_core_t *plc, uint16_t addr);

/* 数据寄存器 */
void fx3u_set_register(fx3u_core_t *plc, uint16_t addr, int16_t value);
int16_t fx3u_get_register(fx3u_core_t *plc, uint16_t addr);
```

### I/O管理

```c
/* GPIO控制 */
void io_set_gpio_output(uint8_t gpio_num, bool value);
bool io_get_gpio_input(uint8_t gpio_num);

/* 继电器映射 */
void io_write_output_relay(uint8_t relay_addr, uint8_t value);
uint8_t io_read_input_relay(uint8_t relay_addr);
```

### MODBUS协议

```c
/* 初始化 */
void modbus_init(modbus_config_t *config, uint8_t slave_id);

/* 帧处理 */
int modbus_slave_process(fx3u_core_t *plc, uint8_t *rx_buffer, 
                         uint16_t rx_len, uint8_t *tx_buffer);
```

## 性能指标

- **扫描周期**: 200ms (可配置)
- **最小内存占用**: ~40KB RAM
- **最大程序大小**: ~100KB Flash
- **通信速率**: 最高115200 bps
- **并发连接**: 1个MODBUS从机

## 与三菱编程软件的兼容性

本模拟器支持三菱GX Works等编程软件通过MODBUS协议进行通信，但不支持直接的GX Works 3.0专用协议。建议使用：

1. **MODBUS工具** - 推荐用于测试和调试
2. **自定义上位机** - 使用MODBUS协议
3. **HMI软件** - 支持MODBUS的HMI系统

## 优化建议

### 对于Pico硬件限制

1. **Flash优化**
   - 使用压缩的程序代码格式
   - 减少调试信息
   - 合并相似的指令

2. **RAM优化**
   - 使用固定大小的缓冲区
   - 避免动态内存分配
   - 使用静态数据结构

3. **性能优化**
   - 增加扫描周期以降低CPU使用率
   - 使用中断而非轮询
   - 优化通信帧处理

## 扩展功能

### 支持的扩展

1. **以太网 (W5500)**
   - TCP/UDP通信
   - 网络诊断

2. **模拟输入 (ADC)**
   - 4通道12位ADC
   - 温度传感器

3. **PWM输出**
   - 频率和占空比可调
   - 多通道输出

## 故障排除

### 常见问题

1. **固件烧录失败**
   - 检查USB连接
   - 确保BOOTSEL按钮被正确按住
   - 尝试重新插拔USB

2. **通信无响应**
   - 检查RS485接线
   - 验证波特率设置 (默认9600)
   - 检查站号设置 (默认1)

3. **PLC不运行**
   - 使用命令 `s` 启动PLC
   - 检查USB控制台输出
   - 查看错误代码

### 调试建议

使用USB串口监控:
```bash
# 使用minicom (Linux/macOS)
minicom -D /dev/ttyACM0 -b 115200

# 使用picocom (Linux)
picocom /dev/ttyACM0 -b 115200

# 使用PuTTY (Windows)
# 连接到COM端口, 波特率115200
```

## 许可证

MIT License

## 贡献指南

欢迎提交Issue和Pull Request！

## 参考资源

- [Pico SDK文档](https://github.com/raspberrypi/pico-sdk)
- [FX3U PLC手册](https://www.mitsubishielectric.com/)
- [MODBUS规范](http://www.modbus.org/)
- [RS485标准](https://en.wikipedia.org/wiki/RS-485)

## 技术支持

如有问题，请：

1. 查看项目的Issue列表
2. 提供详细的错误日志
3. 描述硬件配置
4. 提供复现步骤

---

**开发者**: AI Programming Assistant  
**版本**: 1.0  
**最后更新**: 2024年12月  
