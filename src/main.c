#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define LED_PIN    GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_0 
#define PUMP_PIN GPIO_NUM_4

void app_main(void) {
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_set_direction(PUMP_PIN, GPIO_MODE_OUTPUT);

    // Button input with pull-up
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);


    while (1) {

        //V1: Push Button Water Release Code
        /*if (gpio_get_level(BUTTON_PIN) == 0) {
            gpio_set_level(PUMP_PIN, 1);
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(3000));  
            gpio_set_level(PUMP_PIN, 0);
            gpio_set_level(LED_PIN, 0);
        }*/

        //V2: Timed Water Release Code Based off MCU Uptime
        uint64_t localUptime = esp_timer_get_time();
        {
            gpio_set_level(PUMP_PIN, 1);
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(5000));
            gpio_set_level(PUMP_PIN, 0);
            gpio_set_level(LED_PIN, 0);
            ESP_LOGI("TAG", "Watered and the current time is: %d", localUptime);
            vTaskDelay(pdMS_TO_TICKS(3600000));//Wait an hour then push the code again
        }
    }
}