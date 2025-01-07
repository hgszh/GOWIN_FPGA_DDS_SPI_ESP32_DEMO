
#ifndef SYS_MISC_H_
#define SYS_MISC_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BAT_AD ADC_CHANNEL_0
#define EN_BAT_AD GPIO_NUM_5
#define BUTTON_AD ADC_CHANNEL_1
#define AD_ATTEN ADC_ATTEN_DB_12

#define PWR_BTN_DET GPIO_NUM_40
#define PWR_KEEP GPIO_NUM_41

#define SW_D0 GPIO_NUM_47
#define SW_D1 GPIO_NUM_21
#define SW_RELAY GPIO_NUM_48
#define SW_VOLTAGE_CURRENT GPIO_NUM_11

#define PGA_CS GPIO_NUM_6
#define PGA_DIO GPIO_NUM_7
#define PGA_SCLK GPIO_NUM_15

#define DAC_CS GPIO_NUM_39
#define DAC_SDI GPIO_NUM_45
#define DAC_SCLK GPIO_NUM_38

#define MDAC_SYNC -1
#define MDAC_SDIN -1
#define MDAC_SCLK -1

typedef struct
{
    volatile uint16_t battery_voltage;
} system_status_t;

extern system_status_t system_status;

extern struct Button btn1;
extern struct Button btn2;
extern struct Button btn3;
extern struct Button btn4;
extern struct Button pwr_btn;

void system_misc_task(void *arg);

#endif