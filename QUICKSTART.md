# 快速开始指南 - Pico FX3U 模拟器

## ✅ 项目状态

- **固件编译**: ✅ 成功
- **UF2 文件**: ✅ 已生成
- **GitHub 仓库**: https://github.com/Cpsunn/pico-3n
- **CI/CD 状态**: ✅ 自动构建中

## 5 分钟快速入门

### 第 1 步: 获取固件

**方式 A: 从 GitHub Actions 下载（推荐）**

1. 打开 https://github.com/Cpsunn/pico-3n/actions/workflows/build-uf2.yml
2. 找到最新的 ✅ 成功的 workflow run
3. 向下滚动到 "Artifacts" 部分
4. 下载 "firmware" ZIP 文件
5. 解压，找到 `pico_fx3u_simulator.uf2`

**方式 B: 本地编译**

```bash
# 前提: 已安装 cmake, arm-none-eabi-gcc, Pico SDK
export PICO_SDK_PATH=~/pico/pico-sdk
mkdir -p build && cd build
cmake ..
make -j4
# 输出文件: pico_fx3u_simulator.uf2
```

### 第 2 步: 烧录到 Pico

**准备 Pico:**
1. 按住 **BOOTSEL** 按钮
2. 连接 USB 到电脑（保持按住 BOOTSEL）
3. 释放按钮 → Pico 应显示为 `RPI-RP2` 驱动器

**复制文件:**

```bash
# macOS
cp pico_fx3u_simulator.uf2 /Volumes/RPI-RP2/

# Linux
cp pico_fx3u_simulator.uf2 /mnt/rpi-rp2/

# Windows: 使用文件浏览器拖放到 RPI-RP2 驱动器
```

**等待重启:**
- 文件复制完成后 Pico 自动重启
- 应看到 GPIO2 (RUN LED) 亮起 ✅

### 第 3 步: 验证

连接串口终端查看启动信息：

```bash
# macOS
screen /dev/tty.usbmodem14101 115200

# Linux  
screen /dev/ttyACM0 115200
```

**预期输出:**
```
=== Pico FX3U Simulator v1.0 ===
Initializing PLC core...
Initializing I/O manager...
Initializing RS485 communication...
...
PLC Status: Ready
```

### 第 4 步: 测试命令

在串口终端中输入：

```
d   # 诊断信息
s   # 系统状态
m   # 内存使用
r   # 复位 PLC
```

## 常见问题

### Q: 找不到 UF2 文件？

**A:** 确保选择了 ✅ 成功（绿色）的构建，不是 ❌ 失败的构建。最新的应该是 Run #11 或更新版本。

### Q: Pico 无法进入 Bootloader 模式？

**A:** 
- 检查是否正确按住 BOOTSEL 按钮
- 尝试不同的 USB 线或 USB 端口
- 详见 [FLASHING.md](FLASHING.md)

### Q: 烧录后无输出？

**A:**
- 检查 LED：RUN LED 应亮，ERR LED 应灭
- 检查串口设置：波特率 115200，数据位 8，停止位 1
- 尝试命令 `d` 获取诊断信息

## 项目配置

### 硬件映射

```
GPIO0-1    → UART TX/RX（RS485 通信）
GPIO2      → RUN LED（绿色）
GPIO3      → ERR LED（红色）
GPIO4-5    → Y0-Y1（数字输出）
GPIO6-15   → X0-X9（数字输入）
GPIO16-22  → Y2-Y8（数字输出）
GPIO23     → RUN 开关（去抖）
GPIO24-25  → RS485 DE/RE（驱动控制）
GPIO26-28  → AI0-AI2（模拟输入）
```

详细映射见 [IO_PIN_CONFIGURATION_REPORT.md](IO_PIN_CONFIGURATION_REPORT.md)

### 通信配置

- **协议**: MODBUS RTU
- **波特率**: 9600 bps
- **数据格式**: 8N1（8 位数据, 无校验, 1 停止位）
- **模式**: 从机（地址 1）

## 文档导航

| 文档 | 内容 |
|------|------|
| [README.md](README.md) | 项目完整说明 |
| [FLASHING.md](FLASHING.md) | 详细烧录指南 |
| [IO_PIN_CONFIGURATION_REPORT.md](IO_PIN_CONFIGURATION_REPORT.md) | 硬件引脚详情 |
| [API.md](API.md) | API 函数文档 |
| [BUILD.md](BUILD.md) | 构建系统说明 |

## 下一步

1. ✅ 下载 UF2 文件并烧录（本指南完成）
2. 🔧 通过串口连接验证运行
3. 📡 测试 RS485/MODBUS 通信（如需要）
4. 🎯 根据需要定制功能

## 反馈和支持

- 📖 查阅详细文档了解更多
- 🐛 遇到问题参考 [FLASHING.md](FLASHING.md) 故障排除
- 💡 需要修改功能查看源代码注释

---

**状态**: ✅ 构建成功  
**版本**: 1.0 (Pico SDK)  
**最后更新**: 2025-12-11



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
