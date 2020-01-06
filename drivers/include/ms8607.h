/**
 * @defgroup    drivers_ms8607 MS8607 temperature, pressure and humidity
 *                             sensor
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the TE MS8607 sensor
 *
 * MS8607 measures temperature in centi °C and pressure in mbar. MS8607
 * can also measure relative humidity in %.
 *
 * For more information, see the datasheets:
 * * [MS8607](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS8607-02BA01%7FB%7Fpdf%7FEnglish%7FENG_DS_MS8607-02BA01_B.pdf%7FCAT-BLPS0018)
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * ## Usage
 *
 * To include this driver to your application, simply add one of the following
 * to the application's Makefile:
 *
 * ```make
 * # MS8607 connected
 * USEMODULE += ms8607
 *
 * # Specify the default I2C device to use,
 * # and the MS8607's addresses (see datasheet).
 * # The values below are the defaults:
 * CFLAGS += -DMS8607_PARAM_I2C_DEV=I2C_DEV\(0\)
 * CFLAGS += -DMS8607_PARAM_I2C_ADDR_PT=0x77
 * CFLAGS += -DMS8607_PARAM_I2C_ADDR_H=0x77
 *
 * ```
 *
 * This way the default parameters in `drivers/ms8607/include/ms8607_params.h`
 * are replaced by these new values.
 *
 * @{
 * @file
 * @brief       Device driver interface for the BMP280 and BME280 sensors
 *
 * @details     There are three sensor values that can be read: temperature,
 *              pressure and humidity. The MS8607 device usually measures them
 *              all at once.  It is possible to skip measuring either of the
 *              values by changing the oversampling settings.
 *
 * @author      Thomas POTIER <thomas.potier@u-psud.fr>
 *
 */

#ifndef MS8607_H
#define MS8607_H

#include <stdint.h>
#include "saul.h"

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calibration struct for the MS8607 sensor
 *
 * This must be read from the device at startup.
 */
typedef struct {
    uint16_t C1;        /**< Pressure sensitivity | SENST1 */
    uint16_t C2;        /**< Pressure offset | OFFT1 */
    uint16_t C3;        /**< Temperature coefficient of pressure sensitivity | TCS */
    uint16_t C4;        /**< Temperature coefficient of pressure offset | TCO */
    uint16_t C5;        /**< Reference temperature | TREF */
    uint16_t C6;        /**< Temperature coefficient of the temperature | TEMPSENS */
} ms8607_calibration_t;

/**
 * @brief   Values of OSR for conversion commands (PT)
 */
typedef enum {
    MS8607_OSR_CMD_256	= 0x0,
    MS8607_OSR_CMD_512	= 0x2,
    MS8607_OSR_CMD_1024	= 0x4,
    MS8607_OSR_CMD_2048	= 0x6,
    MS8607_OSR_CMD_4096	= 0x8,
    MS8607_OSR_CMD_8192	= 0xA,
} ms8607_osr_cmd_t;

/**
 * @brief   Values of OSR for user register (H)
 */
typedef enum {
    MS8607_OSR_REG_256	= 0x81,
    MS8607_OSR_REG_1024	= 0x80,
    MS8607_OSR_REG_2048	= 0x01,
    MS8607_OSR_REG_4096	= 0x00,
} ms8607_osr_reg_t;

/**
 * @brief   Status of heater for user register (H)
 */
typedef enum {
    MS8607_HEATER_ON	= 0x04,
    MS8607_HEATER_OFF	= 0x00,
} ms8607_heater_t;

/**
 * @brief   Parameters for the MS8607 sensor
 *
 * These parameters are needed to configure the device at startup.
 */
typedef struct {
    i2c_t i2c_dev;						/**< I2C device which is used */
    uint8_t i2c_addr_pt;				/**< I2C address for PT */
    uint8_t i2c_addr_h;					/**< I2C address for T */
    ms8607_osr_cmd_t osr_p;				/**< OSR value for P */
    ms8607_osr_cmd_t osr_t;				/**< OSR value for T */
    ms8607_osr_reg_t osr_h;				/**< OSR value for H */
	ms8607_heater_t heater_status;		/**< Heater status */
} ms8607_params_t;

/**
 * @brief   Device descriptor for the MS8607 sensor
 */
typedef struct {
    ms8607_params_t params;             /**< Device Parameters */
    ms8607_calibration_t calibration;   /**< Calibration Data */
} ms8607_t;

/**
 * @brief   Status and error return codes
 */
typedef enum {
    MS8607_OK           =  0,     /**< everything was fine */
    MS8607_ERR_BUS      = -1,     /**< bus error */
    MS8607_ERR_NODEV    = -2,     /**< did not detect MS8607 */
} ms8607_return_t;

/**
 * @brief   Export of SAUL interface for temperature sensor
 */
extern const saul_driver_t ms8607_temperature_saul_driver;

/**
 * @brief   Export of SAUL interface for pressure sensor
 */
extern const saul_driver_t ms8607_pressure_saul_driver;

/**
 * @brief   Export of SAUL interface for humidity sensor
 */
extern const saul_driver_t ms8607_relative_humidity_saul_driver;

/**
 * @brief   Initialize the given MS8607 device
 *
 * @param[out] dev      device descriptor of the given MS8607 device
 * @param[in]  params   static configuration parameters
 *
 * @return  MS8607_OK on success
 * @return  MS8607_ERR_BUS on bus error
 * @return  MS8607_ERR_NODEV if no corresponding device was found on the bus
 */
int ms8607_init(ms8607_t *dev, const ms8607_params_t *params);

/**
 * @brief   Read temperature value from the given MS8607 device
 *
 * The measured temperature is returned in centi °C (x.xx°C).
 *
 * @param[in] dev       device to read from
 *
 * @return  measured temperature in centi Celsius
 * @return  INT16_MIN on error
 */
int32_t ms8607_read_temperature(ms8607_t *dev);

/**
 * @brief   Read air pressure value from the given MS8607 device
 *
 * The measured air pressure is returned in ubar (xxxx.xxx mbar) .
 *
 * @param[in]  dev      device to read from
 *
 * @return  air pressure in ubar
 */
int32_t ms8607_read_pressure(ms8607_t *dev);

/**
 * @brief   Read relative humidity value from the given MS8607 device
 *
 * The measured humidity is returned in centi %RH (x.xx% relative humidity).
 *
 * @param[in]  dev      device to read from
 *
 * @return  humidity in centi %RH (i.e. the percentage times 100)
 */
int16_t ms8607_read_humidity(ms8607_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MS8607_H */
/** @} */
