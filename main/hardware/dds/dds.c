#include "dds.h"

#include "system_misc_task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <math.h>

/**
 * @brief 初始化I2S接口用于DAC7811的配置
 *
 * @param[out] tx_handle 指向I2S传输通道句柄的指针
 * @param[in] bclk 传输的时钟引脚
 * @param[in] ws 传输的帧同步引脚
 * @param[in] dout 数据输出引脚
 */
void init_i2s_for_dac7811(i2s_chan_handle_t *tx_handle, gpio_num_t bclk, gpio_num_t ws, gpio_num_t dout)
{
    i2s_port_t port_num = I2S_NUM_1;
    i2s_chan_config_t chan_cfg =
        {
            .id = port_num,
            .role = I2S_ROLE_MASTER,
            .dma_desc_num = 3,
            .dma_frame_num = 1000,
            .auto_clear = false,
        };
    i2s_new_channel(&chan_cfg, tx_handle, NULL);

    i2s_std_config_t std_cfg =
        {
            .clk_cfg =
                {
                    .sample_rate_hz = 600000,
                    .clk_src = I2S_CLK_SRC_DEFAULT,
                    .mclk_multiple = I2S_MCLK_MULTIPLE_128},
            .slot_cfg =
                {
                    .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
                    .slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT,
                    .slot_mode = I2S_SLOT_MODE_MONO,
                    .slot_mask = I2S_STD_SLOT_LEFT,
                    .ws_width = 16,
                    .ws_pol = false,
                    .bit_shift = false,
                    .left_align = true,
                    .big_endian = false,
                    .bit_order_lsb = false},
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = bclk,
                .ws = ws,
                .dout = dout,
                .din = I2S_GPIO_UNUSED,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = true,
                    .ws_inv = false,
                },
            },
        };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*tx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*tx_handle));
}

/**
 * @brief 初始化I2S接口用于DAC8830的配置
 *
 * @param[out] tx_handle 指向I2S传输通道句柄的指针
 * @param[in] bclk 传输的时钟引脚
 * @param[in] ws 传输的帧同步引脚
 * @param[in] dout 数据输出引脚
 */
void init_i2s_for_dac8830(i2s_chan_handle_t *tx_handle, gpio_num_t bclk, gpio_num_t ws, gpio_num_t dout)
{
    i2s_port_t port_num = I2S_NUM_0;
    i2s_chan_config_t chan_cfg =
        {
            .id = port_num,
            .role = I2S_ROLE_MASTER,
            .dma_desc_num = 3,
            .dma_frame_num = 1000,
            .auto_clear = false,
        };
    i2s_new_channel(&chan_cfg, tx_handle, NULL);

    i2s_std_config_t std_cfg =
        {
            .clk_cfg =
                {
                    .sample_rate_hz = 600000,
                    .clk_src = I2S_CLK_SRC_DEFAULT,
                    .mclk_multiple = I2S_MCLK_MULTIPLE_128},
            .slot_cfg =
                {
                    .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
                    .slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT,
                    .slot_mode = I2S_SLOT_MODE_MONO,
                    .slot_mask = I2S_STD_SLOT_RIGHT,
                    .ws_width = 1,
                    .ws_pol = true,
                    .bit_shift = false,
                    .left_align = true,
                    .big_endian = false,
                    .bit_order_lsb = false},
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = bclk,
                .ws = ws,
                .dout = dout,
                .din = I2S_GPIO_UNUSED,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false,
                },
            },
        };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*tx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*tx_handle));
}

/**
 * @brief 根据给定的频率设置I2S采样频率
 *
 * @param[in] tx_handle I2S传输通道句柄
 * @param[in] freq_step 频率步进索引，范围为0到7
 * @return 返回一个周期内的采样点数
 */
uint16_t set_i2s_sampling_for_freq(i2s_chan_handle_t tx_handle, uint8_t freq_step)
{
    const uint32_t freq_values[8] = {60, 100, 600, 1000, 6000, 10000, 60000, 100000};
    i2s_channel_disable(tx_handle);
    uint32_t freq = 0;
    uint16_t samples_per_cycle = 0;

    if (freq_step < 8)
    {
        freq = freq_values[freq_step];
    }

    if (freq < 600)
    {
        // 频率低的时候周期长，降低采样频率来缩短正弦表的长度、节约内存
        i2s_std_clk_config_t clk_cfg = {
            .sample_rate_hz = 120000,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_128};
        i2s_channel_reconfig_std_clock(tx_handle, &clk_cfg);
        // 计算一个周期的点数
        samples_per_cycle = 120000 / freq;
    }
    else
    {
        i2s_std_clk_config_t clk_cfg = {
            .sample_rate_hz = 600000,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_128};
        i2s_channel_reconfig_std_clock(tx_handle, &clk_cfg);
        // 计算一个周期的点数
        samples_per_cycle = 600000 / freq;
    }
    i2s_channel_enable(tx_handle);

    return samples_per_cycle;
}

