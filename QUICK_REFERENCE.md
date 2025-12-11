# Pico FX3U 模拟器 - 快速参考卡片

## 关键信息速查表

### 1. GPIO 引脚分配表

| GPIO | 功能 | 说明 |
|------|------|------|
| 0-7 | 输入(X) | FX3U 输入继电器 X0-X7 |
| 8-15 | 输出(Y) | FX3U 输出继电器 Y0-Y7 |
| 16 | I2C SDA | I2C 数据线 |
| 17 | I2C SCL | I2C 时钟线 |
| 18 | SPI CLK | SPI 时钟 |
| 19 | SPI MOSI | SPI 主出从入 |
| 20 | UART RX | RS485 接收 |
| 21 | UART TX | RS485 发送 |
| 22 | RS485 RTS | RS485 方向控制 |
| 23 | LED | Pico 内置LED |
| 24 | VBUS | USB 电源检测 |
| 25 | GPIO25 | 内置LED（备用） |
| 26-29 | ADC | 模拟输入通道 |

### 2. 内存分配速查

| 类型 | 地址范围 | 数量 | 字节 |
|------|---------|------|------|
| 输入(X) | X0-X255 | 256 | 256 |
| 输出(Y) | Y0-Y255 | 256 | 256 |
| 内部(M) | M0-M2047 | 2048 | 2048 |
| 定时器(T) | T0-T127 | 128 | 1536 |
| 计数器(C) | C0-C127 | 128 | 1536 |
| 寄存器(D) | D0-D4095 | 4096 | 8192 |
| **总计** | | | **~15KB** |

### 3. 特殊寄存器映射

| 地址 | 名称 | 用途 |
|------|------|------|
| D8000 | 扫描时间 | 默认200ms |
| D8001 | PLC版本 | 0x5EF6 |
| D8002 | 内存大小 | 16 (KB) |
| D8006 | 错误代码 | 错误诊断 |
| D8010 | 扫描计数 | 周期计数 |
| D8120 | 通信方式 | 通信配置 |
| D8121 | 站号 | 默认1 |

### 4. 指令速查表

#### 基础指令
```
LD addr     - 装入
AND addr    - 并联
OR addr     - 串联
NOT         - 反向
OUT addr    - 输出
```

#### 定时器/计数器
```
TMR n, addr - 定时器 (n=0-127, 单位ms)
CNT n, addr - 计数器 (n=0-127)
```

#### 数据处理
```
MOV s, d      - 数据传送
ADD a1, a2, r - 加法
SUB a1, a2, r - 减法
MUL a1, a2, r - 乘法
DIV a1, a2, r - 除法
CMP a1, a2    - 比较
```

#### 位操作
```
SET addr - 置位
RST addr - 复位
PLS addr - 脉冲 (上升沿)
```

### 5. 波特率配置表

| 代码 | 波特率 | 备注 |
|------|--------|------|
| 0001 | 300 | 低速 |
| 0100 | 600 | - |
| 0101 | 1200 | - |
| 0110 | 2400 | - |
| 0111 | 4800 | - |
| 1000 | 9600 | 默认 |
| 1001 | 19200 | - |
| 1010 | 38400 | - |
| 1011 | 56000 | - |
| 1100 | 57600 | - |
| 1101 | 115200 | 高速 |

### 6. MODBUS 功能码

| 功能码 | 功能 | 操作 |
|--------|------|------|
| 0x01 | 读线圈 | READ |
| 0x02 | 读输入 | READ |
| 0x03 | 读寄存器 | READ |
| 0x04 | 读输入寄存器 | READ |
| 0x05 | 写线圈 | WRITE |
| 0x06 | 写寄存器 | WRITE |
| 0x0F | 写多线圈 | WRITE |
| 0x10 | 写多寄存器 | WRITE |

### 7. 编译和烧录命令速查

```bash
# 配置和编译
export PICO_SDK_PATH=~/pico-sdk
mkdir build && cd build
cmake ..
make -j4

# 烧录 (macOS)
cp pico_fx3u_simulator.uf2 /Volumes/RPI-RP2/

# 烧录 (Linux)
cp pico_fx3u_simulator.uf2 /media/$USER/RPI-RP2/

# 使用picotool烧录
picotool load pico_fx3u_simulator.elf
```

