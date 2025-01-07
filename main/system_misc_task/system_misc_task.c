#include "system_misc_task.h"
#include "multi_button.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

system_status_t system_status;

// 单片机ADC校准
static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t button_ad_cali_handle;
static adc_cali_handle_t bat_ad_cali_handle;
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
// 电池电压监测任务
static void battery_monitering_task(void *arg);
// ADC按键
enum Button_IDs
{
    btn1_id,
    btn2_id,
    btn3_id,
    btn4_id,
    pwr_btn_id,
};
struct Button btn1;
struct Button btn2;
struct Button btn3;
struct Button btn4;
struct Button pwr_btn;
static uint8_t read_button(uint8_t button_id);
static void button_task(void *arg);

void system_misc_task(void *arg)
{
    // 开机，输出高电平维持电源
    gpio_reset_pin(PWR_KEEP);
    gpio_set_direction(PWR_KEEP, GPIO_MODE_OUTPUT);
    gpio_set_level(PWR_KEEP, 1);

    // 初始化单片机ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = AD_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BAT_AD, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BUTTON_AD, &config));
    adc_calibration_init(ADC_UNIT_1, BUTTON_AD, AD_ATTEN, &button_ad_cali_handle);
    adc_calibration_init(ADC_UNIT_1, BAT_AD, AD_ATTEN, &bat_ad_cali_handle);

    xTaskCreate(battery_monitering_task, "battery moniter", 4096, NULL, 2, NULL);
    xTaskCreate(button_task, "button", 4096, NULL, 2, NULL);

    // 初始化电源键为上拉输入
    gpio_reset_pin(PWR_BTN_DET);
    gpio_set_direction(PWR_BTN_DET, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PWR_BTN_DET, GPIO_PULLUP_ONLY);  

    while (1)
    {
        vTaskDelay(1);
    }  
}

static void double_click_pwr_off(void *btn)
{
    // 如果双击PWR键，则断电关机
    if (get_button_event(&pwr_btn) == DOUBLE_CLICK)
    {
        gpio_set_level(PWR_KEEP, 0);
    }
}

static void button_task(void *arg)
{
    button_init(&btn1, read_button, 1, btn1_id);
    button_init(&btn2, read_button, 1, btn2_id);
    button_init(&btn3, read_button, 1, btn3_id);
    button_init(&btn4, read_button, 1, btn4_id);
    button_init(&pwr_btn, read_button, 0, pwr_btn_id);

    button_start(&btn1);
    button_start(&btn2);
    button_start(&btn3);
    button_start(&btn4);
    button_start(&pwr_btn);

    button_attach(&pwr_btn, DOUBLE_CLICK, double_click_pwr_off);

    while (1)
    {
        button_ticks();
        vTaskDelay(5);
    }
}

static uint8_t read_button(uint8_t button_id)
{
    int adc_raw = 0;
    int adc_sum = 0;
    int voltage = 0;

    for (int i = 0; i < 32; i++)
    {
        adc_oneshot_read(adc1_handle, BUTTON_AD, &adc_raw);
        adc_sum += adc_raw;
    }
    adc_raw = adc_sum >> 5;                                            // 对32个电压样本取平均
    adc_cali_raw_to_voltage(button_ad_cali_handle, adc_raw, &voltage); // ADC校准，返回mV电压结果

    switch (button_id)
    {
    case btn1_id:
        return (voltage > 150 && voltage < 400);
    case btn2_id:
        return (voltage > 900 && voltage < 1150);
    case btn3_id:
        return (voltage > 1500 && voltage < 1750);
    case btn4_id:
        return (voltage < 50);
    case pwr_btn_id:
        return gpio_get_level(PWR_BTN_DET);
    default:
        return 0;
    }
}

static void battery_monitering_task(void *arg)
{
    gpio_reset_pin(EN_BAT_AD);
    gpio_set_direction(EN_BAT_AD, GPIO_MODE_OUTPUT);
    gpio_set_level(EN_BAT_AD, 0);
    int adc_raw, adc_sum, voltage;

    while (1)
    {
        // 使能导通电池分压电阻
        gpio_set_level(EN_BAT_AD, 1);

        // 50mS等待电压稳定
        vTaskDelay(50);

        // 测量电池电压
        for (int i = 0; i < 256; i++)
        {
            adc_oneshot_read(adc1_handle, BAT_AD, &adc_raw);
            adc_sum += adc_raw;
        }
        adc_raw = adc_sum >> 8;                                         // 对256个电池电压样本取平均
        adc_cali_raw_to_voltage(bat_ad_cali_handle, adc_raw, &voltage); // ADC校准，返回mV电压结果
        voltage = voltage * 2;                                          // 0.5倍分压电阻

        // 断开电池分压电阻
        gpio_set_level(EN_BAT_AD, 0);
        adc_sum = 0;

        // 电池低电压断电关机
        if (voltage < 3000)
        {
            gpio_set_level(PWR_KEEP, 0);
        }

        // 更新数据
        system_status.battery_voltage = voltage;
        ESP_LOGI("BAT", "%d mV", voltage);
        printf("BAT=%d,\r\n", voltage);

        //  开机先快速测量几次电池电压方便显示，随后转为每20S更新一次
        static uint8_t count = 0;
        if (count < 10)
        {
            count++;
            vTaskDelay(100);
        }
        else
        {
            vTaskDelay(20000);
        }
    }
}

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI("ESP ADC", "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI("ESP ADC", "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI("ESP ADC", "Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW("ESP ADC", "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE("ESP ADC", "Invalid arg or no memory");
    }

    return calibrated;
}