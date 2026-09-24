#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include "dashboard.h"

#define LED0_NODE DT_ALIAS(led0)
#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

#define MAX_TIME_MS 500
#define MIN_TIME_MS 50
#define MAX_DISTANCE_CM 15
#define MIN_DISTANCE_CM 3

LOG_MODULE_REGISTER(dashboard, LOG_LEVEL_INF);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_NODELABEL(lights_led), gpios);
static const struct gpio_dt_spec parking_led = GPIO_DT_SPEC_GET(DT_NODELABEL(warning_led), gpios);
static const struct gpio_dt_spec parking_buzzer = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), buzzer_gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_NODELABEL(steering_button), gpios);
static const struct gpio_dt_spec heartbeat_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct gpio_callback button_cb_data;


void alarm_timer_handler(struct k_timer *timer_id)
{
    gpio_pin_toggle_dt(&parking_led);
    gpio_pin_toggle_dt(&parking_buzzer);
}
K_TIMER_DEFINE(alarm_timer, alarm_timer_handler, NULL);


void button_pressed_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    dashboard_led_toggle();
}

void dashboard_init(void)
{
    if(!gpio_is_ready_dt(&heartbeat_led))
    {
        LOG_ERR("System LED not ready");
        return;
    }
    gpio_pin_configure_dt(&heartbeat_led, GPIO_OUTPUT_ACTIVE);

    if(!gpio_is_ready_dt(&led))
    {
        LOG_ERR("Blue LED device not ready");
        return;
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    if(!gpio_is_ready_dt(&button))
    {
        LOG_ERR("Button device not ready");
        return;
    }
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    if(!gpio_is_ready_dt(&parking_led))
    {
        LOG_ERR("Parking LED not ready");
        return;
    }
    gpio_pin_configure_dt(&parking_led, GPIO_OUTPUT_INACTIVE);

    if(!gpio_is_ready_dt(&parking_buzzer))
    {
        LOG_ERR("Parking buzzer not ready");
        return;
    }
    gpio_pin_configure_dt(&parking_buzzer, GPIO_OUTPUT_INACTIVE);

    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button_cb_data, button_pressed_callback, BIT(button.pin));
    gpio_add_callback_dt(&button, &button_cb_data);
}

void heartbeat_toggle(void)
{
    gpio_pin_toggle_dt(&heartbeat_led);
}

void dashboard_led_set(bool led_state)
{
    gpio_pin_set_dt(&led, led_state);
}

void dashboard_led_toggle(void)
{
    gpio_pin_toggle_dt(&led);
}

void dashboard_check_alarms(int current_distance_cm)
{
    int delay_ms = MIN_TIME_MS + ((current_distance_cm - MIN_DISTANCE_CM) * (MAX_TIME_MS - MIN_TIME_MS)) / (MAX_DISTANCE_CM - MIN_DISTANCE_CM);

    if(current_distance_cm >= 0 && current_distance_cm <= MIN_DISTANCE_CM)
    {
        k_timer_stop(&alarm_timer);
        gpio_pin_set_dt(&parking_led, 1);
        gpio_pin_set_dt(&parking_buzzer, 1);

    }
    
    else if(current_distance_cm > MIN_DISTANCE_CM && current_distance_cm <= MAX_DISTANCE_CM)
    {

        k_timer_start(&alarm_timer, K_MSEC(delay_ms), K_MSEC(delay_ms));
    }

    else
    {
        k_timer_stop(&alarm_timer);
        gpio_pin_set_dt(&parking_led, 0);
        gpio_pin_set_dt(&parking_buzzer, 0);
    }

}

