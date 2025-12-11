cmake_minimum_required(VERSION 3.13)

# Set the project name
project(pico_fx3u_simulator C CXX ASM)

# Set the C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Include Pico SDK (must be done before project() in some cases, but can also be done here)
if(NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable must be set before running cmake")
endif()

message(STATUS "Using PICO_SDK_PATH: $ENV{PICO_SDK_PATH}")

# Import the Pico SDK import file
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Initialize the Pico SDK
pico_sdk_init()

# Add the executable
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

# Add include directories (relative to current source dir)
target_include_directories(pico_fx3u_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(pico_fx3u_simulator
    pico_stdlib       # Core Pico library (includes GPIO, etc.)
    pico_time         # Time library for sleep_ms, get_absolute_time
    hardware_uart     # UART driver for RS485
    hardware_gpio     # GPIO control
    hardware_timer    # Hardware timer support
    hardware_adc      # ADC support for analog inputs
    hardware_pwm      # PWM support for outputs
)

# Configure stdio
# Use USB for stdio output (printf, etc.)
pico_enable_stdio_usb(pico_fx3u_simulator 1)
# Disable UART stdio to avoid conflicts with RS485
pico_enable_stdio_uart(pico_fx3u_simulator 0)

# Generate additional output formats (.uf2)
pico_add_extra_outputs(pico_fx3u_simulator)
