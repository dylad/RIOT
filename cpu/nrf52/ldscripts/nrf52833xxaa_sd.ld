/* Linker script to configure memory regions. */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x1c000, LENGTH = 0x64000
  RAM (rwx) :  ORIGIN = 0x20001198, LENGTH = 0x1ee68
}


INCLUDE "nrf_common.ld"