### 8. 串口命令速查

| 命令 | 功能 |
|------|------|
| s | 启动PLC |
| t | 停止PLC |
| d | 显示状态 |
| r | 复位PLC |
| ? | 显示帮助 |

### 9. Python MODBUS 示例速查

```python
import serial
from pymodbus.client.sync import ModbusSerialClient

client = ModbusSerialClient(method='rtu', port='COM3', baudrate=9600)
client.connect()

# 读寄存器
result = client.read_holding_registers(0, 10, unit=1)
print(result.registers)

# 写寄存器
client.write_register(0, 100, unit=1)

client.close()
```

### 10. 数据类型对应表

| 类型 | 位宽 | 范围 | 说明 |
|------|------|------|------|
| 位 (bit) | 1 | 0-1 | 继电器 |
| 字 (word) | 16 | -32768 ~ 32767 | 有符号 |
| 双字 | 32 | (不直接支持) | 需用两个寄存器 |
| 浮点数 | 32 | (扩展功能) | v1.1计划 |

### 11. 性能指标速查

| 指标 | 值 | 单位 |
|------|-----|------|
| 扫描周期 | 200 | ms |
| 最大继电器 | 2560 | 个 |
| 最大定时器 | 128 | 个 |
| 最大计数器 | 128 | 个 |
| 最大寄存器 | 4096 | 个 |
| RAM占用 | ~40 | KB |
| Flash占用 | ~80 | KB |
| CPU占用 | <10 | % |

### 12. 文件位置速查

| 文件 | 位置 | 用途 |
|------|------|------|
| 源代码 | src/ | 可编译源文件 |
| 头文件 | include/ | API定义 |
| 文档 | ./ | 各种文档 |
| 构建目录 | build/ | 编译输出 |

### 13. 编译错误排查速查表

| 错误信息 | 解决方案 |
|---------|--------|
| "PICO_SDK_PATH not found" | `export PICO_SDK_PATH=~/pico-sdk` |
| "arm-none-eabi-gcc not found" | 安装 GCC ARM 工具链 |
| "permission denied" | 检查USB驱动和用户权限 |
| "undefined reference" | 检查链接库顺序 |
| "memory exhausted" | 减少 PLC_MAX_* 常数 |

### 14. 引脚电压标准

| 引脚类型 | 电压 | 电流 |
|---------|------|------|
| GPIO输出 | 3.3V | ≤4mA |
| GPIO输入 | 3.3V | 支持 |
| ADC输入 | 0-3.3V | - |
| USB | 5V | - |

### 15. 通信协议栈

```
应用层:  MODBUS / 三菱协议
传输层:  UART (RS485) / TCP/UDP
物理层:  RS485 / 以太网 (RJ45)
```

## 常用代码片段

### 启动PLC并运行10个周期
```c
fx3u_core_t plc;
fx3u_core_init(&plc);
fx3u_core_start(&plc);

for (int i = 0; i < 10; i++) {
    fx3u_core_run_cycle(&plc);
    sleep_ms(200);
}
```

### 读写继电器
```c
fx3u_set_input(&plc, 0, 1);    // X0 = ON
uint8_t val = fx3u_get_input(&plc, 0);
fx3u_set_output(&plc, 5, 1);   // Y5 = ON
```

### 使用定时器
```c
fx3u_timer_start(&plc, 0, 100);  // 启动T0, 100ms
if (fx3u_timer_done(&plc, 0)) {
    printf("Timer expired!\r\n");
}
```

### 数据寄存器计算
```c
fx3u_set_register(&plc, 0, 50);
fx3u_set_register(&plc, 1, 30);
fx3u_add(&plc, 0, 1, 2);  // D2 = D0 + D1 = 80
```

### 发送MODBUS命令
```c
uint8_t cmd[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0x44, 0x09};
rs485_send(cmd, 8);
int len = rs485_receive(rx_buf, 256);
```

---

**打印此卡片以供快速参考！**

更多详情请参考完整文档: README.md, API.md, BUILD.md
