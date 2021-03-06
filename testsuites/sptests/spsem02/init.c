/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 Gedare Bloom.
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

#include <rtems.h>

#include <stdio.h>
#include "tmacros.h"

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_INIT
#include <rtems/confdefs.h>

const char rtems_test_name[] = "SPSEM 2";

rtems_task Task01(rtems_task_argument ignored);
rtems_task Task02(rtems_task_argument ignored);
rtems_task Task03(rtems_task_argument ignored);
rtems_task Init(rtems_task_argument ignored);

static int getprio(void)
{
  rtems_status_code status;
  rtems_task_priority pri;

  status = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &pri);
  directive_failed( status, "rtems_task_set_priority");
  return (int)pri;
}

rtems_id   Task_id[3];
rtems_name Task_name[3];

rtems_id   sem_id[2];
rtems_name sem_name[2];

rtems_task Init(rtems_task_argument ignored)
{
  rtems_status_code status;
  rtems_attribute sem_attr;

  TEST_BEGIN();

  sem_attr = RTEMS_INHERIT_PRIORITY | RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY;

  sem_name[0] = rtems_build_name( 'S','0',' ',' ');
  status = rtems_semaphore_create(
    sem_name[0],
    1,
    sem_attr,
    0,
    &sem_id[0]
  );
  directive_failed( status, "rtems_semaphore_create of S0");
  printf("init: S0 created\n");

  sem_name[1] = rtems_build_name( 'S','1',' ',' ');
  status = rtems_semaphore_create(
    sem_name[1],
    1,
    sem_attr,
    0,
    &sem_id[1]
  );
  directive_failed( status, "rtems_semaphore_create of S1");
  printf("init: S1 created\n");

  Task_name[0] = rtems_build_name( 'T','A','0','1');
  status = rtems_task_create(
    Task_name[0],
    36,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[0]
  );
  directive_failed( status, "rtems_task_create of TA01");
  printf("init: TA01 created with priority 36\n");

  Task_name[1] = rtems_build_name( 'T','A','0','2');
  status = rtems_task_create(
    Task_name[1],
    34,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[1]
  );
  directive_failed( status , "rtems_task_create of TA02\n");
  printf("init: TA02 created with priority 34\n");

  Task_name[2] = rtems_build_name( 'T','A','0','3');
  status = rtems_task_create(
    Task_name[2],
    32,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[2]
  );
  directive_failed( status , "rtems_task_create of TA03\n");
  printf("init: TA03 created with priority 32\n");

  status = rtems_task_start( Task_id[0], Task01, 0);
  directive_failed( status, "rtems_task_start of TA01");

  rtems_task_exit();
}

/* Task01 starts with priority 36 */
rtems_task Task01(rtems_task_argument ignored)
{
  rtems_status_code status;
  printf("TA01: started with priority %d\n", getprio());

  status = rtems_semaphore_obtain( sem_id[0], RTEMS_WAIT, 0 );
  directive_failed( status, "rtems_semaphore_obtain of S0\n");
  printf("TA01: priority %d, holding S0\n", getprio());

  status = rtems_semaphore_obtain( sem_id[1], RTEMS_WAIT, 0 );
  directive_failed( status, "rtems_semaphore_obtain of S1");
  printf("TA01: priority %d, holding S0, S1\n", getprio());

  /* Start Task 2 (TA02) with priority 34. It will run immediately. */
  status = rtems_task_start( Task_id[1], Task02, 0);
  directive_failed( status, "rtems_task_start of TA02\n");

  /* Start Task 3 (TA03) with priority 32. It will run immediately. */
  status = rtems_task_start( Task_id[2], Task03, 0);
  directive_failed( status, "rtems_task_start of TA03\n");
  printf("TA01: priority %d, holding S0, S1\n", getprio());

  status = rtems_semaphore_release(sem_id[1]);
  directive_failed( status, "rtems_semaphore_release of S1\n");
  printf("TA01: priority %d, holding S0\n", getprio());

  status = rtems_semaphore_release(sem_id[0]);
  directive_failed( status, "rtems_semaphore_release of S0\n");
  printf("TA01: priority %d\n", getprio());

  printf("TA01: exiting\n");
  TEST_END();

  rtems_test_exit(0);
}

/* TA02 starts at Task02 with priority 34 */
rtems_task Task02(rtems_task_argument ignored)
{
  rtems_status_code status;

  printf("TA02: started with priority %d\n", getprio());

  /* Obtain S1, which should be held by TA01 by now */
  status = rtems_semaphore_obtain( sem_id[1], RTEMS_WAIT, 0 );
  directive_failed( status, " rtems_semaphore_obtain S1");
  printf("TA02: priority %d, holding S1\n", getprio());

  printf("TA02: suspending\n");
  status = rtems_task_suspend( RTEMS_SELF);
  directive_failed( status, "rtems_task_suspend TA02");
}

/* Task03 starts with priority 32 */
rtems_task Task03(rtems_task_argument ignored)
{
  rtems_status_code status;
  printf("TA03: started with priority %d\n", getprio());

  status = rtems_semaphore_obtain( sem_id[0], RTEMS_WAIT, 0 );
  directive_failed( status, "rtems_semaphore_obtain of S0\n");
  printf("TA03: priority %d, holding S0\n", getprio());

  status = rtems_semaphore_release(sem_id[0]);
  directive_failed( status, "rtems_semaphore_release of S0\n");
  printf("TA03: priority %d\n", getprio());

  printf("TA03: exiting\n");
  rtems_task_exit();
}
