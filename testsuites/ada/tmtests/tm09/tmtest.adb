-- SPDX-License-Identifier: BSD-2-Clause

--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 9 of the RTEMS
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
with TIME_TEST_SUPPORT;
with TIMER_DRIVER;
with RTEMS.MESSAGE_QUEUE;

package body TMTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TASK_ID : RTEMS.ID;
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      RTEMS.TASKS.CREATE( 
         1,
         128, 
         4096, 
         RTEMS.DEFAULT_OPTIONS,
         RTEMS.DEFAULT_ATTRIBUTES,
         TASK_ID,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE" );

      RTEMS.TASKS.START( TASK_ID, TMTEST.TEST_TASK'ACCESS, 0, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  TEST_TASK
--

   procedure TEST_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS       : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;
         RTEMS.MESSAGE_QUEUE.CREATE(
            1,
            TIME_TEST_SUPPORT.OPERATION_COUNT,
            16,
            RTEMS.DEFAULT_OPTIONS,
            TMTEST.QUEUE_ID,
            STATUS
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_CREATE",
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_CREATE
      );

      TMTEST.QUEUE_TEST;

      TIMER_DRIVER.INITIALIZE;
         RTEMS.MESSAGE_QUEUE.DELETE(
            TMTEST.QUEUE_ID,
            STATUS
         );
      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "MESSAGE_QUEUE_DELETE" );
      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_DELETE",
         TMTEST.END_TIME, 
         1, 
         0,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_DELETE
      );

      TEST_SUPPORT.ADA_TEST_END;
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TEST_TASK;

-- 
--  QUEUE_TEST
--

   procedure QUEUE_TEST
   is
      SEND_LOOP_TIME    : RTEMS.UNSIGNED32;
      URGENT_LOOP_TIME  : RTEMS.UNSIGNED32;
      RECEIVE_LOOP_TIME : RTEMS.UNSIGNED32;
      SEND_TIME         : RTEMS.UNSIGNED32;
      URGENT_TIME       : RTEMS.UNSIGNED32;
      RECEIVE_TIME      : RTEMS.UNSIGNED32;
      EMPTY_FLUSH_TIME  : RTEMS.UNSIGNED32;
      FLUSH_TIME        : RTEMS.UNSIGNED32;
      FLUSH_COUNT       : RTEMS.UNSIGNED32;
      EMPTY_FLUSH_COUNT : RTEMS.UNSIGNED32;
      BUFFER            : TMTEST.BUFFER;
      BUFFER_POINTER    : RTEMS.ADDRESS;
      MESSAGE_SIZE      : RTEMS.Size := 0;
      STATUS            : RTEMS.STATUS_CODES;
   begin

      SEND_LOOP_TIME    := 0;
      URGENT_LOOP_TIME  := 0;
      RECEIVE_LOOP_TIME := 0;
      SEND_TIME         := 0;
      URGENT_TIME       := 0;
      RECEIVE_TIME      := 0;
      EMPTY_FLUSH_TIME  := 0;
      FLUSH_TIME        := 0;
      FLUSH_COUNT       := 0;
      EMPTY_FLUSH_COUNT := 0;

      BUFFER_POINTER := BUFFER'ADDRESS;

      for ITERATIONS in 1 .. TIME_TEST_SUPPORT.ITERATION_COUNT
      loop

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               TIMER_DRIVER.EMPTY_FUNCTION;
            end loop;
         SEND_LOOP_TIME := SEND_LOOP_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               TIMER_DRIVER.EMPTY_FUNCTION;
            end loop;
         URGENT_LOOP_TIME := URGENT_LOOP_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               TIMER_DRIVER.EMPTY_FUNCTION;
            end loop;
         RECEIVE_LOOP_TIME := RECEIVE_LOOP_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               RTEMS.MESSAGE_QUEUE.SEND( 
                  TMTEST.QUEUE_ID,
                  BUFFER_POINTER,
                  16,
                  STATUS
               );
            end loop;
         SEND_TIME := SEND_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               RTEMS.MESSAGE_QUEUE.RECEIVE( 
                  TMTEST.QUEUE_ID,
                  BUFFER_POINTER,
                  RTEMS.DEFAULT_OPTIONS,
                  RTEMS.NO_TIMEOUT,
                  MESSAGE_SIZE,
                  STATUS
               );
            end loop;
         RECEIVE_TIME := RECEIVE_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               RTEMS.MESSAGE_QUEUE.URGENT( 
                  TMTEST.QUEUE_ID,
                  BUFFER_POINTER,
                  16,
                  STATUS
               );
            end loop;
         URGENT_TIME := URGENT_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            for INDEX in 1 .. TIME_TEST_SUPPORT.OPERATION_COUNT
            loop
               RTEMS.MESSAGE_QUEUE.RECEIVE( 
                  TMTEST.QUEUE_ID,
                  BUFFER_POINTER,
                  RTEMS.DEFAULT_OPTIONS,
                  RTEMS.NO_TIMEOUT,
                  MESSAGE_SIZE,
                  STATUS
               );
            end loop;
         RECEIVE_TIME := RECEIVE_TIME + TIMER_DRIVER.READ_TIMER;

         TIMER_DRIVER.INITIALIZE;
            RTEMS.MESSAGE_QUEUE.FLUSH( 
               TMTEST.QUEUE_ID,
               EMPTY_FLUSH_COUNT,
               STATUS
            );
         EMPTY_FLUSH_TIME := EMPTY_FLUSH_TIME + TIMER_DRIVER.READ_TIMER;

         -- send one message to flush
         RTEMS.MESSAGE_QUEUE.SEND( 
            TMTEST.QUEUE_ID, 
            BUFFER_POINTER, 
            16,
            STATUS
         );
         TIMER_DRIVER.INITIALIZE;
            RTEMS.MESSAGE_QUEUE.FLUSH( 
               TMTEST.QUEUE_ID,
               FLUSH_COUNT,
               STATUS
            );
         FLUSH_TIME := FLUSH_TIME + TIMER_DRIVER.READ_TIMER;

      end loop;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "MESSAGE_QUEUE_SEND (no tasks waiting)",
         SEND_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT *
            TIME_TEST_SUPPORT.ITERATION_COUNT,
         SEND_LOOP_TIME,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_SEND
      );

      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_URGENT (no tasks waiting)",
         URGENT_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT *
            TIME_TEST_SUPPORT.ITERATION_COUNT,
         URGENT_LOOP_TIME,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_URGENT
      );

      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_RECEIVE (messages available)",
         RECEIVE_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT *
            TIME_TEST_SUPPORT.ITERATION_COUNT * 2,
         RECEIVE_LOOP_TIME * 2,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_RECEIVE
      );

      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_FLUSH (empty queue)",
         EMPTY_FLUSH_TIME, 
         TIME_TEST_SUPPORT.ITERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_FLUSH
      );

      TIME_TEST_SUPPORT.PUT_TIME(
         "MESSAGE_QUEUE_FLUSH (messages flushed)",
         FLUSH_TIME, 
         TIME_TEST_SUPPORT.ITERATION_COUNT,
         0,
         RTEMS_CALLING_OVERHEAD.MESSAGE_QUEUE_FLUSH
      );

   end QUEUE_TEST;

end TMTEST;
