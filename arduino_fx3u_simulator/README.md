# FX3U Simulator (Arduino Library)

This folder contains an Arduino IDE–ready port of the FX3U PLC simulator. Copy the entire `arduino_fx3u_simulator` directory into your Arduino `libraries/` folder or use it as a PlatformIO library. Key contents:

- `library.properties`: metadata for the Arduino Library Manager.
- `src/`: original FX3U core, IO, MODBUS, RS485, timer, and program files adapted for Arduino builds.
- `examples/FX3USimulator/FX3U_Simulator.ino`: demonstration sketch showing how to initialize the PLC core, load a default ladder program, and interact via USB console or RS485.

## Using the Example
1. Install this library into `~/Documents/Arduino/libraries/FX3U_Simulator`.
2. Open Arduino IDE, select `Examples -> FX3U Simulator -> FX3U_Simulator`.
3. Choose the Raspberry Pi Pico (or other RP2040 board) and upload. The serial monitor at 115200 baud exposes the same commands as the original Pico firmware (`s`, `t`, `d`, `r`, `?`).

## Notes
- The example relies on the Pico SDK headers already included in the library; no external dependencies are needed beyond the board support package.
- RS485/transceiver control pins must be adapted to your Arduino wiring (see `fx3u_io.h`).
- MODBUS functions remain the same as in the CMake build.
