#include <stdio.h>
#include <string.h> 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hardware/adc.h"
#include "include/joystick.h"
#include "pico/stdlib.h"
#include "include/wifi_conn.h"      // Funções personalizadas de conexão WiFi
#include "include/mqtt_comm.h"      // Funções personalizadas para MQTT
#include "include/xor_cipher.h"     // Funções de cifra XOR

const uint led_pin_green = 11;

void setup(){
    gpio_init(led_pin_green);
    gpio_set_dir(led_pin_green, GPIO_OUT);
    setup_joystick();
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
        
        char tempString[16];
        snprintf(tempString, sizeof(tempString), "%.0f", tempC);

        if(mqtt_comm_publish("ha/desafio11/marina.leite/temp", (const uint8_t *)tempString, strlen(tempString))){
            gpio_put(led_pin_green, 1); 
            sleep_ms(1000);
            gpio_put(led_pin_green, 0);
        }
        vTaskDelay(30000 / portTICK_PERIOD_MS); // 30 segundos
    }
}

// JOYSTICK
void vJoystickTask(void *pvParameters){
    int last_direction = 5;
    uint16_t vry_value, vrx_value;

    char direcao_anterior[16] = "";
    char direcao_atual[16] = "";

    while (1){
        joystick_read_axis(&vry_value, &vrx_value);

        int direction = get_direction(vry_value, vrx_value);

        if(direction != last_direction){
            switch (direction) {
            case 4: strcpy(direcao_atual, "Direita"); break;
            case 3: strcpy(direcao_atual, "Esquerda"); break;
            case 2: strcpy(direcao_atual, "Cima"); break;
            case 1: strcpy(direcao_atual, "Baixo"); break;
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        last_direction = direction;
        if(strcmp(direcao_anterior, direcao_atual) != 0){
            printf("Direção %s \n", direcao_atual);
            strcpy(direcao_anterior, direcao_atual);
        }
        
        if(mqtt_comm_publish("ha/desafio11/marina.leite/joy", direcao_atual, strlen(direcao_atual))){
            gpio_put(led_pin_green, 1); 
            sleep_ms(1000);
            gpio_put(led_pin_green, 0);
        }
        vTaskDelay(300 / portTICK_PERIOD_MS); 
    }
}

int main(){
    stdio_init_all();
    adc_init();
    setup();
    
    connect_to_wifi("Laica-IoT", "Laica321");
    printf("Wi-Fi conectado!\n");

    mqtt_setup("marinaLeite123", "200.137.1.176", "desafio11", "desafio11.laica");
    
    xTaskCreate(vTemperatureTask, "Temperature Task", 256, NULL, 1, NULL);
    xTaskCreate(vJoystickTask, "Joystick Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();
}
