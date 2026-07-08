#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "../secrets.h"
#include "./components/wifi.h"

#define LED_PIN    GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_0 
#define PUMP_PIN GPIO_NUM_4


void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_set_direction(PUMP_PIN, GPIO_MODE_OUTPUT);

    // Button input with pull-up
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    /* //V3 WiFi Web Server
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    */

    while (1) {

        /*//V1: Push Button Water Release Code
         if (gpio_get_level(BUTTON_PIN) == 0) {
            gpio_set_level(PUMP_PIN, 1);
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(3000));  
            gpio_set_level(PUMP_PIN, 0);
            gpio_set_level(LED_PIN, 0);
        }

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


    */
    vTaskDelay(pdMS_TO_TICKS(100000));

    }
}
