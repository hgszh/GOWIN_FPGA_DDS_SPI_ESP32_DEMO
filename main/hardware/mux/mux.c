#include "mux.h"
#include "system_misc_task.h"
#include "driver/gpio.h"

void set_measurement_range(uint8_t range)
{
    switch (range)
    {
    case 0:
        gpio_set_level(SW_D0, 0);
        gpio_set_level(SW_D1, 0);
        gpio_set_level(SW_RELAY, 1);
        break;
    case 1:
        gpio_set_level(SW_D0, 1);
        gpio_set_level(SW_D1, 0);
        gpio_set_level(SW_RELAY, 0);
        break;
    case 2:
        gpio_set_level(SW_D0, 0);
        gpio_set_level(SW_D1, 1);
        gpio_set_level(SW_RELAY, 0);
        break;
    case 3:
        gpio_set_level(SW_D0, 1);
        gpio_set_level(SW_D1, 1);
        gpio_set_level(SW_RELAY, 0);
        break;
    default:
        gpio_set_level(SW_D0, 0);
        gpio_set_level(SW_D1, 0);
        gpio_set_level(SW_RELAY, 0);
        break;
    }
}

void en_voltage_measurement(void)
{
    gpio_set_level(SW_VOLTAGE_CURRENT, 1);
}

void en_current_measurement(void)
{
    gpio_set_level(SW_VOLTAGE_CURRENT, 0);
}