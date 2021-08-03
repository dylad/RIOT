#include "nrf_modem_os.h"
#include "nrf_errno.h"
#include "nrf_modem_platform.h"
#include "nrf_modem_limits.h"

#include <errno.h>

/* RIOT-OS specific headers */
#include "nrf_modem_riot.h"
#include "cpu.h"

nrfx_err_t nrfx_ipc_init(uint8_t irq_priority, nrfx_ipc_handler_t handler, void * p_context)
{
    (void) irq_priority;
    (void)handler;
    (void)p_context;
    printf("[nrf_modem_hal]:%s\n", __FUNCTION__);
    return 0;
}

void nrfx_ipc_uninit(void)
{
    printf("[nrf_modem_hal]:%s\n", __FUNCTION__);
}

void nrfx_ipc_config_load(const nrfx_ipc_config_t * p_config)
{
    (void)p_config;
    printf("[nrf_modem_hal]:%s\n", __FUNCTION__);
}