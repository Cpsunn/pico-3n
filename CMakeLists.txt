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
    src/fx3u_program.c
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

# Add extra output targets (.uf2 file) - this is the key function
pico_add_extra_outputs(pico_fx3u_simulator)

# Optional: Add custom command to explicitly create the UF2 if needed
# This ensures the UF2 is always generated
add_custom_command(TARGET pico_fx3u_simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Checking for .uf2 file generation..."
    COMMAND ls -la ${CMAKE_CURRENT_BINARY_DIR}/pico_fx3u_simulator.* 2>/dev/null || echo "Binary files:"
    COMMAND find ${CMAKE_CURRENT_BINARY_DIR} -name "*.elf" -o -name "*.uf2" -o -name "*.bin" 2>/dev/null
)
