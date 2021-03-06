/* SPDX-License-Identifier: BSD-2-Clause */

/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
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

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

void Fill_buffer(
  char  source[],
  long *buffer
);

void Put_buffer(
  void *buffer
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

TEST_EXTERN rtems_id   Queue_id[ 4 ];        /* array of queue ids */
TEST_EXTERN rtems_name Queue_name[ 4 ];      /* array of queue names */

/* test configuration */

#define MESSAGE_SIZE (sizeof(long) * 4)  /* must be multiple of sizeof(long) */

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               4
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     10
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

/*
 *  Created in init.c:
 *    Q1 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) (1600)
 *    Q2 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 10, MESSAGE_SIZE )   (160)
 *    Q3 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) (1600)
 *
 *  Q1 and Q2 deleted in task1.c.
 *
 *  Q1 recreated in task1.c:
 *    Q1 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, 20 )           (2000)
 *
 *  Q1 deleted again in task1.c.
 *
 *  Q1 repeatedly created and deleted for 2 messages of 1-1030 bytes
 *  in length. Account for peak usage:
 *    Q1 - CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 2, 1030 )           (2060)
 *
 *  Because each message requires memory for the message plus two
 *  pointers to place the buffer on lists, it is easier to just
 *  allocate memory for all the message queues. But we can safely
 *  ignore Q2 and the last instance of Q1 since enough memory is
 *  free when the third instance of Q1 is created.
 */
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
   /* Q1 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, MESSAGE_SIZE ) + \
   /* Q2 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 10, MESSAGE_SIZE ) + \
   /* Q3 is statically allocated */ \
   /* Q1 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 100, 20 ) + \
   /* Q1 */ CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 2,  1030 )

#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* end of include file */
