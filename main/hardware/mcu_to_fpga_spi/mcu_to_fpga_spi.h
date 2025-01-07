
#ifndef COMM_H_
#define COMM_H_

#include "driver/spi_master.h"

#define fpga2mcu_sclk GPIO_NUM_36
#define fpga2mcu_mosi GPIO_NUM_37
#define fpga2mcu_miso GPIO_NUM_38
#define fpga2mcu_cs GPIO_NUM_39

typedef enum
{
    COMM_STATUS_SUCCESS = 0x55,
    COMM_STATUS_FAIL = 0xAA
} comm_status_t;

// 显式指定枚举类型的底层类型为 uint8_t，确保枚举类型占用 1 字节
typedef enum : uint8_t
{
    CMD_DDS_PINC_SET = 0xAA, // 设置DDS相位增量
    CMD_DDS_AMPL_SET = 0xBA, // 设置DDS幅度
    CMD_CHECK = 0xCC,        // 检查通信是否成功
    CMD_UNKNOWN = 0xFF       // 未知命令
} mcu_spi_command_t;

#pragma pack(1) // 确保联合体的成员紧密排列，不进行内存对齐
typedef union
{
    struct
    {
        mcu_spi_command_t command; // 8位命令
        uint32_t data;             // 32位数据
        uint8_t crc;               // 8位CRC校验
    };
    uint8_t raw[6];
} spi_packet_t;
#pragma pack()

void mcu_fpga_spi_init(void);
bool spi_write_read(uint8_t *tx_buffer, uint8_t *rx_buffer, size_t bytes);
bool send_command_to_fpga(spi_command_t command, uint32_t data);
bool set_dds_freq(float freq);
bool set_dds_ampl(uint16_t ampl);

// 设置DDS频率
//  f(Hz) * 2^32 / 2000000
#define SET_DDS_FREQ(f) ((uint32_t)((f) * 2147.483648f))
// 设置DDS幅度（12bit）
#define SET_DDS_AMPL(ampl) ((uint16_t)((ampl) & 0xFFF0))

#endif