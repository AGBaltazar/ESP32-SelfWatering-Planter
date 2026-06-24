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
#include "driver/gpio.h"
#include "secrets.h"

#define LED_PIN    GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_0 
#define PUMP_PIN GPIO_NUM_4

#define WIFI_SSID      SECRET_SSID
#define WIFI_PASS      SECRET_PASS

static const char *TAG = "wifi_server";
static int led_state = 0;


const char html_page[] = 
    "<!DOCTYPE html><html>"
    "<head><title>ESP32 Water Pump Control</title>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<style>body{text-align:center;font-family:sans-serif;} .btn{padding:20px;font-size:24px;}</style></head>"
    "<body><h1>Water Pump Control</h1>"
    "<p><a href='/toggle'><button class='btn'>Enable Water Pump</button></a></p>"
    "<p><a href='/toggle_five'><button class='btn'>Enable 5 Second Water Pump</button></a></p>"
    "</body></html>";


static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
};


static esp_err_t toggle_get_handler(httpd_req_t *req) {
    led_state = !led_state;
    gpio_set_level(PUMP_PIN, led_state);
    gpio_set_level(LED_PIN, 0);
    ESP_LOGI(TAG, "GPIO %d toggled to: %d", PUMP_PIN, led_state);
    
    // Redirect back to the main page
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
};
static esp_err_t toggle_get_handler_five(httpd_req_t *req) {
    led_state = !led_state;
    gpio_set_level(PUMP_PIN, led_state);
    gpio_set_level(LED_PIN, 1);
    ESP_LOGI(TAG, "GPIO %d toggled to: %d", PUMP_PIN, led_state);
    vTaskDelay(pdMS_TO_TICKS(5000));  
    gpio_set_level(PUMP_PIN, 0);
    gpio_set_level(LED_PIN, 0);

    
    // Redirect back to the main page
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
};

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t toggle = {
    .uri       = "/toggle",
    .method    = HTTP_GET,
    .handler   = toggle_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t toggle_five = {
    .uri       = "/toggle_five",
    .method    = HTTP_GET,
    .handler   = toggle_get_handler_five,
    .user_ctx  = NULL
};


static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &toggle);
        httpd_register_uri_handler(server, &toggle_five);
        return server;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
};


static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected from Wi-Fi. Retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected! IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        start_webserver();
    }
};

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
