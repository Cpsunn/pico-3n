# Pico FX3U 模拟器 - 优化和扩展指南

## 硬件优化

### 1. 内存优化

#### RAM优化策略

```c
// 在 fx3u_core.h 中定义可调参数
// 默认值为完全配置，可按需减少

#define PLC_MAX_INPUTS      256     // 输入 (32 bytes)
#define PLC_MAX_OUTPUTS     256     // 输出 (32 bytes)
#define PLC_MAX_INTERNALS   2048    // 内部 (256 bytes)
#define PLC_MAX_TIMERS      128     // 定时器 (1536 bytes)
#define PLC_MAX_COUNTERS    128     // 计数器 (1536 bytes)
#define PLC_MAX_REGISTERS   4096    // 数据寄存器 (8192 bytes)
```

**优化示例** - 用于内存受限情况:

```c
// 减少的配置 (~15KB)
#define PLC_MAX_INPUTS      64
#define PLC_MAX_OUTPUTS     64
#define PLC_MAX_INTERNALS   512
#define PLC_MAX_TIMERS      64
#define PLC_MAX_COUNTERS    64
#define PLC_MAX_REGISTERS   1024
```

#### 通信缓冲区优化

```c
// 在 communication.h 中
#define RX_BUFFER_SIZE      256     // 接收缓冲区
#define TX_BUFFER_SIZE      256     // 发送缓冲区
#define RX_QUEUE_SIZE       4       // 接收队列深度

// 优化版本
#define RX_BUFFER_SIZE      128
#define TX_BUFFER_SIZE      128
#define RX_QUEUE_SIZE       2
```

### 2. Flash优化

#### 程序存储优化

```c
// 使用压缩的指令格式
typedef struct {
    uint8_t opcode;          // 1字节
    uint16_t operand;        // 2字节
    // 总计: 3字节 (vs. 8字节标准格式)
} fx3u_instruction_compact_t;
```

#### 代码空间节省

```c
// 使用宏减少代码重复
#define GET_BIT(plc, addr) \
    ((addr) < PLC_MAX_INPUTS ? (plc)->inputs[(addr)] : 0)

#define SET_BIT(plc, addr, val) \
    do { if ((addr) < PLC_MAX_OUTPUTS) (plc)->outputs[(addr)] = (val); } while(0)
```

### 3. CPU优化

#### 扫描周期优化

```c
// 根据应用场景调整扫描周期
// 更长的周期 = 更低的CPU占用

#define PLC_SCAN_PERIOD_MS  500     // 标准: 200ms, 低功耗: 500ms

// 在 main.c 中
timer_cfg.period_us = PLC_SCAN_PERIOD_MS * 1000;
```

#### 指令优化

```c
// 使用内联函数加速关键路径
static inline uint8_t fx3u_get_bit_fast(fx3u_core_t *plc, uint16_t addr)
{
    return plc->internals[addr & 0x0FFF];  // 快速位掩码
}
```

## 性能调优

### 1. 缓存优化

```c
// 在 fx3u_core.c 中添加缓存
static struct {
    uint16_t last_timer_check;
    uint16_t last_counter_check;
    uint32_t cycle_time_us;
} g_perf_cache;

// 只在需要时更新定时器
if (!(plc->cycle_count & 0x0F)) {  // 每16个周期更新一次
    // 更新定时器
}
```

### 2. 并行处理

```c
// 在 main.c 中使用多核处理 (如果可用)
// Pico有双核，可利用第二核处理通信

void core1_entry(void) {
    while (1) {
        process_communication();
    }
}

int main(void) {
    multicore_launch_core1(core1_entry);
    // 主核运行PLC
}
```

## 功能扩展

### 1. 添加浮点运算支持

```c
// 新增文件: src/fx3u_float.c
typedef struct {
    float value;
    bool is_valid;
} fx3u_float_t;

#define PLC_MAX_FLOAT_REGS  256

typedef struct {
    fx3u_core_t base;
    fx3u_float_t float_registers[PLC_MAX_FLOAT_REGS];
} fx3u_core_extended_t;

inst_result_t fx3u_float_add(fx3u_core_extended_t *plc, uint16_t addr1, 
                             uint16_t addr2, uint16_t result_addr);
// ... 更多浮点操作
```

### 2. 高级定时器支持

```c
// 添加日志和统计功能
typedef struct {
    uint32_t start_time;
    uint32_t stop_time;
    uint32_t elapsed_us;
    uint32_t min_time_us;
    uint32_t max_time_us;
    uint32_t total_runs;
} timer_stats_t;

void fx3u_timer_get_stats(fx3u_core_t *plc, uint16_t timer_num, 
                          timer_stats_t *stats);
```

### 3. 程序下载支持

```c
// 新增: src/program_loader.c
typedef struct {
    uint32_t program_size;
    uint32_t checksum;
    uint8_t version[4];
    uint32_t timestamp;
} program_header_t;

int program_load_from_flash(fx3u_core_t *plc, uint32_t flash_offset);
int program_save_to_flash(fx3u_core_t *plc, uint32_t flash_offset);
```

### 4. 以太网 (W5500) 集成

