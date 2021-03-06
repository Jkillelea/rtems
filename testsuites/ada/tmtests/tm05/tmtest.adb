-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 5 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions
--  are met:
--  1. Redistributions of source code must retain the above copyright
--     notice, this list of conditions and the following disclaimer.
--  2. Redistributions in binary form must reproduce the above copyright
--     notice, this list of conditions and the following disclaimer in the
--     documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
--

with INTERFACES; use INTERFACES;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIMER_DRIVER;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      TMTEST.TEST_INIT;

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      TASK_ENTRY : RTEMS.TASKS.ENTRY_POINT;
      PRIORITY   : RTEMS.TASKS.PRIORITY;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      PRIORITY := 250;

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASKS.CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            PRIORITY, 
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            TMTEST.TASK_ID( INDEX ),
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );

         PRIORITY := PRIORITY - 1;

         if INDEX = 0 then
            TASK_ENTRY := TMTEST.LOW_TASK'ACCESS;
         elsif INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.HIGH_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASKS.START( TMTEST.TASK_ID( INDEX ), TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

   end TEST_INIT;

-- 
--  HIGH_TASK
--

   procedure HIGH_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS       : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPENT" );

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_RESUME causing preempt",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_RESUME
      );

      TEST_SUPPORT.ADA_TEST_END;
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end HIGH_TASK;

-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS       : RTEMS.STATUS_CODES;
   begin
 
      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

      TMTEST.TASK_INDEX := TMTEST.TASK_INDEX + 1;
      RTEMS.TASKS.RESUME( TMTEST.TASK_ID( TMTEST.TASK_INDEX ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );
 
   end MIDDLE_TASKS;

-- 
--  LOW_TASK
--

   procedure LOW_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS   : RTEMS.STATUS_CODES;
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_SUSPEND self",
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_SUSPEND
      );
   
      TMTEST.TASK_INDEX := 1;
      TIMER_DRIVER.INITIALIZE;
      RTEMS.TASKS.RESUME( TMTEST.TASK_ID( TMTEST.TASK_INDEX ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME" );

   end LOW_TASK;

end TMTEST;
