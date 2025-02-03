/*
 * Copyright (C) 2025 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam_common
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "board.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"
#include "ztimer.h"
#include <sys/reent.h>

#define ENABLE_DEBUG        0
#include "debug.h"

typedef enum {
    I2C_READY,
    I2C_LOST_ARBITRATION,
    I2C_NACK,
    I2C_XFER,
    I2C_TIMEOUT,
    I2C_DATA_AVAIL,
}i2c_state_t;

typedef struct {
    mutex_t dev_lock;
    mutex_t isr_lock;
    i2c_state_t state;
} i2c_internal_t;

static i2c_internal_t _i2c_dev[I2C_NUMOF];

/**
 * @brief   Shortcut for accessing the used I2C SERCOM device
 */
static inline Twi *bus(i2c_t dev)
{
    return i2c_config[dev].dev;
}

/**
 * @brief   Setup I2C bus speed
 */
static void _setup_bus_speed(i2c_t dev, i2c_speed_t speed)
{
    uint32_t clk_div;
    uint32_t cl_div;

    /* Ensure speed is supported */
    assert(speed);

    /* Loop through all possible clk_div value to find a match
     * From SAM3x datasheet:
     * Tlow = ((cl_div * (2 ^ clk_div)) + 4) * T_mck
     * where T_mck is the period of peripheral clock (aka CLOCK_CORECLOCK)
     * Tlow is half the period of the desired I2C frequency clock output
     * considering that we want a 50/50 duty cycle on the clock */

    for (clk_div=0; clk_div<8; clk_div++) {
        cl_div = (((CLOCK_CORECLOCK / 2 * speed) - 4) / ( 1 << clk_div));
        if (cl_div < 255) {
            bus(dev)->TWI_CWGR = TWI_CWGR_CLDIV(cl_div) | TWI_CWGR_CHDIV(cl_div) |
                                 TWI_CWGR_CKDIV(clk_div);
        }
    }
    assert(0);
}

void _wait_for_interrupts(i2c_t dev)
{
    /* Wait for interrupts events */
    mutex_lock(&_i2c_dev[dev].isr_lock);
}

void _read_data(i2c_t dev, uint8_t *data, size_t len)
{
    size_t count = 0;

    /* Enable interrupts */

    /* Start read sequence */
    
    /* Read data buffer. */
    while (len--) {
        /* Wait for a byte to be available or an error to occur */
        _wait_for_interrupts(dev);
        if (_i2c_dev[dev].state == I2C_DATA_AVAIL) {
            data[count] = bus(dev)->TWI_RHR & TWI_RHR_RXDATA_Msk;
            count++;
        }
    }
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* initialize device and irq locks */
    mutex_init(&_i2c_dev[dev].dev_lock);
    mutex_init_locked(&_i2c_dev[dev].isr_lock);

    /* Enable peripheral clock through PMC */
    PMC->PMC_PCER0 = (1 << i2c_config[dev].pmc_id);

    /* Enable I2C Master Mode */
    bus(dev)->TWI_CR = TWI_CR_MSEN;

    /* Setup bus speed rate */
    _setup_bus_speed(dev, i2c_config[dev].speed);

    /* Initialize GPIOs for I2C operation */
    gpio_init_mux(i2c_config[dev].sda_pin, i2c_config[dev].mux);
    gpio_init_mux(i2c_config[dev].scl_pin, i2c_config[dev].mux);

    /* Setup needed interrupts */
    bus(dev)->TWI_IER = TWI_IER_ARBLST | TWI_IER_RXRDY | TWI_IER_TXRDY;

    /* Enable associated NVIC */
    NVIC_EnableIRQ(i2c_config[dev].irqn);

    /* Disable clock now that bus is setup */
    PMC->PMC_PCDR0 = (1 << i2c_config[dev].pmc_id);
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_lock(&_i2c_dev[dev].dev_lock);

    /* Enable peripheral clock through PMC */
    PMC->PMC_PCER0 = (1 << i2c_config[dev].pmc_id);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_unlock(&_i2c_dev[dev].dev_lock);

    /* Disable peripheral clock through PMC */
    PMC->PMC_PCDR0 = (1 << i2c_config[dev].pmc_id);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    uint16_t count = 0;

    assert(dev < I2C_NUMOF);

    /* 10-bits addressing not supported for now */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    /* Send I2C Start command if needed */
    if (!(flags & I2C_NOSTART)) {
        /* Send Start sequence */
        bus(dev)->TWI_CR = TWI_CR_START;
    }

    /* Set device address and direction */
    bus(dev)->TWI_MMR = TWI_MMR_DADR(addr) | TWI_MMR_MREAD;

    /* Read data buffer. */
#if 0
    while (len--) {
        /* Wait for a byte to be available to read */
        while (!(bus(dev)->TWI_SR & TWI_SR_RXRDY)) {}
        getdata[count] = bus(dev)->TWI_RHR & TWI_RHR_RXDATA_Msk;
        count++;
    }
#endif

    /* Send I2C Stop command if needed */
    if (!(flags & I2C_NOSTOP)) {
        /* Send Start sequence */
        bus(dev)->TWI_CR = TWI_CR_STOP;
    }

    return count;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    (void)dev;
    (void)addr;
    (void)data;
    (void)len;
    (void)flags;
    return 0;
}


#ifdef MODULE_PERIPH_I2C_RECONFIGURE
void i2c_init_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    gpio_init_mux(i2c_config[dev].sda_pin, i2c_config[dev].mux);
    gpio_init_mux(i2c_config[dev].scl_pin, i2c_config[dev].mux);
}

void i2c_deinit_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
}
#endif

static inline void isr_handler(int num)
{
    Twi *dev = i2c_config[num].dev;

    /* Get IRQs status from register (clear on read) */
    uint32_t irq_status = dev->TWI_SR;

    if (irq_status & TWI_SR_ARBLST)
    {
        /* Arbitration lost on bus */
        _i2c_dev[num].state = I2C_LOST_ARBITRATION;
        irq_status &= ~TWI_SR_ARBLST;
    }

    if (irq_status & TWI_SR_RXRDY)
    {
        /* New RX data available */
        _i2c_dev[num].state = I2C_DATA_AVAIL;
        irq_status &= ~TWI_SR_RXRDY;
    }

    if (irq_status & TWI_IER_TXRDY)
    {
        /* TX data has been transferred */
    }

    if (irq_status) {
        DEBUG("[i2c]: Unhandled interrupts 0x%lx\n", irq_status);
    }

    mutex_unlock(&_i2c_dev[num].isr_lock);
    cortexm_isr_end();
}

#ifdef I2C_0_ISR
void I2C_0_ISR(void)
{
    isr_handler(0);
}
#endif

#ifdef I2C_1_ISR
void I2C_1_ISR(void)
{
    isr_handler(1);
}
#endif
