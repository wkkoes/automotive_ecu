#ifndef IMU.H
#define IMU.H

#include <stdbool.h>


void bmi160_init(void);
void bmi160_read(void);
void imu_thread_function(void *arg1, void *arg2, void *arg3);












#endif