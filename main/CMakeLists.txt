idf_component_register(SRCS "main.c"
                            "system_misc_task/system_misc_task.c"
                            "hardware/MultiButton/multi_button.c"
                            "hardware/mcu_to_fpga_spi/mcu_to_fpga_spi.c"
                INCLUDE_DIRS "."
                             "system_misc_task"
                             "hardware/MultiButton"
                             "hardware/mcu_to_fpga_spi"
                REQUIRES driver esp_adc
                             )
