/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicEventMP
 *
 * @brief This source file contains the implementation to support the Event
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

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/statesimpl.h>

/**
 *  The following enumerated type defines the list of
 *  remote event operations.
 */
typedef enum {
  EVENT_MP_SEND_REQUEST  =  0,
  EVENT_MP_SEND_RESPONSE =  1
}   Event_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote event operations.
 */
typedef struct {
  rtems_packet_prefix         Prefix;
  Event_MP_Remote_operations  operation;
  rtems_event_set             event_in;
}   Event_MP_Packet;

RTEMS_STATIC_ASSERT(
  sizeof(Event_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  Event_MP_Packet
);

static Event_MP_Packet *_Event_MP_Get_packet( Objects_Id id )
{
  if ( !_Thread_MP_Is_remote( id ) ) {
    return NULL;
  }

  return (Event_MP_Packet *) _MPCI_Get_packet();
}

rtems_status_code _Event_MP_Send(
  rtems_id        id,
  rtems_event_set event_in
)
{
  Event_MP_Packet *the_packet;
  Status_Control   status;

  the_packet = _Event_MP_Get_packet( id );
  if ( the_packet == NULL ) {
    return RTEMS_INVALID_ID;
  }

  the_packet->Prefix.the_class  = MP_PACKET_EVENT;
  the_packet->Prefix.length     = sizeof ( *the_packet );
  the_packet->Prefix.to_convert = sizeof ( *the_packet );
  the_packet->operation         = EVENT_MP_SEND_REQUEST;
  the_packet->Prefix.id         = id;
  the_packet->event_in          = event_in;

  status = _MPCI_Send_request_packet(
    _Objects_Get_node( id ),
    &the_packet->Prefix,
    STATES_READY
  );
  return _Status_Get( status );
}

static void _Event_MP_Send_response_packet (
  Event_MP_Remote_operations  operation,
  Thread_Control             *the_thread
)
{
  Event_MP_Packet *the_packet;

  switch ( operation ) {

    case EVENT_MP_SEND_RESPONSE:

      the_packet = ( Event_MP_Packet *) the_thread->receive_packet;

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

    case EVENT_MP_SEND_REQUEST:
      break;

  }
}

void _Event_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  Event_MP_Packet *the_packet;

  the_packet = (Event_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case EVENT_MP_SEND_REQUEST:

      the_packet->Prefix.return_code = rtems_event_send(
        the_packet->Prefix.id,
        the_packet->event_in
      );

      _Event_MP_Send_response_packet(
        EVENT_MP_SEND_RESPONSE,
        _Thread_Executing
      );
      break;

    case EVENT_MP_SEND_RESPONSE: {
      _MPCI_Process_response( the_packet_prefix );
      _MPCI_Return_packet( the_packet_prefix );

      break;
    }

  }
}

/*
 *  _Event_MP_Send_object_was_deleted
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/*
 *  _Event_MP_Send_extract_proxy
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/* end of file */