```c
// 完整实现 src/w5500_driver.c
#include "w5500.h"

void w5500_init(void);
int w5500_socket_create(uint8_t socket, uint8_t mode);
int w5500_send(uint8_t socket, uint8_t *buffer, uint16_t len);
int w5500_receive(uint8_t socket, uint8_t *buffer, uint16_t max_len);

// MODBUS TCP 支持
void modbus_tcp_init(void);
int modbus_tcp_process(fx3u_core_t *plc);
```

### 5. 实时时钟 (RTC) 支持

```c
// 新增: src/rtc_support.c
typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} rtc_time_t;

void rtc_init(void);
rtc_time_t rtc_get_time(void);
void rtc_set_time(rtc_time_t time);
```

## 调试和诊断工具

### 1. 性能监测

```c
// 添加到 main.c
#define ENABLE_PERF_MONITOR 1

typedef struct {
    uint32_t total_cycles;
    uint32_t comm_cycles;
    uint32_t plc_cycles;
    uint32_t avg_cycle_time_us;
    uint32_t peak_cycle_time_us;
} perf_stats_t;

perf_stats_t g_perf_stats;

void dump_perf_stats(void) {
    printf("Performance Statistics:\r\n");
    printf("Total cycles: %lu\r\n", g_perf_stats.total_cycles);
    printf("Avg cycle time: %lu us\r\n", g_perf_stats.avg_cycle_time_us);
    printf("Peak cycle time: %lu us\r\n", g_perf_stats.peak_cycle_time_us);
}
```

### 2. 内存诊断

```c
// 在 memory_manager.c 中
void memory_dump_heap_stats(void) {
    extern char __heap_start, __heap_end;
    
    printf("Heap statistics:\r\n");
    printf("Heap start: 0x%x\r\n", (uintptr_t)&__heap_start);
    printf("Heap end: 0x%x\r\n", (uintptr_t)&__heap_end);
    printf("Heap size: %u bytes\r\n", 
           (uintptr_t)&__heap_end - (uintptr_t)&__heap_start);
}
```

### 3. 通信调试

```c
// 在 communication.c 中
#define ENABLE_COMM_DEBUG 1

#if ENABLE_COMM_DEBUG
void dump_hex_buffer(uint8_t *buffer, uint16_t len) {
    for (int i = 0; i < len; i++) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\r\n");
    }
    printf("\r\n");
}

void log_communication(const char *direction, uint8_t *buffer, uint16_t len) {
    printf("[%s] %d bytes: ", direction, len);
    dump_hex_buffer(buffer, len);
}
#endif
```

## 与三菱编程软件的互操作性

### 1. 三菱 MC 协议支持

```c
// 新增: src/mitsubishi_protocol.c
typedef struct {
    uint8_t frame_header;   // 0x50 (P)
    uint8_t frame_number;
    uint16_t length;
    uint8_t cpu_code[2];
    uint8_t command;
    uint8_t sub_command;
    // ... 更多字段
} mc_frame_t;

int mc_protocol_parse_frame(uint8_t *buffer, uint16_t len, mc_frame_t *frame);
int mc_protocol_process(fx3u_core_t *plc, mc_frame_t *frame);
```

### 2. GX Works 兼容性注意

```
三菱 GX Works 使用 MC 协议，而非 MODBUS。
本模拟器支持 MODBUS，建议使用:

1. MODBUS 转换工具 (将 MC 协议转为 MODBUS)
2. VB.NET 中间件 (作为协议桥接)
3. 第三方 HMI 软件支持 MODBUS

或者自行实现 MC 协议支持。
```

## 部署清单

### 生产环境部署前检查

- [ ] 完整的硬件测试
- [ ] RS485 通信稳定性测试
- [ ] PLC 指令执行正确性验证
- [ ] 内存和CPU使用率评估
- [ ] 长时间运行稳定性测试
- [ ] 故障恢复机制测试
- [ ] 安全认证 (如需要)
- [ ] 备份和恢复程序测试

### 性能基准测试

```bash
# 编译优化版本
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# 运行基准测试脚本
python3 benchmark.py

# 结果应该输出:
# - 平均扫描周期时间
# - 内存占用
# - CPU占用率
# - 通信吞吐量
```

## 升级路线图

### v1.0 (当前)
- ✅ 基础PLC功能
- ✅ MODBUS 支持
- ✅ GPIO I/O

### v1.1 (下个版本)
- ⏳ 浮点运算
- ⏳ 程序下载功能
- ⏳ 改进的调试工具

### v1.2
- ⏳ 以太网 (W5500)
- ⏳ 实时时钟
- ⏳ 数据表支持

### v2.0
- ⏳ 多核支持
- ⏳ 高级指令集
- ⏳ 三菱 MC 协议

## 故障排除

### 常见问题

1. **内存溢出**
   - 减少 PLC_MAX_* 常数
   - 启用内存诊断工具
   - 优化程序大小

2. **扫描周期不稳定**
   - 增加扫描周期
   - 禁用非必要的功能
   - 检查通信竞争条件

3. **通信丢包**
   - 降低波特率
   - 增加缓冲区大小
   - 检查RS485接线

## 参考资源

- [Pico进阶编程](https://datasheets.raspberrypi.org/rp2040/rp2040_datasheet.pdf)
- [C编程最佳实践](https://en.wikibooks.org/wiki/C_Programming)
- [MODBUS规范](http://www.modbus.org/)
- [FX系列PLC手册](https://www.mitsubishielectric.com/)

---

本指南持续更新中。欢迎提交改进建议！
