/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphoreMP
 *
 * @brief This source file contains the implementation to support the Semaphore
 *   Manager in multiprocessing (MP) configurations.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/sysinit.h>

RTEMS_STATIC_ASSERT(
  sizeof(Semaphore_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Semaphore_MP_Packet
);

static Semaphore_MP_Packet *_Semaphore_MP_Get_packet( void )
{
  return (Semaphore_MP_Packet *) _MPCI_Get_packet();
}

void _Semaphore_MP_Send_process_packet (
  Semaphore_MP_Remote_operations  operation,
  Objects_Id                      semaphore_id,
  rtems_name                      name,
  Objects_Id                      proxy_id
)
{
  Semaphore_MP_Packet *the_packet;
  uint32_t             node;

  switch ( operation ) {

    case SEMAPHORE_MP_ANNOUNCE_CREATE:
    case SEMAPHORE_MP_ANNOUNCE_DELETE:
    case SEMAPHORE_MP_EXTRACT_PROXY:

      the_packet                    = _Semaphore_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_SEMAPHORE;
      the_packet->Prefix.length     = sizeof ( Semaphore_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Semaphore_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = semaphore_id;
      the_packet->name              = name;
      the_packet->proxy_id          = proxy_id;

      if ( operation == SEMAPHORE_MP_EXTRACT_PROXY )
         node = _Objects_Get_node( semaphore_id );
      else
         node = MPCI_ALL_NODES;

      _MPCI_Send_process_packet( node, &the_packet->Prefix );
      break;

    case SEMAPHORE_MP_OBTAIN_REQUEST:
    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_REQUEST:
    case SEMAPHORE_MP_RELEASE_RESPONSE:
      break;
  }
}

static rtems_status_code _Semaphore_MP_Send_request_packet(
  Objects_Id                     semaphore_id,
  rtems_option                   option_set,
  rtems_interval                 timeout,
  Semaphore_MP_Remote_operations operation
)
{
  Semaphore_MP_Packet *the_packet;
  Status_Control       status;

  if ( !_Semaphore_MP_Is_remote( semaphore_id ) ) {
    return RTEMS_INVALID_ID;
  }

  switch ( operation ) {

    case SEMAPHORE_MP_OBTAIN_REQUEST:
    case SEMAPHORE_MP_RELEASE_REQUEST:

      the_packet                    = _Semaphore_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_SEMAPHORE;
      the_packet->Prefix.length     = sizeof ( Semaphore_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( Semaphore_MP_Packet );
      if ( ! _Options_Is_no_wait(option_set))
          the_packet->Prefix.timeout = timeout;

      the_packet->operation         = operation;
      the_packet->Prefix.id         = semaphore_id;
      the_packet->option_set        = option_set;

      status = _MPCI_Send_request_packet(
        _Objects_Get_node( semaphore_id ),
        &the_packet->Prefix,
        STATES_WAITING_FOR_SEMAPHORE
      );
      return _Status_Get( status );

    case SEMAPHORE_MP_ANNOUNCE_CREATE:
    case SEMAPHORE_MP_ANNOUNCE_DELETE:
    case SEMAPHORE_MP_EXTRACT_PROXY:
    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_RESPONSE:
      break;

  }
  /*
   *  The following line is included to satisfy compilers which
   *  produce warnings when a function does not end with a return.
   */
  return RTEMS_SUCCESSFUL;
}

rtems_status_code _Semaphore_MP_Obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
)
{
  return _Semaphore_MP_Send_request_packet(
    id,
    option_set,
    timeout,
    SEMAPHORE_MP_OBTAIN_REQUEST
  );
}

rtems_status_code _Semaphore_MP_Release( rtems_id id )
{
  return _Semaphore_MP_Send_request_packet(
    id,
    0,
    MPCI_DEFAULT_TIMEOUT,
    SEMAPHORE_MP_RELEASE_REQUEST
  );
}

static void _Semaphore_MP_Send_response_packet (
  Semaphore_MP_Remote_operations  operation,
  Objects_Id                      semaphore_id,
  Thread_Control                 *the_thread
)
{
  Semaphore_MP_Packet *the_packet;

  switch ( operation ) {

    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_RESPONSE:

      the_packet = ( Semaphore_MP_Packet *) the_thread->receive_packet;

/*
 *  The packet being returned already contains the class, length, and
 *  to_convert fields, therefore they are not set in this routine.
 */
      the_packet->operation = operation;
      the_packet->Prefix.id = the_packet->Prefix.source_tid;

      _MPCI_Send_response_packet(
        _Objects_Get_node( the_packet->Prefix.source_tid ),
        &the_packet->Prefix
      );
      break;

    case SEMAPHORE_MP_ANNOUNCE_CREATE:
    case SEMAPHORE_MP_ANNOUNCE_DELETE:
    case SEMAPHORE_MP_EXTRACT_PROXY:
    case SEMAPHORE_MP_OBTAIN_REQUEST:
    case SEMAPHORE_MP_RELEASE_REQUEST:
      break;

  }
}

static void _Semaphore_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  Semaphore_MP_Packet *the_packet;
  Thread_Control      *the_thread;

  the_packet = (Semaphore_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case SEMAPHORE_MP_ANNOUNCE_CREATE:

      _Objects_MP_Allocate_and_open(
        &_Semaphore_Information,
        the_packet->name,
        the_packet->Prefix.id,
        true
      );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_ANNOUNCE_DELETE:

      _Objects_MP_Close( &_Semaphore_Information, the_packet->Prefix.id );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_EXTRACT_PROXY:

      the_thread = _Thread_MP_Find_proxy( the_packet->proxy_id );

      if ( the_thread != NULL ) {
        _Thread_queue_Extract( the_thread );
      }

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_OBTAIN_REQUEST:

      the_packet->Prefix.return_code = rtems_semaphore_obtain(
        the_packet->Prefix.id,
        the_packet->option_set,
        the_packet->Prefix.timeout
      );

      if ( the_packet->Prefix.return_code != RTEMS_PROXY_BLOCKING )
        _Semaphore_MP_Send_response_packet(
           SEMAPHORE_MP_OBTAIN_RESPONSE,
           the_packet->Prefix.id,
           _Thread_Executing
        );
      break;

    case SEMAPHORE_MP_OBTAIN_RESPONSE:
    case SEMAPHORE_MP_RELEASE_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case SEMAPHORE_MP_RELEASE_REQUEST:

      the_packet->Prefix.return_code = rtems_semaphore_release(
        the_packet->Prefix.id
      );

      _Semaphore_MP_Send_response_packet(
        SEMAPHORE_MP_RELEASE_RESPONSE,
        the_packet->Prefix.id,
        _Thread_Executing
      );
      break;
  }
}

void _Semaphore_MP_Send_object_was_deleted (
  Thread_Control *the_proxy,
  Objects_Id      mp_id
)
{
  the_proxy->receive_packet->return_code = RTEMS_OBJECT_WAS_DELETED;

  _Semaphore_MP_Send_response_packet(
    SEMAPHORE_MP_OBTAIN_RESPONSE,
    mp_id,
    the_proxy
  );

}

void _Semaphore_MP_Send_extract_proxy (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  _Semaphore_MP_Send_process_packet(
    SEMAPHORE_MP_EXTRACT_PROXY,
    id,
    (rtems_name) 0,
    the_thread->Object.id
  );

}

void  _Semaphore_Core_mutex_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

  _Semaphore_MP_Send_response_packet(
     SEMAPHORE_MP_OBTAIN_RESPONSE,
     id,
     the_thread
   );
}

void  _Semaphore_Core_semaphore_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

  _Semaphore_MP_Send_response_packet(
     SEMAPHORE_MP_OBTAIN_RESPONSE,
     id,
     the_thread
   );
}

static void _Semaphore_MP_Initialize( void )
{
  _MPCI_Register_packet_processor(
    MP_PACKET_SEMAPHORE,
    _Semaphore_MP_Process_packet
  );
}

RTEMS_SYSINIT_ITEM(
  _Semaphore_MP_Initialize,
  RTEMS_SYSINIT_CLASSIC_SEMAPHORE_MP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
