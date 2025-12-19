#include "door_sensor.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(door_sensor);

static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(DT_NODELABEL(sensor), gpios);
static struct gpio_callback gpio_cb_data;
static door_sensor_callback_t state_changed_cb;

static void gpio_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	if (state_changed_cb) {
		state_changed_cb(door_sensor_is_open());
	}
}

int door_sensor_init(void)
{
	int ret;

	if (!gpio_is_ready_dt(&sw)) {
		LOG_ERR("GPIO device not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&sw, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Could not configure GPIO (%d)", ret);
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&sw, GPIO_INT_EDGE_BOTH);
	if (ret < 0) {
		LOG_ERR("Could not configure GPIO interrupt (%d)", ret);
		return ret;
	}

	gpio_init_callback(&gpio_cb_data, gpio_callback, BIT(sw.pin));

	ret = gpio_add_callback(sw.port, &gpio_cb_data);
	if (ret < 0) {
		LOG_ERR("Could not add GPIO callback (%d)", ret);
		return ret;
	}

	return 0;
}

bool door_sensor_is_open(void)
{
	return gpio_pin_get_dt(&sw) != 0;
}

void door_sensor_set_callback(door_sensor_callback_t cb)
{
	state_changed_cb = cb;
}
