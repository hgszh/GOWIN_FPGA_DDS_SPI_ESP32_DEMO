#include <stdio.h>

#include "system_misc_task.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "mcu_to_fpga_spi.h"

#include "multi_button.h"

void app_main(void)
{
    xTaskCreate(system_misc_task, "system_misc", 1024 * 4, NULL, 3, NULL);


}
