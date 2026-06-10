#include "door_sensor.h"
#include "app_task.h"
#include "power_monitor.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static void on_door_state_changed(bool is_open)
{
	gpio_pin_set_dt(&led, is_open);
	LOG_INF("Door %s", is_open ? "opened" : "closed");
	AppTask::UpdateContactState(is_open);

	int32_t voltage_mv;
	if (power_monitor_read_mv(&voltage_mv) == 0) {
		AppTask::UpdateBatVoltage(voltage_mv);
	}
}

int main(void)
{
	LOG_INF("Door Sensor Powered On");

	if (!gpio_is_ready_dt(&led)) {
		LOG_ERR("LED GPIO not ready");
		return 0;
	}

	int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		LOG_ERR("Could not configure LED GPIO (%d)", ret);
		return 0;
	}

	ret = power_monitor_init();
	if (ret < 0) {
		LOG_ERR("Could not init power monitor (%d)", ret);
		return 0;
	}

	int32_t voltage_mv;
	ret = power_monitor_read_mv(&voltage_mv);
	if (ret == 0) {
		LOG_INF("Supply voltage: %d mV", voltage_mv);
		AppTask::UpdateBatVoltage(voltage_mv);
	}

	ret = door_sensor_init();
	if (ret < 0) {
		LOG_ERR("Could not init door sensor (%d)", ret);
		return 0;
	}

	door_sensor_set_callback(on_door_state_changed);

	bool is_open = door_sensor_is_open();

	gpio_pin_set_dt(&led, is_open);
	LOG_INF("Initial state: Door %s", is_open ? "open" : "closed");

	AppTask::UpdateContactState(is_open);

	CHIP_ERROR err = AppTask::Instance().StartApp();

	LOG_ERR("Exited with code %" CHIP_ERROR_FORMAT, err.Format());
	return err == CHIP_NO_ERROR ? EXIT_SUCCESS : EXIT_FAILURE;
}
