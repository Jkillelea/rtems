/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC24XX
 *
 * @brief UART 3 probe.
 */

/*
 * Copyright (c) 2011-2014 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/io.h>

bool lpc24xx_uart_probe_3(rtems_termios_device_context *context)
{
  static const lpc24xx_pin_range pins [] = {
    LPC24XX_PIN_UART_3_TXD_P0_0,
    LPC24XX_PIN_UART_3_RXD_P0_1,
    LPC24XX_PIN_TERMINAL
  };

  lpc24xx_module_enable(LPC24XX_MODULE_UART_3, LPC24XX_MODULE_PCLK_DEFAULT);
  lpc24xx_pin_config(&pins [0], LPC24XX_PIN_SET_FUNCTION);

  return ns16550_probe(context);
}
