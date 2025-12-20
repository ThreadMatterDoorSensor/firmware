#pragma once

using door_sensor_callback_t = void (*)(bool is_open);

int door_sensor_init();
bool door_sensor_is_open();
void door_sensor_set_callback(door_sensor_callback_t cb);
