# Flashing UF2 Firmware to Pico

本文档说明如何将编译生成的 UF2 固件烧录到 Raspberry Pi Pico。

## 获取 UF2 文件

### 方法 1：从 GitHub Actions 下载（推荐）

1. 访问 GitHub Actions 页面：
   https://github.com/Cpsunn/pico-3n/actions/workflows/build-uf2.yml

2. 选择最新的成功的 "Build UF2" workflow run（状态为绿色）

3. 点击该 run，向下滚动到 "Artifacts" 部分

4. 点击 "firmware" artifact 下载 ZIP 文件

5. 解压 ZIP 文件，找到 `pico_fx3u_simulator.uf2` 文件

### 方法 2：本地构建 UF2

如果已经安装了编译工具链（arm-none-eabi-gcc, cmake, Pico SDK），可以在本地构建：

```bash
mkdir -p build
cd build
cmake .. -DPICO_SDK_PATH=~/pico/pico-sdk
make -j$(nproc)
```

编译完成后，UF2 文件应该在 `build/` 目录中。

## 烧录步骤

### 步骤 1：进入 Bootloader 模式

1. **断开 Pico 的 USB 连接**（如果已连接）

2. **按住 BOOTSEL 按钮**（Pico 上的物理按钮）

3. **连接 USB 到电脑**，同时保持按住 BOOTSEL 按钮

4. **松开 BOOTSEL 按钮**

此时 Pico 应该被识别为 USB 存储设备，挂载点为 `RPI-RP2`。

### 步骤 2：复制 UF2 文件

**macOS:**
```bash
cp pico_fx3u_simulator.uf2 /Volumes/RPI-RP2/
# 或者使用 Finder 拖放文件到 RPI-RP2 驱动器
```

**Linux:**
```bash
cp pico_fx3u_simulator.uf2 /mnt/rpi-rp2/
```

**Windows (PowerShell):**
```powershell
Copy-Item pico_fx3u_simulator.uf2 -Destination D:\ # 根据实际盘符调整
```

### 步骤 3：验证烧录

1. 文件复制完成后，Pico 会**自动重启并断开 USB 连接**

2. 等待 1-2 秒

3. Pico 会自动重新连接为串口设备

4. LED 状态：
   - **GPIO2 (RUN LED)** 应该亮起（绿色）
   - **GPIO3 (ERR LED)** 应该熄灭（无错误）

## 验证固件运行

### 通过 USB 串口连接

烧录完成后，Pico 会以 CDC (USB 串口) 设备身份出现。

**macOS 上连接：**

```bash
# 查看可用的串口设备
ls -la /dev/tty.usbmodem*

# 使用 screen 连接（默认波特率 115200）
screen /dev/tty.usbmodem14101 115200

# 或使用 minicom
minicom -D /dev/tty.usbmodem14101 -b 115200
```

**预期输出：**
```
=== Pico FX3U Simulator v1.0 ===
Initializing PLC core...
Initializing I/O manager...
Initializing RS485 communication...
Initializing communication interface...
Initializing MODBUS protocol...
System initialization completed.
PLC Status: Ready
```

### 与 PLC 交互

连接后可以发送以下命令：

- `d` - 显示诊断信息
- `r` - 复位 PLC
- `s` - 显示系统状态
- `m` - 显示内存使用情况

### 测试功能

连接 RS485 设备（支持 MODBUS RTU）：

```
- GPIO24: RS485 DE (驱动使能) - 3.3V TTL
- GPIO25: RS485 RE (接收使能) - 3.3V TTL
- GPIO0: UART TX
- GPIO1: UART RX
- 波特率：9600 bps
- 数据格式：8N1（8 位数据, 无校验, 1 停止位）
```

## 故障排除

### 问题：Pico 无法进入 Bootloader 模式

- 检查 BOOTSEL 按钮是否正确按下
- 尝试使用不同的 USB 线（某些线可能是充电线，不支持数据传输）
- 尝试连接到电脑的不同 USB 端口

### 问题：烧录后 Pico 无法启动

- 检查 LED 状态，如果 ERR LED 点亮，表示固件初始化出错
- 尝试重新进入 Bootloader 模式并重新烧录
- 查看串口输出以获取错误信息

### 问题：无法看到 USB 串口设备

- 检查 USB 连接是否良好
- 在 macOS 上，可能需要安装 CP210x 或 CH340 驱动（取决于板子的 UART 转接芯片）
- 尝试使用 `dmesg` (Linux) 或 `system_profiler SPUSBDataType` (macOS) 检查 USB 设备是否被识别

### 问题：固件运行不正常

- 查看串口输出是否有错误消息
- 检查硬件连接是否正确
- 如有需要，通过命令 `d` 获取详细的诊断信息

## 相关文档

- [Pico SDK 官方文档](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [MODBUS RTU 协议规范](https://modbus.org/specs.php)
- [物理引脚配置详见 IO_PIN_CONFIGURATION_REPORT.md](./IO_PIN_CONFIGURATION_REPORT.md)

