/**
 * @ingroup     drivers_ms8607
 * @{
 *
 * @file
 * @brief       Device driver implementation for MS8607 sensors
 *
 * @author      Thomas POTIER <thomas.potier@u-psud.fr>
 *
 * @}
 */

#include <string.h>
#include <math.h>

#include "log.h"
#include "assert.h"
#include "ms8607.h"
#include "ms8607_internals.h"
#include "xtimer.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   Read the calibration data from sensor PROM
 */
static int _read_calibration_data(ms8607_t *dev)
{
	// bus should be already aquired
	uint8_t buf[2 * MS8607_CMD_PT_PROM_MASK];
	for (int i = 0; i < MS8607_CMD_PT_PROM_MASK; i++) {
		i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_pt,
				MS8607_CMD_PT_PROM_RD | (i << MS8607_CMD_PT_PROM_SHFT), 0);
		if (i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr_pt,
					&(buf[2 * i]), 2, 0) != 0)
			return (MS8607_ERR_BUS);
	}
	dev->calibration.C1 = buf[2] << 8 | buf[3];
	dev->calibration.C2 = buf[4] << 8 | buf[5];
	dev->calibration.C3 = buf[6] << 8 | buf[7];
	dev->calibration.C4 = buf[8] << 8 | buf[9];
	dev->calibration.C5 = buf[10] << 8 | buf[11];
	dev->calibration.C6 = buf[12] << 8 | buf[13];
	return MS8607_OK;
}

int ms8607_init(ms8607_t *dev, const ms8607_params_t *params)
{
	uint8_t reg;

	assert(dev && params);
	dev->params = *params;
	/* acquire bus */
	if (i2c_acquire(dev->params.i2c_dev) != 0) {
		DEBUG("[ms8607] error: unable to acquire bus\n");
		return MS8607_ERR_BUS;
	}
	/* reset PT */
	if (i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_pt,
			MS8607_CMD_PT_RST, 0) != 0) {
		DEBUG("[ms8607] error: unable to reset PT\n");
		goto err;
	}
	/* read the compensation data from the sensor's PROM */
	if (_read_calibration_data(dev) != MS8607_OK) {
		DEBUG("[ms8607] error: could not read calibration data\n");
		goto err;
	}
	/* reset H */
	if (i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_h,
			MS8607_CMD_H_RST, 0) != 0) {
		DEBUG("[ms8607] error: unable to reset H\n");
		goto err;
	}
	/* init user register */
	if ((i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_h,
					MS8607_CMD_H_UR_RD, 0) != 0)
			|| (i2c_read_byte(dev->params.i2c_dev, dev->params.i2c_addr_h, &reg, 0)
				!= 0)) {
		DEBUG("[ms8607] error: unable to read user register\n");
		goto err;
	}
	reg &= ~MS8607_CMD_H_UR_OSR_MSK;
	reg |= dev->params.osr_h;
	if (i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr_h,
					MS8607_CMD_H_UR_WR, reg, 0) != 0) {
		DEBUG("[ms8607] error: unable to write user register\n");
		goto err;
	}
	i2c_release(dev->params.i2c_dev);
	return MS8607_OK;
err:
	i2c_release(dev->params.i2c_dev);
	DEBUG("[ms8607] init: bus error while initializing device\n");
	return MS8607_ERR_BUS;
}

static int _read_adc_pt(ms8607_t *dev, uint32_t *uncompensated, uint8_t cmd)
{
	uint8_t buf[3];

	/* acquire bus */
	if (i2c_acquire(dev->params.i2c_dev) != 0) {
		DEBUG("[ms8607] error: unable to acquire bus\n");
		goto err;
	}
	/* start conversion */
	if (i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_pt, cmd, 0)
			!= 0) {
		DEBUG("[ms8607] error: unable to send conversion command\n");
		goto err;
	}
	xtimer_usleep(20000);
	/* read adc */
	if (i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_pt,
			MS8607_CMD_PT_ADC_ADC_RD, 0) != 0) {
		DEBUG("[ms8607] error: unable to send adc read command\n");
		goto err;
	}
	if (i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr_pt,
				buf, 3, 0) != 0) {
		DEBUG("[ms8607] error: unable to receive data\n");
		goto err;
	}
	*uncompensated = buf[0] << 16 | buf[1] << 8 | buf[2];
	i2c_release(dev->params.i2c_dev);
	return MS8607_OK;
err:
	i2c_release(dev->params.i2c_dev);
	DEBUG("[ms8607] read_adc_pt: bus error\n");
	return MS8607_ERR_BUS;
}

static int _read_rh(ms8607_t *dev, uint16_t *d3)
{
	uint8_t buf[3];
	int status;

	/* acquire bus */
	if (i2c_acquire(dev->params.i2c_dev) != 0) {
		DEBUG("[ms8607] error: unable to acquire bus\n");
		goto err;
	}
	/* start measure */
	if (i2c_write_byte(dev->params.i2c_dev, dev->params.i2c_addr_h,
				MS8607_CMD_H_MEAS_NHOLD, 0) != 0) {
		DEBUG("[ms8607] error: unable to send measure command\n");
		goto err;
	}
	do {
		xtimer_usleep(15000);
		status = i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr_h,
				&buf, 3, 0);
	} while (status == EIO);
	if (status != 0) {
		DEBUG("[ms8607] error: unable to receive data\n");
		goto err;
	}
	*d3 = (buf[0] << 8 | buf[1]) & 0xFFFC;
	i2c_release(dev->params.i2c_dev);
	return MS8607_OK;
err:
	i2c_release(dev->params.i2c_dev);
	DEBUG("[ms8607] read_rh: bus error\n");
	return MS8607_ERR_BUS;
}

int32_t ms8607_read_temperature(ms8607_t *dev)
{
	uint32_t uncompensated_temp;
	int32_t dt;

	assert(dev);
	if (_read_adc_pt(dev, &uncompensated_temp, MS8607_CMD_PT_CONV_D2
				| dev->params.osr_t) != MS8607_OK)
		return (INT32_MIN);
	dt = uncompensated_temp - dev->calibration.C5 * 256;
	return (2000 + dt * dev->calibration.C6 / 8388608);
}

int32_t ms8607_read_pressure(ms8607_t *dev)
{
	uint32_t d2;
	uint32_t d1;
	int32_t dt;
	int64_t off;
	int64_t sens;

	assert(dev);
	if ((_read_adc_pt(dev, &d2, MS8607_CMD_PT_CONV_D2
					| dev->params.osr_t) != MS8607_OK)
			|| (_read_adc_pt(dev, &d1, MS8607_CMD_PT_CONV_D1
					| dev->params.osr_p) != MS8607_OK))
		return (INT32_MIN);
	dt = d2 - dev->calibration.C5 * 256;
	off = dev->calibration.C2 * 131072
		+ (dev->calibration.C4 * dt) / 64;
	sens = dev->calibration.C1 * 65536
		+ (dev->calibration.C3 * dt) / 128;
	return ((d1 * sens / 2097152 - off) / 32768);
}

int16_t ms8607_read_humidity(ms8607_t *dev)
{
	uint16_t d3;

	assert(dev);
	if (_read_rh(dev, &d3) != MS8607_OK)
		return (INT16_MIN);
	return (-600 + 12500 * d3 / 65536);
}
