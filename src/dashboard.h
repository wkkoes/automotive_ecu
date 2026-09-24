#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <stdbool.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

void dashboard_init(void);
void dashboard_led_set(bool state);
void dashboard_led_toggle(void);
void heartbeat_toggle(void);
void button_pressed_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void dashboard_check_alarms(int current_distance_cm);
void alarm_timer_handler(struct k_timer *timer_id);

#endif
