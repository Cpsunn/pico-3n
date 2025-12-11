cmake_minimum_required(VERSION 3.13)

# 拉取Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_fx3u_simulator C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

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

target_include_directories(pico_fx3u_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_link_libraries(pico_fx3u_simulator
    pico_stdlib
    pico_runtime
    hardware_uart
    hardware_gpio
    hardware_timer
    hardware_dma
    hardware_spi
    hardware_i2c
)

pico_enable_stdio_usb(pico_fx3u_simulator 1)
pico_enable_stdio_uart(pico_fx3u_simulator 0)

pico_add_extra_outputs(pico_fx3u_simulator)
