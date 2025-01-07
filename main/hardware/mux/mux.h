
#ifndef MUX_H_
#define MUX_H_

#include <stdint.h>

void set_measurement_range(uint8_t range);
void en_voltage_measurement(void);
void en_current_measurement(void);

#endif