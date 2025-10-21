#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define VRX 26
#define VRY 27
#define ADC_CHANNEL_0 0
#define ADC_CHANNEL_1 1
#define SW 22

void setup_joystick();
void joystick_read_axis(uint16_t *vry_value, uint16_t *vrx_value);
int get_direction(uint16_t vry_value, uint16_t vrx_value);