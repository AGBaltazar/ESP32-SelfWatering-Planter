#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_common.h"

#define LCDHOST              SPI2_HOST      /*!<*/
#define SPI_MASTER_SDA_PIN     GPIO_NUM_14  /*!<*/
#define SPI_MASTER_SCL_PIN    GPIO_NUM_12   /*!<*/
#define SPI_MASTER_SCLK_SPEED        100000 /*!<*/

#define LCD_DC_PIN       GPIO_NUM_25    /*!<*/
#define LCD_CS_PIN       GPIO_NUM_26    /*!<*/
#define LCD_RST_PIN      GPIO_NUM_22    /*!<*/
#define LCD_PIXEL_CLK    20*1000*1000   /*!<*/
#define LCD_CMD_BITS     8
#define LCD_PARAM_BITS   8

esp_lcd_panel_io_handle_t io_handle = NULL;
esp_lcd_panel_handle_t esp_lcd_panel_handle = NULL;

void lcd_init(void) {
    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = SPI_MASTER_SDA_PIN,
        .sclk_io_num = SPI_MASTER_SCL_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCDHOST, &spi_bus_config, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_dev_config = {
        .dc_gpio_num    = LCD_DC_PIN,
        .cs_gpio_num    = LCD_CS_PIN,
        .pclk_hz        = LCD_PIXEL_CLK,
        .lcd_cmd_bits   = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode       = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCDHOST, &io_dev_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num  = LCD_RST_PIN,
        .rgb_ele_order   = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel  = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &esp_lcd_panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(esp_lcd_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(esp_lcd_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(esp_lcd_panel_handle, true));
}