#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>

void sensors_init(void);
void bme280_read(void);
void hcsr04_read(void);
void sensors_log(void);
int hcsr04_get_distance(void);

#endif
