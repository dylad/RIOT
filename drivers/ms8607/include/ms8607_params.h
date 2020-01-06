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

#ifndef MS8607_PARAMS_H
#define MS8607_PARAMS_H

#include "board.h"
#include "ms8607.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the MS8607
 * @{
 */
#ifndef MS8607_PARAM_I2C_DEV
#define MS8607_PARAM_I2C_DEV        I2C_DEV(0)
#endif
#ifndef MS8607_PARAM_I2C_ADDR_PT
#define MS8607_PARAM_I2C_ADDR_PT    (0x76)
#endif
#ifndef MS8607_PARAM_I2C_ADDR_H
#define MS8607_PARAM_I2C_ADDR_H     (0x40)
#endif
#ifndef MS8607_PARAM_OSR_P
#define MS8607_PARAM_OSR_P          (MS8607_OSR_CMD_8192)
#endif
#ifndef MS8607_PARAM_OSR_T
#define MS8607_PARAM_OSR_T          (MS8607_OSR_CMD_8192)
#endif
#ifndef MS8607_PARAM_OSR_H
#define MS8607_PARAM_OSR_H          (MS8607_OSR_REG_4096)
#endif

#ifndef MS8607_PARAMS
#define MS8607_PARAMS                           \
    {                                           \
        .i2c_dev  = MS8607_PARAM_I2C_DEV,       \
        .i2c_addr_pt = MS8607_PARAM_I2C_ADDR_PT,\
        .i2c_addr_h = MS8607_PARAM_I2C_ADDR_H,  \
        .osr_p = MS8607_PARAM_OSR_P,            \
        .osr_t = MS8607_PARAM_OSR_T,            \
        .osr_h = MS8607_PARAM_OSR_H,            \
    }
#endif
/**@}*/

/**
 * @brief   Configure BMX280
 */
static const ms8607_params_t ms8607_params[] =
{
    MS8607_PARAMS
};

/**
 * @brief   The number of configured sensors
 */
#define MS8607_NUMOF    (1)

/**
 * @brief   Configuration details of SAUL registry entries
 *
 */
static const saul_reg_info_t ms8607_saul_reg_info[MS8607_NUMOF] =
{
        { .name = "ms8607" }
};

#ifdef __cplusplus
}
#endif

#endif /* MS8607_PARAMS_H */
/** @} */
