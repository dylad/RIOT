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

#include "saul.h"

#include "ms8607.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    res->val[0] = ms8607_read_temperature((ms8607_t *)dev);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_pressure(const void *dev, phydat_t *res)
{
    res->val[0] = ms8607_read_pressure((ms8607_t *)dev) / 100;
    res->unit = UNIT_PA;
    res->scale = -5;

    return 1;
}

static int read_relative_humidity(const void *dev, phydat_t *res)
{
    res->val[0] = ms8607_read_humidity((ms8607_t *)dev);
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t ms8607_temperature_saul_driver = {
    .read = read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t ms8607_pressure_saul_driver = {
    .read = read_pressure,
    .write = saul_notsup,
    .type = SAUL_SENSE_PRESS,
};

const saul_driver_t ms8607_relative_humidity_saul_driver = {
    .read = read_relative_humidity,
    .write = saul_notsup,
    .type = SAUL_SENSE_HUM,
};