/**
 * @brief 生成正弦波表
 *
 * @param[in] samples_per_cycle 一个周期内的采样点数
 * @param[out] sine_table 存储生成的正弦波数据的数组
 * @param[in] amplitude 正弦波的幅值
 * @param[in] phase_step 相位步进，控制相位偏移
 * @param[in] phase_cal_factor 相位校正因子
 * @return 返回生成的正弦波表的实际长度
 */
uint16_t generate_sine_table(uint16_t samples_per_cycle, uint16_t *sine_table, uint16_t amplitude, uint8_t phase_step, float phase_cal_factor)
{
    uint16_t complete_cycles = 0;
    uint16_t actual_table_length = 0;

    if (phase_step < 4)
    {
        // 计算正弦表可以容纳多少个整周期，避免截断
        complete_cycles = SINE_TABLE_LEN / samples_per_cycle;
        // 计算得到正弦表实际可用长度
        actual_table_length = complete_cycles * samples_per_cycle;

        for (uint16_t i = 0; i < SINE_TABLE_LEN; i++)
        {
            sine_table[i] = (uint16_t)(0.5 * amplitude * (1.0 + sinf((float)2.0 * M_PI / samples_per_cycle * i + phase_step * M_PI_2 + phase_cal_factor)));
        }

        return actual_table_length;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief 将数据转换为DAC7811格式
 *
 * @param[in,out] inputArray 输入数据数组，将被转换为DAC7811格式
 * @param[in] size 数组的元素数量
 */
void convert_to_dac7811_format(uint16_t *inputArray, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        // bit12=1
        inputArray[i] |= 0x1000;
        // bit15、14、13=0
        inputArray[i] &= 0x1FFF;
    }
}

i2s_chan_handle_t dac8830_handle = NULL;
i2s_chan_handle_t dac7811_handle = NULL;
uint16_t sine_table[SINE_TABLE_LEN] = {0};
uint16_t mdac_sine_table_ref0[SINE_TABLE_LEN] = {0};
uint16_t mdac_sine_table_ref90[SINE_TABLE_LEN] = {0};
uint16_t length_sine, length_sine_ref0, length_sine_ref90;

void dac_task(void *arg)
{
    init_i2s_for_dac8830(&dac8830_handle, DAC_SCLK, DAC_CS, DAC_SDI);
    init_i2s_for_dac7811(&dac7811_handle, MDAC_SCLK, MDAC_SYNC, MDAC_SDIN);

    // 60Hz
    // uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 0);
    // uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 0);
    //  length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0);
    // length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 0.025);
    // length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 0.025);

    /*
       // 100Hz
       uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 1);
       uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 1);
       length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0);
       length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 0.039);
       length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 0.039);
    */

    // 600Hz
    //  uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 2);
    //   uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 2);
    //   length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0);
    //   length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 0.242);
    //  length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 0.242);

    // 1KHz
    //  uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 3);
    //  uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 3);
    //  length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0);
    //  length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 0.417);
    // length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 0.417);

    /*
        // 6KHz
        uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 4);
        uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 4);
        length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0);
        length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 2.499);
        length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 2.499);
    */

    // 10k
    //  uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 5);
    //   uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 5);
    //  length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0);
    //  length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, -2.11);
    //  length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, -2.11);

    /*
       // 60k
       uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 6);
       uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 6);
       length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 0.18);
       length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 0);
       length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 0);
    */

    // 100k
    uint16_t dac8830_samples_per_cycle = set_i2s_sampling_for_freq(dac8830_handle, 7);
    uint16_t dac7811_samples_per_cycle = set_i2s_sampling_for_freq(dac7811_handle, 7);
    length_sine = generate_sine_table(dac8830_samples_per_cycle, sine_table, 65535, 0, 1.6);
    length_sine_ref0 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref0, 4095, 0, 0);
    length_sine_ref90 = generate_sine_table(dac7811_samples_per_cycle, mdac_sine_table_ref90, 4095, 1, 0);

    convert_to_dac7811_format(mdac_sine_table_ref0, length_sine_ref0);
    convert_to_dac7811_format(mdac_sine_table_ref90, length_sine_ref90);

    while (1)
    {
        i2s_channel_write(dac8830_handle, sine_table, length_sine * 2, NULL, 10);
        // i2s_channel_write(dac7811_handle, mdac_sine_table_ref90, length_sine_ref90 * 2, NULL, 10);
        i2s_channel_write(dac7811_handle, mdac_sine_table_ref0, length_sine_ref0 * 2, NULL, 10);
    }
}
