#include "joystick.h"
#include <stdio.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"

void setup_joystick(){
  adc_init();         
  adc_gpio_init(VRX); 
  adc_gpio_init(VRY);

  gpio_init(SW);             
  gpio_set_dir(SW, GPIO_IN); 
  gpio_pull_up(SW);         
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value){
  adc_select_input(ADC_CHANNEL_0); 
  sleep_us(2);                     
  *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

  adc_select_input(ADC_CHANNEL_1); 
  sleep_us(2);                     
  *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}

int get_direction(uint16_t vry_value, uint16_t vrx_value){
    if(vry_value < 150){
        return 1;
    }
    if(vry_value > 3900){
        return 2;
    }
    if(vrx_value < 150){
        return 3;
    }
    if(vrx_value > 3900){
        return 4;
    }
    return 5;
}
