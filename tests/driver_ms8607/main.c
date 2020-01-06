/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the MS8607 temperature, pressure, and
 *              humidity sensor driver
 *
 * @author      Thomas POTIER <thomas.potier@u-psud.fr>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "ms8607_params.h"
#include "ms8607.h"
#include "xtimer.h"
#include "fmt.h"

#define MAINLOOP_DELAY  (2)         /* read sensor every 2 seconds */

int main(void)
{
	ms8607_t dev;
	puts("ms8607 test application\n");
	puts("+------------Initializing------------+");
	switch (ms8607_init(&dev, &ms8607_params[0])) {
		case MS8607_ERR_BUS:
			puts("[Error] Something went wrong when using the I2C bus");
			return 1;
		case MS8607_ERR_NODEV:
			puts("[Error] Unable to communicate with any BMX280 device");
			return 1;
		default:
			/* all good -> do nothing */
			break;
	}
	puts("Initialization successful\n");
	puts("+------------Calibration Data------------+");
	printf("C1: %u\n", dev.calibration.C1);
	printf("C2: %u\n", dev.calibration.C2);
	printf("C3: %u\n", dev.calibration.C3);
	printf("C4: %u\n", dev.calibration.C4);
	printf("C5: %u\n", dev.calibration.C5);
	printf("C6: %u\n", dev.calibration.C6);
	puts("\n+--------Starting Measurements--------+");
	while (1) {
		/* read temperature, pressure [and humidity] values */
		int32_t temperature = ms8607_read_temperature(&dev);
		int32_t pressure = ms8607_read_pressure(&dev);
		int16_t humidity = ms8607_read_humidity(&dev);
		/* format values for printing */
		char str_temp[8];
		size_t len = fmt_s16_dfp(str_temp, temperature, -2);
		str_temp[len] = '\0';
		char str_hum[8];
		len = fmt_s16_dfp(str_hum, humidity, -2);
		str_hum[len] = '\0';
		/* print values to STDIO */
		printf("Temperature [°C]: %s\n", str_temp);
		printf("   Pressure [Pa]: %" PRIu32 "\n", pressure);
		printf("  Humidity [%%rH]: %s\n", str_hum);
		puts("\n+-------------------------------------+\n");
		xtimer_sleep(MAINLOOP_DELAY);
	}
	return 0;
}
