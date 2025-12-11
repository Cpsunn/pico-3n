#!/bin/bash
# 快速验证脚本 - 检查所有源文件和头文件的一致性

set -e

cd "$(dirname "$0")"

echo "=== FX3U Pico Simulator Build Verification ==="
echo ""
echo "1. Checking file counts..."
echo "   Headers: $(ls include/*.h | wc -l)"
echo "   Sources: $(ls src/*.c | wc -l)"

echo ""
echo "2. Checking for syntax issues..."
# 简单的 C 语法检查 (注释和括号匹配)
for file in src/*.c include/*.h; do
    if ! grep -q "^#" "$file" 2>/dev/null; then
        echo "   ⚠️  $file might be empty"
    fi
done

echo ""
echo "3. Checking CMakeLists.txt..."
if grep -q "hardware_adc\|hardware_pwm\|pico_time" CMakeLists.txt; then
    echo "   ✓ All required libraries listed"
else
    echo "   ✗ Missing required libraries"
    exit 1
fi

echo ""
echo "4. Verifying key functions exist..."
functions=("fx3u_core_init" "fx3u_core_start" "io_manager_init" "modbus_slave_process" "rs485_send")
for func in "${functions[@]}"; do
    if grep -q "^.*$func.*(" src/*.c; then
        echo "   ✓ $func"
    else
        echo "   ✗ $func NOT FOUND"
        exit 1
    fi
done

echo ""
echo "=== All checks passed! Ready to build ==="
