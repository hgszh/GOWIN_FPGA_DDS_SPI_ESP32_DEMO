#include "mcu_to_fpga_spi.h"
#include "crc.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static spi_device_handle_t spi_master;
void mcu_fpga_spi_init(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = fpga2mcu_miso,
        .mosi_io_num = fpga2mcu_mosi,
        .sclk_io_num = fpga2mcu_sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 6,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED);
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = fpga2mcu_cs,
        .queue_size = 1,
    };
    gpio_pulldown_en(fpga2mcu_cs);
    gpio_pulldown_en(fpga2mcu_mosi);
    gpio_pulldown_en(fpga2mcu_sclk);
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi_master);
}

bool spi_write_read(uint8_t *tx_buffer, uint8_t *rx_buffer, size_t bytes)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = bytes * 8;
    t.tx_buffer = tx_buffer;
    t.rx_buffer = rx_buffer;
    ret = spi_device_polling_transmit(spi_master, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGI("SPI", "SPI transmit failed: %s", esp_err_to_name(ret));
        return false;
    }
    return true;
}

bool send_command_to_fpga(spi_command_t command, uint32_t data)
{
    spi_packet_t packet;
    packet.command = command;
    packet.data = __builtin_bswap32(data); // 转为大端字节序
    // 计算data的CRC值
    uint8_t data_crc = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        data_crc = crc(data_crc, packet.raw[i + 1]);
    }
    packet.crc = data_crc;
    // 发送命令包
    if (!spi_write_read(packet.raw, NULL, sizeof(packet.raw)))
    {
        return false;
    }
    // 等待FPGA处理
    ESP_LOGI("SPI", "Command sent: 0x%02X, Data: 0x%08lX, Data CRC: 0x%02X", packet.command, packet.data, packet.crc);
    // 读取FPGA返回的ACK
    packet.command = CMD_CHECK;
    packet.data = 0;
    packet.crc = 0;
    uint8_t ack[sizeof(packet.raw)] = {0};
    if (!spi_write_read(packet.raw, &ack[0], sizeof(packet.raw)))
    {
        return false;
    }
    // 通过ACK判断通信是否成功
    if (ack[0] != COMM_STATUS_SUCCESS)
    {
        ESP_LOGI("SPI", "FPGA ACK ERR: %d", ack[0]);
        return false;
    }
    return true;
}

/***************************************************************************************************/

bool set_dds_freq(float freq)
{
    uint8_t i = 10;
    while (i > 0)
    {
        i--;
        if (send_command_to_fpga(CMD_DDS_PINC_SET, SET_DDS_FREQ(freq)))
        {
            return true;
        }
        else
        {
            vTaskDelay(100);
        }
    }
    return false;
}

bool set_dds_ampl(uint16_t ampl)
{
    uint8_t i = 10;
    while (i > 0)
    {
        i--;
        if (send_command_to_fpga(CMD_DDS_AMPL_SET, SET_DDS_AMPL(ampl)))
        {
            return true;
        }
        else
        {
            vTaskDelay(100);
        }
    }
    return false;
}
