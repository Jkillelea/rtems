/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-cp15-start.h>

BSP_START_TEXT_SECTION void bsp_start_hook_0(void)
{
}

BSP_START_TEXT_SECTION void bsp_start_hook_1(void)
{
  bsp_start_copy_sections();
  beagle_setup_mmu_and_cache();
  bsp_start_clear_bss();
}
