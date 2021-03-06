/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Support Call to function Enables Locking of Mutex Object 
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

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/posixapi.h>

Status_Control _POSIX_Mutex_Seize_slow(
  POSIX_Mutex_Control           *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  const struct timespec         *abstime,
  Thread_queue_Context          *queue_context
)
{
  if ( (uintptr_t) abstime != POSIX_MUTEX_ABSTIME_TRY_LOCK ) {
    _Thread_queue_Context_set_thread_state(
      queue_context,
      STATES_WAITING_FOR_MUTEX
    );
    _Thread_queue_Context_set_deadlock_callout(
      queue_context,
      _Thread_queue_Deadlock_status
    );
    _Thread_queue_Enqueue(
      &the_mutex->Recursive.Mutex.Queue.Queue,
      operations,
      executing,
      queue_context
    );
    return _Thread_Wait_get_status( executing );
  } else {
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_UNAVAILABLE;
  }
}

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t              *mutex,
  const struct timespec        *abstime,
  Thread_queue_Enqueue_callout  enqueue_callout
)
{
  POSIX_Mutex_Control  *the_mutex;
  unsigned long         flags;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Status_Control        status;

  the_mutex = _POSIX_Mutex_Get( mutex );
  POSIX_MUTEX_VALIDATE_OBJECT( the_mutex, flags );

  executing = _POSIX_Mutex_Acquire( the_mutex, &queue_context );
  _Thread_queue_Context_set_enqueue_callout( &queue_context, enqueue_callout);
  _Thread_queue_Context_set_timeout_argument( &queue_context, abstime, true );

  switch ( _POSIX_Mutex_Get_protocol( flags ) ) {
    case POSIX_MUTEX_PRIORITY_CEILING:
      status = _POSIX_Mutex_Ceiling_seize(
        the_mutex,
        flags,
        executing,
        abstime,
        &queue_context
      );
      break;
    case POSIX_MUTEX_NO_PROTOCOL:
      status = _POSIX_Mutex_Seize(
        the_mutex,
        flags,
        POSIX_MUTEX_NO_PROTOCOL_TQ_OPERATIONS,
        executing,
        abstime,
        &queue_context
      );
      break;
    default:
      _Assert(
        _POSIX_Mutex_Get_protocol( flags ) == POSIX_MUTEX_PRIORITY_INHERIT
      );
      status = _POSIX_Mutex_Seize(
        the_mutex,
        flags,
        POSIX_MUTEX_PRIORITY_INHERIT_TQ_OPERATIONS,
        executing,
        abstime,
        &queue_context
      );
      break;
  }

  return _POSIX_Get_error( status );
}
