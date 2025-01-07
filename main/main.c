#include <stdio.h>
#include "system_misc_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "multi_button.h"
#include "mcu_to_fpga_spi.h"
#include "esp_log.h"

void app_main(void)
{
    xTaskCreate(system_misc_task, "system_misc", 1024 * 4, NULL, 0, NULL);

    mcu_fpga_spi_init();
    PressEvent event;
    float freq = 50000.0f;
    uint16_t ampl = 4095;
    set_dds_freq(freq);
    set_dds_ampl(ampl);
    bool flag = true;

    while (1)
    {
        if (get_button_event(&btn1) != NONE_PRESS)
        {
            vTaskDelay(10);
            event = get_button_event(&btn1);
            switch (event)
            {
            case SINGLE_CLICK:
                if (flag)
                {
                    if (freq + 1 <= 5000000)
                    {
                        freq++;
                        if (set_dds_freq(freq))
                        {
                            int int_freq = (int)freq;
                            ESP_LOGI("DDS", "FREQ: %d", int_freq);
                        }
                    }
                }
                else
                {
                    if (ampl + 1 <= 4095)
                    {
                        ampl++;
                        if (set_dds_ampl(ampl))
                        {
                            ESP_LOGI("DDS", "AMPL: %d", ampl);
                        }
                    }
                }
                break;
            case LONG_PRESS_HOLD:
                vTaskDelay(100);
                if (flag)
                {
                    if (freq + 1000 <= 5000000)
                    {
                        freq = freq + 1000;
                        if (set_dds_freq(freq))
                        {
                            int int_freq = (int)freq;
                            ESP_LOGI("DDS", "FREQ: %d", int_freq);
                        }
                    }
                }
                else
                {
                    if (ampl + 10 <= 4095)
                    {
                        ampl = ampl + 100;
                        if (set_dds_ampl(ampl))
                        {
                            ESP_LOGI("DDS", "AMPL: %d", ampl);
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
        else if (get_button_event(&btn2) != NONE_PRESS)
        {
            vTaskDelay(10);
            event = get_button_event(&btn2);
            switch (event)
            {
            case SINGLE_CLICK:
                if (flag)
                {
                    if (freq - 1 >= 0)
                    {
                        freq--;
                        if (set_dds_freq(freq))
                        {
                            int int_freq = (int)freq;
                            ESP_LOGI("DDS", "FREQ: %d", int_freq);
                        }
                    }
                }
                else
                {
                    if (ampl - 1 >= 0)
                    {
                        ampl--;
                        if (set_dds_ampl(ampl))
                        {
                            ESP_LOGI("DDS", "AMPL: %d", ampl);
                        }
                    }
                }
                break;
            case LONG_PRESS_HOLD:
                vTaskDelay(100);
                if (flag)
                {
                    if (freq - 1000 >= 0)
                    {
                        freq = freq - 1000;
                        if (set_dds_freq(freq))
                        {
                            int int_freq = (int)freq;
                            ESP_LOGI("DDS", "FREQ: %d", int_freq);
                        }
                    }
                }
                else
                {
                    if (ampl - 10 >= 0)
                    {
                        ampl = ampl - 100;
                        if (set_dds_ampl(ampl))
                        {
                            ESP_LOGI("DDS", "AMPL: %d", ampl);
                        }
                    }
                }
            default:
                break;
            }
        }
        else if (get_button_event(&btn3) != NONE_PRESS)
        {
            vTaskDelay(10);
            event = get_button_event(&btn3);
            switch (event)
            {
            case SINGLE_CLICK:
                if (flag)
                {
                    flag = false;
                    ESP_LOGI("DDS", "SET: AMPL");
                }
                else
                {
                    flag = true;
                    ESP_LOGI("DDS", "SET: FREQ");
                }
                break;
            default:
                break;
            }
        }
        else
        {
            vTaskDelay(10);
        }
    }
}
