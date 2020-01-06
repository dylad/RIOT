/**
 * @ingroup     drivers_bmx280
 * @brief       Internal addresses, registers, constants for the BMX280 family sensors.
 * @{
 * @file
 * @brief       Internal addresses, registers, constants for the BMX280 family sensors.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef MS8607_INTERNALS_H
#define MS8607_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    MS8607 commands
 * @{
 */
#define MS8607_CMD_PT_RST				(0x1E)
#define MS8607_CMD_PT_CONV_D1			(0x40)
#define MS8607_CMD_PT_CONV_D2			(0x50)
#define MS8607_CMD_PT_ADC_ADC_RD		(0x00)
#define MS8607_CMD_PT_PROM_RD			(0xA0)

#define MS8607_CMD_PT_PROM_MASK			(0x07)
#define MS8607_CMD_PT_PROM_SHFT			(0x01)

#define MS8607_CMD_H_RST				(0xFE)
#define MS8607_CMD_H_UR_RD				(0xE6)
#define MS8607_CMD_H_UR_WR				(0xE7)
#define MS8607_CMD_H_MEAS_HOLD			(0xE5)
#define MS8607_CMD_H_MEAS_NHOLD			(0xF5)

#define MS8607_CMD_H_UR_OSR_MSK			(0x81)

#ifdef __cplusplus
}
#endif

#endif /* MS8607_INTERNALS_H */
/** @} */
