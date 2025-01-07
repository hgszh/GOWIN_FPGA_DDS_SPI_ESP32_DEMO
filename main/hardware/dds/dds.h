
#ifndef DDS_H_
#define DDS_H_

#include <stdint.h>

typedef enum
{
    freq_40,
    freq_100,
    freq_200,
    freq_500,
    freq_1k,
    freq_2k,
    freq_5k,
    freq_10k,
    freq_20k,
    freq_50k,
    freq_100k
} dds_freq_t;

#define SINE_TABLE_LEN 2000

void dac_task(void *arg);

#endif