cmake_minimum_required(VERSION 3.13)

# Enforce C11 and C++17 standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure PICO_SDK_PATH is set
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable must be set")
endif()

# Include the Pico SDK tools
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Define the project
project(pico_fx3u_simulator C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Define the target executable
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

# Add include directories
target_include_directories(pico_fx3u_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link required libraries
target_link_libraries(pico_fx3u_simulator
    pico_stdlib
    pico_time
    hardware_uart
    hardware_gpio
    hardware_timer
    hardware_adc
    hardware_pwm
)

# Configure stdio (USB output for printf)
pico_enable_stdio_usb(pico_fx3u_simulator 1)
pico_enable_stdio_uart(pico_fx3u_simulator 0)

# Add extra output targets (.uf2 file)
pico_add_extra_outputs(pico_fx3u_simulator)
