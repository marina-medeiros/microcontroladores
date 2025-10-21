#include <stdio.h>
#include <string.h> 
#include<stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include "queue.h"
#include <time.h>
#include "pico/stdlib.h"
#include "include/mpu6050_i2c.h"
#include "include/wifi_conn.h"      // Funções personalizadas de conexão WiFi
#include "include/mqtt_comm.h"      // Funções personalizadas para MQTT
#include "pico/cyw43_arch.h"
#include "lwip/apps/sntp.h"
#include "lwip/dns.h"

const uint led_pin_green = 11;

// Configuração do LED
void setup(){
    gpio_init(led_pin_green);
    gpio_set_dir(led_pin_green, GPIO_OUT);
}

// Envio de dados do sensor para o broker
void vMPU6050Task(){
    int16_t accel_raw[3], gyro_raw[3], temp_raw;
    float previous_accel[3] = {0};
    float previous_gyro[3] = {0};
    float previous_temperature;
    int change = 0;

    while (true){
        change = 0;
        mpu6050_read_raw(accel_raw, gyro_raw, &temp_raw);

        // Converte e armazena os valores lidos pelo sensor 
        float accel[3] = {
            accel_raw[0] / 16384.0f, 
            accel_raw[1] / 16384.0f,
            accel_raw[2] / 16384.0f
        };

        float gyro[3] = {
            gyro_raw[0] / 131.0f,     
            gyro_raw[1] / 131.0f,
            gyro_raw[2] / 131.0f
        };

        float temperature = (temp_raw / 340.0f) + 36.53f; 

        // Registra a ocorrência de mudanças, se ocorrerem
        for(int ii = 0; ii < 3; ii++){
            if(fabs(previous_accel[ii] - accel[ii]) >= 5.0f){
                change = 1;
                printf("Accel: %f\n", fabs(previous_accel[ii] - accel[ii]));
                break;
            }if(fabs(previous_gyro[ii] - gyro[ii]) >= 5.0f){
                change = 2;
                printf("Gyro: %f\n", fabs(previous_gyro[ii] - gyro[ii]));
                break;
            }
            if(fabs(temperature - previous_temperature) > 0.5f){
                change = 3;
                printf("Temp: %f\n", fabs(temperature - previous_temperature));
                break;
            }
        }

        // Atualiza valores antigos vs. novos
        for(int ii = 0; ii < 3; ii++){
            previous_accel[ii] = accel[ii];
            previous_gyro[ii] = gyro[ii];
        }

        previous_temperature = temperature;

        time_t now = time(NULL);
        struct tm *t = gmtime(&now);  // use gmtime(&now) para UTC

        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", t);

        // Cria o JSON
        char mensagem[350];
        snprintf(mensagem, sizeof(mensagem),
            "{\n"
            "    \"team\": \"desafio11\",\n"
            "    \"device\": \"bitdoglab01_marina\",\n"
            "    \"ip\": \"10.228.1.105\",\n" 
            "    \"ssid\": \"wIFRN-IoT\",\n"
            "    \"sensor\": \"MPU-6050\",\n"
            "    \"data\": {\n"
            "        \"accel\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f},\n"
            "        \"gyro\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f},\n"
            "        \"temperature\": %.2f\n"
            "    },\n"
            "    \"timestamp\": \"%s\"\n" 
            "}",
            accel[0], accel[1], accel[2],
            gyro[0], gyro[1], gyro[2],
            temperature,
            timestamp
        );
        printf("Tamanho da mensagem: %d bytes\n", strlen(mensagem));
        printf("Mensagem JSON: %s\n", mensagem);

        // Envia o MQTT
        if (mqtt_comm_publish("ha/desafio11/marina.leite/mpu6050", mensagem, strlen(mensagem))) {
            printf("MQTT enviado!\n");
            gpio_put(led_pin_green, 1);
            sleep_ms(1000);
            gpio_put(led_pin_green, 0);
        }
        // Espera intervalos diferentes, dependendo da ocorrência de mudança
        if(change >= 1){
            printf("Change %d\n", change);
            vTaskDelay(10000 / portTICK_PERIOD_MS); 
        }else{
            printf("Não houveram mudanças. Espera de 35 segundos.\n");
            vTaskDelay(25000 / portTICK_PERIOD_MS); 
        }
    }
}

int main() {
    stdio_init_all();          // Inicializa USB serial
    sleep_ms(5000);
    mpu6050_setup_i2c();       // Configura barramento I2C
    mpu6050_reset();           // Reinicia o sensoreu 
    mpu6050_set_accel_range(1);
    setup();

    connect_to_wifi("Laica-IoT", "Laica321"); // Conecta ao Wi-Fi
    printf("Wi-Fi conectado!\n");

    mqtt_setup("marinaLeite321", "200.137.1.176", "desafio11", "desafio11.laica");

    xTaskCreate(vMPU6050Task, "Temperature Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();
}