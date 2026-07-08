#include "esp_wifi.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

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