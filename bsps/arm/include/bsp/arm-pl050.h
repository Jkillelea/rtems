/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM PL050 Support
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_PL050_H
#define LIBBSP_ARM_SHARED_ARM_PL050_H

#include <rtems/termiostypes.h>

#include <bsp/arm-pl050-regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  rtems_termios_device_context base;
  volatile pl050 *regs;
  rtems_vector_number irq;
  uint32_t initial_baud;
} arm_pl050_context;

extern const rtems_termios_device_handler arm_pl050_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_PL050_H */
