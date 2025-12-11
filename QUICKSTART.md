# Pico FX3U 模拟器 - 快速开始指南

## 5分钟快速入门

### 第1步: 准备环境

```bash
# 1. 克隆或下载本项目
# 2. 确保已安装Pico SDK
export PICO_SDK_PATH=~/pico-sdk

# 3. 进入项目目录
cd pico-fx3u-simulator
```

### 第2步: 编译

```bash
# 创建构建目录
mkdir -p build
cd build

# 配置和编译
cmake ..
make -j4

# 检查构建输出
ls -la *.uf2
```

### 第3步: 烧录

```bash
# 方法1: 直接复制 (macOS/Linux)
cp pico_fx3u_simulator.uf2 /Volumes/RPI-RP2/

# 方法2: 使用picotool
picotool load pico_fx3u_simulator.elf

# 方法3: 手动拖放
# - 按住BOOTSEL键并连接USB
# - 将.uf2文件拖到出现的驱动器
```

### 第4步: 测试

```bash
# 连接USB并打开串口监控
minicom -D /dev/ttyACM0 -b 115200

# 在终端输入:
# s - 启动PLC
# d - 显示状态
# t - 停止PLC
```

## 程序流程示例

### 示例1: 简单的继电器操作

```c
#include "fx3u_core.h"

fx3u_core_t plc;

// 初始化
fx3u_core_init(&plc);
fx3u_core_start(&plc);

// 设置输入X0
fx3u_set_input(&plc, 0, 1);

// 执行逻辑: X0 -> Y0 (LED亮)
if (fx3u_get_input(&plc, 0)) {
    fx3u_set_output(&plc, 0, 1);
}

// 通过GPIO控制LED
io_write_output_relay(0, fx3u_get_output(&plc, 0));
```

### 示例2: 定时器应用

```c
// 启动100ms定时器
fx3u_timer_start(&plc, 0, 100);

// 在每个周期检查
if (fx3u_timer_done(&plc, 0)) {
    fx3u_set_output(&plc, 1, 1);  // 输出ON
    fx3u_timer_stop(&plc, 0);
}
```

### 示例3: 数据寄存器和计算

```c
// 存储数值
fx3u_set_register(&plc, 0, 100);
fx3u_set_register(&plc, 1, 50);

// 执行加法: D0 + D1 = D2
int16_t val1 = fx3u_get_register(&plc, 0);
int16_t val2 = fx3u_get_register(&plc, 1);
fx3u_set_register(&plc, 2, val1 + val2);
```

## MODBUS通信示例

### Python客户端示例

```python
#!/usr/bin/env python3
import serial
import time

# 打开串口
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

def send_modbus_command(data):
    """发送MODBUS命令"""
    ser.write(bytes(data))
    time.sleep(0.1)
    response = ser.read(256)
    return response

# 示例: 读寄存器 (从机ID=1, 功能码=3, 地址=0, 数量=10)
# MODBUS RTU: [SlaveID] [FunctionCode] [AddrHi] [AddrLo] [QttyHi] [QttyLo] [CRCLo] [CRCHi]
cmd = [0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0x44, 0x09]
response = send_modbus_command(cmd)

print("Response:", response.hex())

ser.close()
```

### Node.js 示例

```javascript
const SerialPort = require('serialport');
const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600 });

port.on('open', () => {
    // 发送MODBUS命令
    const cmd = Buffer.from([0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0x44, 0x09]);
    port.write(cmd, (err) => {
        if (err) console.error(err);
    });
});

port.on('data', (data) => {
    console.log('Response:', data.toString('hex'));
});
```

## 常见配置修改

### 修改扫描周期

编辑 `src/main.c`:
```c
timer_cfg.period_us = 100000;  // 改为100ms
```

### 修改MODBUS站号

编辑 `src/main.c`:
```c
g_comm_config.station_id = 2;  // 改为ID 2
```

### 修改波特率

编辑 `src/main.c`:
```c
g_rs485_config.baudrate = 19200;  // 改为19200 bps
```

## 性能优化建议

### 内存优化

```c
/* 减少内存占用 */
#define PLC_MAX_INPUTS      64      // 从256减少到64
#define PLC_MAX_OUTPUTS     64
#define PLC_MAX_INTERNALS   512     // 从2048减少到512
#define PLC_MAX_REGISTERS   1024    // 从4096减少到1024
```

### CPU优化

```c
/* 增加扫描周期以降低CPU占用 */
#define SCAN_PERIOD_MS      500     // 500ms而非200ms
```

## 故障排除

### 问题: "PICO_SDK_PATH not found"
```bash
# 解决方案
export PICO_SDK_PATH=~/pico-sdk
cmake ..
```

### 问题: 编译失败，提示缺少arm工具链
```bash
# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi

# macOS
brew install arm-none-eabi-gcc
```

### 问题: 固件烧录后无反应
```bash
# 1. 检查USB连接
# 2. 重新按住BOOTSEL并连接
# 3. 检查CPU是否被烧毁 (LED闪烁检查)
```

### 问题: MODBUS通信无响应
```bash
# 1. 检查波特率 (应该是9600)
# 2. 检查RS485接线
# 3. 验证站号是否正确 (默认1)
# 4. 在终端输入 'd' 检查PLC状态
```

## 更新和维护

### 更新Pico SDK
```bash
cd ~/pico-sdk
git pull
git submodule update --init
```

### 清理构建文件
```bash
cd pico-fx3u-simulator/build
rm -rf *
cmake ..
make
```

### 备份和恢复

```bash
# 备份Flash内容
picotool save backup.bin

# 恢复Flash
picotool load backup.bin
```

## 下一步

1. 查看 `README.md` 获取完整文档
2. 浏览 `include/` 目录了解API
3. 参考 `src/main.c` 了解实现细节
4. 尝试修改代码并自定义功能

## 更多资源

- [Pico开发者文档](https://datasheets.raspberrypi.org/pico/pico_datasheet.pdf)
- [RP2040芯片手册](https://datasheets.raspberrypi.org/rp2040/rp2040_datasheet.pdf)
- [Pico SDK示例](https://github.com/raspberrypi/pico-examples)
- [FX3U PLC参考](https://www.mitsubishielectric.co.jp/)

---

祝您开发愉快！遇到问题欢迎反馈。
