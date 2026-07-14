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
#include "esp_adc/adc_oneshot.h"

#define LED_PIN     GPIO_NUM_2
#define BUTTON_PIN  GPIO_NUM_0 
#define PUMP_PIN    GPIO_NUM_4
#define ADC_UNIT    ADC_UNIT_1
#define ADC_CHANNEL ADC_CHANNEL_5 // Port 33 on a ESP32 Base
#define ADC_ATTENUATION ADC_ATTEN_DB_12
#define LOW_WET     1100 //The lowest value the sensor can read meaning its submerged  
#define MAX_DRY     3280 //The max value the sensor can read, a high value means dry soil
#define SENSOR_DIFF 2180 //Difference between the Max-low used to calculate eprcentage


void app_main(void) {
    int soil_raw;
    int32_t percentage_sensor;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

      adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTENUATION,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));

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

        //V1: Push Button Water Release Code
         if (gpio_get_level(BUTTON_PIN) == 0) {
            gpio_set_level(PUMP_PIN, 1);
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(3000));  
            gpio_set_level(PUMP_PIN, 0);
            gpio_set_level(LED_PIN, 0);
        }
        /*
        //V2: Timed Water Release Code Based off MCU Uptime
        uint64_t localUptime = esp_timer_get_time();
        {
            gpio_set_level(PUMP_PIN, 1);
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(5000));
            gpio_set_level(PUMP_PIN, 0);
            gpio_set_level(LED_PIN, 0);,
            ESP_LOGI("TAG", "Watered and the current time is: %d", localUptime);
            vTaskDelay(pdMS_TO_TICKS(3600000));//Wait an hour then push the code again
        }*/
       
       //v4: Soil Sensor based released that will provide the raw value between 1100-3280 and a converted percent based value
       ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &soil_raw));
       ESP_LOGI("TAG", "Current Soil Level: %d ", soil_raw);
       percentage_sensor = ((MAX_DRY - soil_raw)*100) / (SENSOR_DIFF);
       ESP_LOGI("TAG", "The soil is %d % wet", percentage_sensor);
        
       if(percentage_sensor <= 5){
            gpio_set_level(PUMP_PIN, 1);
            gpio_set_level(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(3000));
            gpio_set_level(PUMP_PIN, 0);
            gpio_set_level(LED_PIN, 0);
            ESP_LOGI("TAG", "Plant has been watered");
       }
       vTaskDelay(pdMS_TO_TICKS(5000));

    }
}
