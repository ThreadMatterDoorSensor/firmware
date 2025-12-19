#ifndef DOOR_SENSOR_H
#define DOOR_SENSOR_H

#include <stdbool.h>

typedef void (*door_sensor_callback_t)(bool is_open);

int door_sensor_init(void);
bool door_sensor_is_open(void);
void door_sensor_set_callback(door_sensor_callback_t cb);

#endif
