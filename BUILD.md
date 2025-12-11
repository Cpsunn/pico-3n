# 构建和部署指南

## 环境设置

### Linux (Ubuntu 20.04/22.04)

```bash
# 1. 安装依赖
sudo apt update
sudo apt install -y \
    cmake \
    git \
    build-essential \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib

# 2. 克隆Pico SDK
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# 3. 设置环境变量
export PICO_SDK_PATH=~/pico-sdk
echo 'export PICO_SDK_PATH=~/pico-sdk' >> ~/.bashrc
```

### macOS (M1/M2/Intel)

```bash
# 1. 使用Homebrew安装工具链
brew install cmake arm-none-eabi-gcc

# 2. 克隆Pico SDK
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# 3. 设置环境变量
export PICO_SDK_PATH=~/pico-sdk
echo 'export PICO_SDK_PATH=~/pico-sdk' >> ~/.zprofile
```

### Windows (MSYS2)

```bash
# 1. 安装MSYS2
# https://www.msys2.org/

# 2. 在MSYS2终端中安装工具
pacman -S git cmake arm-none-eabi-gcc arm-none-eabi-gdb

# 3. 克隆Pico SDK
cd ~
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# 4. 设置环境变量 (在MSYS2中)
export PICO_SDK_PATH=~/pico-sdk
```

## 构建步骤

### 1. 进入项目目录

```bash
cd /path/to/pico-fx3u-simulator
```

### 2. 创建构建目录

```bash
mkdir -p build
cd build
```

### 3. 配置项目

```bash
cmake ..
```

如需自定义构建选项，可使用:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DPICO_BOARD=pico \
      ..
```

### 4. 编译项目

```bash
# 使用多核加速编译
make -j$(nproc)

# 或者简单的make
make
```

### 5. 检查构建输出

```bash
ls -lah pico_fx3u_simulator*
```

应该看到:
- `pico_fx3u_simulator.elf` - 可执行文件
- `pico_fx3u_simulator.uf2` - Pico固件
- `pico_fx3u_simulator.hex` - Intel HEX格式
- `pico_fx3u_simulator.bin` - 二进制文件

## 烧录固件

### 方法1: UF2格式 (推荐)

```bash
# macOS
cp build/pico_fx3u_simulator.uf2 /Volumes/RPI-RP2/

# Linux
cp build/pico_fx3u_simulator.uf2 /media/$USER/RPI-RP2/

# Windows (在PowerShell或cmd中)
copy build\pico_fx3u_simulator.uf2 D:\
```

### 方法2: 使用picotool

```bash
# 安装picotool
sudo apt install picotool  # Linux
brew install picotool      # macOS

# 烧录
picotool load build/pico_fx3u_simulator.elf

# 验证
picotool info
```

### 方法3: 使用OpenOCD (调试器)

```bash
# 安装OpenOCD
sudo apt install openocd   # Linux
brew install openocd       # macOS

# 启动调试服务器
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg

# 在另一个终端加载固件
telnet localhost 4444
> program build/pico_fx3u_simulator.elf verify reset
> exit
```

## 验证烧录

### 检查固件是否正确烧录

```bash
# 使用picotool查看信息
picotool info

# 应该显示程序大小和入口点
```

### 通过USB串口测试

```bash
# Linux/macOS
minicom -D /dev/ttyACM0 -b 115200

# 或使用screen
screen /dev/ttyACM0 115200

# Windows (使用PuTTY或其他串口工具)
```

应该看到启动信息:
```
=== Pico FX3U Simulator v1.0 ===
Initializing PLC core...
Initializing I/O manager...
Initializing RS485 communication...
...
System initialization completed.
```

## 调试

### 使用GDB调试

```bash
# 启动OpenOCD
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg &

# 启动GDB
arm-none-eabi-gdb build/pico_fx3u_simulator.elf

# 在GDB中
(gdb) target remote localhost:3333
(gdb) load
(gdb) continue
(gdb) break main
(gdb) ...
```

### 启用调试输出

编辑 `CMakeLists.txt`:

```cmake
add_definitions(-DDEBUG=1)

# 或者编译时指定
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### 查看构建大小

```bash
arm-none-eabi-size build/pico_fx3u_simulator.elf

# 输出类似:
#    text    data     bss     dec     hex filename
#   98765    1234    5678  105677   19d4d pico_fx3u_simulator.elf
```

## 优化构建

### 减小固件大小

```bash
# 使用Release模式
cmake -DCMAKE_BUILD_TYPE=Release ..

# 启用链接时优化
cmake -DCMAKE_CXX_FLAGS_RELEASE="-O3 -flto" ..

# 剥离调试符号
arm-none-eabi-strip build/pico_fx3u_simulator.elf
```

### 加快编译速度

```bash
# 使用ccache加速
cmake -DCMAKE_C_COMPILER_LAUNCHER=ccache ..

# 使用更多并行线程
make -j8
```

## 清理和重建

### 完全清理

```bash
cd /path/to/pico-fx3u-simulator
rm -rf build
mkdir build
cd build
cmake ..
make
```

### 增量编译

```bash
cd build
cmake ..
make
```

## CI/CD 构建脚本

### GitHub Actions

创建文件 `.github/workflows/build.yml`:

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
      with:
        submodules: true
    
    - name: Install dependencies
      run: |
        sudo apt update
        sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi
    
    - name: Clone Pico SDK
      run: |
        git clone https://github.com/raspberrypi/pico-sdk.git
        cd pico-sdk
        git submodule update --init
    
    - name: Build
      env:
        PICO_SDK_PATH: ${{ github.workspace }}/pico-sdk
      run: |
        mkdir build && cd build
        cmake ..
        make -j$(nproc)
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v2
      with:
        name: pico_fx3u_simulator
        path: build/pico_fx3u_simulator.uf2
```

### Makefile 包装

创建 `Makefile`:

```makefile
PICO_SDK_PATH ?= ~/pico-sdk
BUILD_DIR := build
TARGET := pico_fx3u_simulator

.PHONY: all clean build flash
export PICO_SDK_PATH

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make -j$(shell nproc)

clean:
	rm -rf $(BUILD_DIR)

flash: build
	cp $(BUILD_DIR)/$(TARGET).uf2 /Volumes/RPI-RP2/ || \
	cp $(BUILD_DIR)/$(TARGET).uf2 /media/$(USER)/RPI-RP2/

size: build
	arm-none-eabi-size $(BUILD_DIR)/$(TARGET).elf

help:
	@echo "Available targets:"
	@echo "  make build     - Build the project"
	@echo "  make clean     - Clean build files"
	@echo "  make flash     - Flash to Pico"
	@echo "  make size      - Show binary size"
```

然后使用:
```bash
make build
make flash
make size
```

## 版本管理

### 标记发行版

```bash
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

## 常见问题

### Q: 编译找不到Pico SDK
A: 确保设置了 `PICO_SDK_PATH` 环境变量，并且路径正确

### Q: 烧录时出现 "No drive found"
A: 确保按住BOOTSEL后重新连接USB，Pico应显示为USB设备

### Q: 编译失败，提示缺少arm工具链
A: 按照上面的操作系统特定步骤安装 `gcc-arm-none-eabi`

### Q: 如何启用USB调试
A: 在 `CMakeLists.txt` 中添加 `pico_enable_stdio_usb(pico_fx3u_simulator 1)`

---

更多帮助请参考官方文档:
- https://github.com/raspberrypi/pico-sdk
- https://datasheets.raspberrypi.org/pico/getting_started_with_pico.pdf
