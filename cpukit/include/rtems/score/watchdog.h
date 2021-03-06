/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreWatchdog which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_WATCHDOG_H
#define _RTEMS_SCORE_WATCHDOG_H

#include <rtems/score/basedefs.h>
#include <rtems/score/chain.h>
#include <rtems/score/rbtree.h>

struct Per_CPU_Control;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreWatchdog Watchdog Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Watchdog Handler implementation.
 *
 * This handler encapsulates functionality related to the scheduling of
 * watchdog functions to be called at specific times in the future.  It
 * provides mechanisms which can be used to initialize and manipulate watchdog
 * timers.
 *
 * @note This handler does not have anything to do with hardware watchdog
 *       timers.
 *
 * @{
 */

typedef struct Watchdog_Control Watchdog_Control;

/**
 *  @brief Return type from a Watchdog Service Routine.
 *
 *  This type defines the return type from a Watchdog Service Routine.
 */
typedef void Watchdog_Service_routine;

/**
 *  @brief Pointer to a watchdog service routine.
 *
 *  This type define a pointer to a watchdog service routine.
 */
typedef Watchdog_Service_routine
  ( *Watchdog_Service_routine_entry )( Watchdog_Control * );

/**
 * @brief The watchdog header to manage scheduled watchdogs.
 */
typedef struct {
  /**
   * @brief Red-black tree of scheduled watchdogs sorted by expiration time.
   */
  RBTree_Control Watchdogs;

  /**
   * @brief The scheduled watchdog with the earliest expiration time or NULL in
   * case no watchdog is scheduled.
   */
  RBTree_Node *first;
} Watchdog_Header;

/**
 *  @brief The control block used to manage each watchdog timer.
 *
 *  The following record defines the control block used
 *  to manage each watchdog timer.
 */
struct Watchdog_Control {
  /**
   * @brief Nodes for the watchdog.
   */
  union {
    /**
     * @brief this field is a red-black tree node structure and allows this to
     * be placed on a red-black tree used to manage the scheduled watchdogs.
     */
    RBTree_Node RBTree;

    /**
     * @brief this field is a chain node structure and allows this to be placed
     * on a chain used to manage pending watchdogs by the timer server.
     */
    Chain_Node Chain;
  } Node;

#if defined(RTEMS_SMP)
  /** @brief This field references the processor of this watchdog control. */
  struct Per_CPU_Control *cpu;
#endif

  /** @brief This field is the function to invoke. */
  Watchdog_Service_routine_entry routine;

  /** @brief This field is the expiration time point. */
  uint64_t expire;
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
