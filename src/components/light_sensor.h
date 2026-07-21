//This header file will be utilized to configure the light sensor using I2C based off the Docs
//https://docs.espressif.com/projects/esp-idf/en/v5.1/esp32/api-reference/peripherals/i2c.html
#include "driver/i2c_master.h"

static const char *LOG = "veml7700_sensor";


#define I2C_MASTER_SCL_IO           GPIO_NUM_16    /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_NUM_17     /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0       /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          100000        /*!< I2C master clock frequency */
#define I2C_MASTER_TIMEOUT_MS       1000

#define VEML7700_SENSOR_ADDR         0x10        /*!< Address of the VEML7700 sensor */
#define VEML7700_DEVICE_ID           0x07        /*!< Device ID of the VEML7700 sensor */
#define VEML7700_ALS_REG            0x04        /*!< Ambient Light Singal (ALS) of the VEML7700 sensor */
#define VEML7700_PWR_REG            0X00        /*!< Register utilized for turnning the sensor off/on*/
#define VEML7700_RESOLUTION         0.0576      /*!< Reloution multiplier used to convert the raw lux*/


static void i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = VEML7700_SENSOR_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_config, dev_handle));
}
