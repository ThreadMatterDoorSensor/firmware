#pragma once

#include <cstdint>

int power_monitor_init();
int power_monitor_read_mv(int32_t *voltage_mv);
