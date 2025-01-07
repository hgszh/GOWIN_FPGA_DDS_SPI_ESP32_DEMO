#include "pga113.h"
#include "system_misc_task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static spi_device_handle_t pga_spi;
static pga_gain_t gain = GAIN1;
static pga_channel_t channel = CH1;

void init_pga113(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = PGA_DIO,
        .sclk_io_num = PGA_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 8,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_device_interface_config_t devcfg = {

        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PGA_CS,
        .queue_size = 3,
    };
    spi_bus_add_device(SPI2_HOST, &devcfg, &pga_spi);
}

static void write_pga113(spi_device_handle_t spi, const uint16_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = &data;
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK)
    {
        ESP_LOGI("PGA113", "SPI transmit failed: %s", esp_err_to_name(ret));
    }
}

void set_pga113_gain(pga_gain_t new_gain)
{
    gain = new_gain;
    uint16_t data = 0x2A00 | ((gain << 4) | channel);
    data = SPI_SWAP_DATA_TX(data, 16);
    write_pga113(pga_spi, data);
}

void set_pga113_channel(pga_channel_t new_channel)
{
    channel = new_channel;
    uint16_t data = 0x2A00 | ((gain << 4) | channel);
    data = SPI_SWAP_DATA_TX(data, 16);
    write_pga113(pga_spi, data);
}