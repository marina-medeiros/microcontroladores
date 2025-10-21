#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hardware/adc.h"
#include "joystick.h"
#include "pico/stdlib.h"
#include <stdio.h>

const uint led_pin_red = 12;

void setup(){
    gpio_init(led_pin_red);
    gpio_set_dir(led_pin_red, GPIO_OUT);
    setup_joystick();
}

//LED
void vBlinkTask(){
    for (;;){
        gpio_put(led_pin_red, 1); // LED pin 12 
        vTaskDelay(50);
        gpio_put(led_pin_red, 0);
        vTaskDelay(950);
        //printf("Blinking\n");
    }
}

// SENSOR
void vTemperatureTask(){
    while (true){
        adc_set_temp_sensor_enabled(true);
        adc_select_input(4);
        // converte o valor para abranger 12 bits no máximo
        const float conversionFactor = 3.3f / (1 << 12);
        float adc = (float)adc_read() * conversionFactor;
        float tempC = 27.0f - (adc - 0.706f) / 0.001721f; // Calculo para a temperatura aproximada mostrado no datasheet do RP2040
        printf("Temperatura: %0.2f\n", tempC);
        vTaskDelay(2500);
    }
}

// JOYSTICK
void vJoystickTask(){
    int last_direction = 5;
    uint16_t vry_value, vrx_value;

    while (1){
        joystick_read_axis(&vry_value, &vrx_value);

        int direction = get_direction(vry_value, vrx_value);

        if(direction != last_direction){
            switch (direction) {
            case 4:
                printf("Direita\n");
                vTaskDelay(50);
                break;
            case 3:
                printf("Esquerda\n");
                vTaskDelay(50);
                break;
            case 2:
                printf("Cima\n");
                vTaskDelay(50);
                break;
            case 1:
                printf("Baixo\n");
                vTaskDelay(50);
                break;
            case 5:
                vTaskDelay(50);
                break;
            }
        }

        last_direction = direction;
    }
    
}

int main(){
    stdio_init_all();
    adc_init();
    setup();
    
    xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);
    xTaskCreate(vTemperatureTask, "Temperature Task", 256, NULL, 1, NULL);
    xTaskCreate(vJoystickTask, "Joystick Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();
}
