cmake_minimum_required(VERSION 3.13)

# 确保设置了 PICO_SDK_PATH
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set")
endif()

# 拉取Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_fx3u_simulator C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 初始化 Pico SDK
pico_sdk_init()

# 定义可执行文件
add_executable(pico_fx3u_simulator
    src/main.c
    src/fx3u_core.c
    src/fx3u_instructions.c
    src/fx3u_io.c
    src/communication.c
    src/modbus_protocol.c
    src/rs485_driver.c
    src/ethernet_adapter.c
    src/memory_manager.c
    src/timer.c
)

# 包含目录
target_include_directories(pico_fx3u_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接库
target_link_libraries(pico_fx3u_simulator
    pico_stdlib
    pico_time
    hardware_uart
    hardware_gpio
    hardware_timer
    hardware_adc
    hardware_pwm
)

# 启用 USB 和 UART stdio（但注意冲突）
pico_enable_stdio_usb(pico_fx3u_simulator 1)
pico_enable_stdio_uart(pico_fx3u_simulator 0)

# 生成额外的文件格式（.uf2）
pico_add_extra_outputs(pico_fx3u_simulator)
