#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "dashboard.h"
#include "sensors.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    dashboard_init();
    sensors_init();

    while(1)
    {
        heartbeat_toggle();

        bme280_read();
        hcsr04_read();
        
        int distance = hcsr04_get_distance();
        dashboard_check_alarms(distance);

        sensors_log();
        
        k_msleep(1000);
    }
}
   

    