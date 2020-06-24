/**
 * @file
 * 
 * @ingroup RTEMSAPIClassic
 *
 * the Public Interface to the RTEMS Classic API
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_H
#define _RTEMS_H

/**
 * @defgroup RTEMSAPIClassic Classic
 *
 * @ingroup RTEMSAPI
 * 
 * @brief RTEMS Classic API definitions and modules.
 */
/** @{ */

#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#include <rtems/config.h>
#include <rtems/init.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/barrier.h>
#include <rtems/rtems/cache.h>
#include <rtems/rtems/clock.h>
#include <rtems/extension.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/object.h>
#include <rtems/rtems/part.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/io.h>
#include <rtems/fatal.h>
#include <rtems/rtems/ratemon.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/mp.h>
#endif

#include <rtems/rtems/support.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An MPCI must support packets of at least this size.
 */
#define RTEMS_MINIMUM_PACKET_SIZE  MP_PACKET_MINIMUM_PACKET_SIZE

/**
 * @brief Defines the count of @c uint32_t numbers in a packet which must be
 * converted to native format in a heterogeneous system.
 *
 * In packets longer than this value, some of the extra data may be a user
 * message buffer which is not automatically endian swapped.
 */
#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
