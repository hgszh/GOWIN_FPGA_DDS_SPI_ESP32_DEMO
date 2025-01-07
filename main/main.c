#include <stdio.h>
#include "system_misc_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "multi_button.h"
#include "mcu_to_fpga_spi.h"
#include "esp_log.h"

#define MAX_FREQ 5000000.0f
#define MIN_FREQ 0.0f
#define MAX_AMPL 4095
#define MIN_AMPL 0
#define FREQ_STEP_SINGLE 1.0f
#define FREQ_STEP_LONG 1000.0f
#define AMPL_STEP_SINGLE 1
#define AMPL_STEP_LONG 100
#define DEBOUNCE_DELAY 10    // 消抖延时
#define LONG_PRESS_DELAY 100 // 长按调整间隔

void adjust_dds_value(bool flag, bool increase, bool long_press, float *freq, uint16_t *ampl);

void app_main(void)
{
    xTaskCreate(system_misc_task, "system_misc", 1024 * 4, NULL, 0, NULL);

    mcu_fpga_spi_init();
    PressEvent event;
    float freq = 50000.0f;
    uint16_t ampl = MAX_AMPL;
    set_dds_freq(freq);
    set_dds_ampl(ampl);
    bool flag = true; // true: 调节频率；false: 调节幅度

    while (1)
    {
        // 按键1：增加频率或幅度
        if (get_button_event(&btn1) != NONE_PRESS)
        {
            vTaskDelay(DEBOUNCE_DELAY);
            event = get_button_event(&btn1);
            adjust_dds_value(flag, true, event == LONG_PRESS_HOLD, &freq, &ampl);
        }
        // 按键2：减少频率或幅度
        else if (get_button_event(&btn2) != NONE_PRESS)
        {
            vTaskDelay(DEBOUNCE_DELAY);
            event = get_button_event(&btn2);
            adjust_dds_value(flag, false, event == LONG_PRESS_HOLD, &freq, &ampl);
        }
        // 按键3：切换模式（频率<->幅度）
        else if (get_button_event(&btn3) != NONE_PRESS)
        {
            vTaskDelay(DEBOUNCE_DELAY);
            event = get_button_event(&btn3);
            if (event == SINGLE_CLICK)
            {
                flag = !flag;
                ESP_LOGI("DDS", "SET: %s", flag ? "FREQ" : "AMPL");
            }
        }
        else
        {
            vTaskDelay(DEBOUNCE_DELAY);
        }
    }
}

void adjust_dds_value(bool flag, bool increase, bool long_press, float *freq, uint16_t *ampl)
{
    if (flag) // 调节频率
    {
        float step = long_press ? FREQ_STEP_LONG : FREQ_STEP_SINGLE;
        if (increase)
        {
            if (*freq + step <= MAX_FREQ)
                *freq += step;
        }
        else
        {
            if (*freq - step >= MIN_FREQ)
                *freq -= step;
        }
        if (set_dds_freq(*freq))
        {
            ESP_LOGI("DDS", "FREQ: %d", (int)(*freq));
        }
        if (long_press)
        {
            vTaskDelay(LONG_PRESS_DELAY);
        }
    }
    else // 调节幅度
    {
        uint16_t step = long_press ? AMPL_STEP_LONG : AMPL_STEP_SINGLE;
        if (increase)
        {
            if (*ampl + step <= MAX_AMPL)
                *ampl += step;
        }
        else
        {
            if (*ampl >= step)
                *ampl -= step;
        }
        if (set_dds_ampl(*ampl))
        {
            ESP_LOGI("DDS", "AMPL: %d", *ampl);
        }
        if (long_press)
        {
            vTaskDelay(LONG_PRESS_DELAY);
        }
    }
}
