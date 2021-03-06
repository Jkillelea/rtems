/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicMessage
 *
 * @brief This source file contains the implementation of
 *   rtems_message_queue_delete().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/messageimpl.h>
#include <rtems/rtems/attrimpl.h>

rtems_status_code rtems_message_queue_delete(
  rtems_id id
)
{
  Message_queue_Control *the_message_queue;
  Thread_queue_Context   queue_context;

  _Objects_Allocator_lock();
  the_message_queue = _Message_queue_Get( id, &queue_context );

  if ( the_message_queue == NULL ) {
    _Objects_Allocator_unlock();

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Message_queue_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  _CORE_message_queue_Acquire_critical(
    &the_message_queue->message_queue,
    &queue_context
  );

  _Objects_Close( &_Message_queue_Information, &the_message_queue->Object );

  _Thread_queue_Context_set_MP_callout(
    &queue_context,
    _Message_queue_MP_Send_object_was_deleted
  );
  _CORE_message_queue_Close(
    &the_message_queue->message_queue,
    &queue_context
  );

#if defined(RTEMS_MULTIPROCESSING)
  if ( the_message_queue->is_global ) {
    _Objects_MP_Close(
      &_Message_queue_Information,
      the_message_queue->Object.id
    );

    _Message_queue_MP_Send_process_packet(
      MESSAGE_QUEUE_MP_ANNOUNCE_DELETE,
      the_message_queue->Object.id,
      0,                         /* Not used */
      0
    );
  }
#endif

  _Message_queue_Free( the_message_queue );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
