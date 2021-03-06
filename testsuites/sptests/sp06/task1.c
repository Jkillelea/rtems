/* SPDX-License-Identifier: BSD-2-Clause */

/*  Task_1
 *
 *  This task serves as a test task.  Verifies that a task can restart
 *  both itself and other tasks.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_1(
  rtems_task_argument   argument
)
{
  uint32_t    pass;
  rtems_status_code status;

  puts( "TA1 - is beginning to run" );

  pass = argument + 1;

  if ( pass == 5 ) {
    TEST_END();
    rtems_test_exit( 0 );
  }

  if ( pass == 1 ) {
    puts( "TA1 - restarting TA3" );
    status = rtems_task_restart( Task_id[ 3 ], Restart_argument );
    directive_failed( status, "rtems_task_restart of TA3" );

    status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );
  }

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_restart - restarting TA2" );
  status = rtems_task_restart( Task_id[ 2 ], Restart_argument );
  directive_failed( status, "rtems_task_restart TA2" );

  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_restart - restarting self" );
  status = rtems_task_restart( Task_id[ 1 ], pass );
  directive_failed( status, "rtems_task_restart of RTEMS_SELF" );
}
