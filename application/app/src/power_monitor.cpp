#include "power_monitor.h"

#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(power_monitor, CONFIG_LOG_DEFAULT_LEVEL);

static const struct adc_dt_spec adc_ch = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

static int16_t sample_buf;
static struct adc_sequence sequence = {
	.buffer = &sample_buf,
	.buffer_size = sizeof(sample_buf),
};

int power_monitor_init()
{
	if (!adc_is_ready_dt(&adc_ch)) {
		LOG_ERR("ADC device not ready");
		return -ENODEV;
	}

	int err = adc_channel_setup_dt(&adc_ch);
	if (err < 0) {
		LOG_ERR("ADC channel setup failed (%d)", err);
	}
	return err;
}

int power_monitor_read_mv(int32_t *voltage_mv)
{
	int err = adc_sequence_init_dt(&adc_ch, &sequence);
	if (err < 0) {
		return err;
	}

	err = adc_read(adc_ch.dev, &sequence);
	if (err < 0) {
		LOG_ERR("ADC read failed (%d)", err);
		return err;
	}

	*voltage_mv = sample_buf;
	return adc_raw_to_millivolts_dt(&adc_ch, voltage_mv);
}
