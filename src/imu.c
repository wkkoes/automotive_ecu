#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>

#define IMU_STACK_SIZE 1024
#define IMU_PRIORITY 5

LOG_MODULE_REGISTER(imu, LOG_LEVEL_INF);

const struct device *imu_dev = DEVICE_DT_GET(DT_NODELABEL(bmi160));
struct sensor_value imu_bmi160[3];

void bmi160_init()
{
    if(!device_is_ready(imu_dev))
    {
        LOG_ERR("IMU device not ready");
        return;
    }
}

void bmi160_read()
{
    int imu_data = sensor_sample_fetch(imu_dev);
    if(imu_data < 0)
    {
        LOG_ERR("BMI160 read error");
        return;
    }

    sensor_channel_get(imu_dev, SENSOR_CHAN_ACCEL_XYZ, imu_bmi160);
}

void imu_thread_function(void *arg1, void *arg2, void *arg3)
{
    bmi160_init();

    while(1)
    {
        bmi160_read();
        k_msleep(100);
    }
}
K_THREAD_DEFINE(imu_thread_id, IMU_STACK_SIZE, imu_thread_function, NULL, NULL, NULL, IMU_PRIORITY, 0, 0);

