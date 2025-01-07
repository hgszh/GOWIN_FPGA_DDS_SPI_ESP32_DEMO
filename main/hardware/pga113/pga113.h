
#ifndef PGA113_H_
#define PGA113_H_

#include <stdint.h>

typedef enum
{
    GAIN1 = 0,
    GAIN2,
    GAIN5,
    GAIN10,
    GAIN20,
    GAIN50,
    GAIN100,
    GAIN200
} pga_gain_t;

typedef enum
{
    CH0_VCAL = 0,
    CH1,
    CAL1_GND = 12,
    CAL2_VCAL_0_9,
    CAL3_VCAL_0_1,
    CAL4_VREF
} pga_channel_t;

void init_pga113(void);
void set_pga113_gain(pga_gain_t new_gain);
void set_pga113_channel(pga_channel_t new_channel);

#endif