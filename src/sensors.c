#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include "sensors.h"

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);

const struct device *sens_bme280 = DEVICE_DT_GET(DT_NODELABEL(temp_press_sensor));
static const struct gpio_dt_spec trigger_pin = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, trigger_gpios);
static const struct gpio_dt_spec echo_pin = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, echo_gpios);


struct sensor_value press_bme280, temp_bme280;
struct sensor_value distance_hcsr04;

void sensors_init(void)
{

    if(!device_is_ready(sens_bme280))
    {
        LOG_ERR("BME280 sensor not ready");
        return;
    }

    if((!gpio_is_ready_dt(&trigger_pin)) || !gpio_is_ready_dt(&echo_pin))
    {
        LOG_ERR("HCSR04 device not ready");
        return;
    }
    gpio_pin_configure_dt(&trigger_pin, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&echo_pin, GPIO_INPUT);
}

void bme280_read(void)
{
    int bme280_data = sensor_sample_fetch(sens_bme280);
    if(bme280_data < 0)
    {
        LOG_ERR("BME280 read error");
        return;
    }
    
    sensor_channel_get(sens_bme280, SENSOR_CHAN_PRESS, &press_bme280);
    sensor_channel_get(sens_bme280, SENSOR_CHAN_AMBIENT_TEMP, &temp_bme280); 
}

void hcsr04_read(void)
{
    gpio_pin_set_dt(&trigger_pin, 1);
    k_busy_wait(10);
    gpio_pin_set_dt(&trigger_pin, 0);    

    uint32_t wait_timeout_start = k_cycle_get_32();
    while(gpio_pin_get_dt(&echo_pin) == 0)
    {

        uint32_t actual_time = k_cycle_get_32();
        uint32_t elapsed_time_us = k_cyc_to_us_floor32(actual_time - wait_timeout_start);
        if(elapsed_time_us > 4000)
        {
            LOG_ERR("ERROR reading from hc-sr04");
            return;
        }
    }

    uint32_t echo_start = k_cycle_get_32();
    while(gpio_pin_get_dt(&echo_pin) == 1)
    {

        uint32_t wave_actual = k_cycle_get_32();
        uint32_t wave_timeout = k_cyc_to_us_floor32(wave_actual - echo_start);
        if(wave_timeout > 50000)
        {
            LOG_ERR("ERROR reading from h-sr04");
            return;
        }
    }
    
    uint32_t echo_end = k_cycle_get_32();
    uint32_t time_us = k_cyc_to_us_floor32(echo_end - echo_start);
    distance_hcsr04.val1 = time_us / 58;
    distance_hcsr04.val2 = ((time_us % 58) * 1000000) / 58;
}

int hcsr04_get_distance(void)
{
    return distance_hcsr04.val1;
}

void sensors_log(void)
{
    printk("\033[H"); 
    printk("=== AUTOMOTIVE ECU DASHBOARD ===\n");
    printk("Temperature: %d.%06d C      \n", temp_bme280.val1, temp_bme280.val2);
    printk("Pressure:   %d.%06d kPa    \n", press_bme280.val1, press_bme280.val2);
    printk("Distance:     %d.%06d cm     \n", distance_hcsr04.val1, distance_hcsr04.val2);
}