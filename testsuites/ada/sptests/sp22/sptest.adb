-- SPDX-License-Identifier: BSD-2-Clause

--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 22 of the RTEMS
--  Single Processor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;
with RTEMS.CLOCK;
with RTEMS.TIMER;

package body SPTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEST_SUPPORT.ADA_TEST_BEGIN;

      TIME := ( 1988, 12, 31, 9, 0, 0, 0 );

      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

      SPTEST.TASK_NAME( 1 )  := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );
      SPTEST.TIMER_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'M', '1', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      TEXT_IO.PUT_LINE( "INIT - timer_create - creating timer 1" );
      RTEMS.TIMER.CREATE(
         SPTEST.TIMER_NAME( 1 ), 
         SPTEST.TIMER_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CREATE OF TM1" );
      TEXT_IO.PUT( "INIT - timer 1 has id (" );
      UNSIGNED32_IO.PUT( SPTEST.TIMER_ID( 1 ), WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
      
      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  DELAYED_RESUME
--

   procedure DELAYED_RESUME (
      IGNORED_ID      : in     RTEMS.ID;
      IGNORED_ADDRESS : in     RTEMS.ADDRESS
   ) is
      pragma Unreferenced(IGNORED_ID);
      pragma Unreferenced(IGNORED_ADDRESS);
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.RESUME( SPTEST.TASK_ID( 1 ), STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_RESUME OF SELF" );

   end DELAYED_RESUME;

-- 
--  PRINT_TIME
--

   procedure PRINT_TIME 
   is
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.CLOCK.GET_TOD( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET_TOD" );

      TEST_SUPPORT.PUT_NAME( 
         SPTEST.TASK_NAME( 1 ),
         FALSE
      );

      TEST_SUPPORT.PRINT_TIME( "- clock_get - ", TIME, "" );
      TEXT_IO.NEW_LINE;

   end PRINT_TIME;
   
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      TMID   : RTEMS.ID;
      TIME   : RTEMS.TIME_OF_DAY;
      STATUS : RTEMS.STATUS_CODES;
   begin

-- GET ID

      TEXT_IO.PUT_LINE( "TA1 - timer_ident - identing timer 1" );
      RTEMS.TIMER.IDENT( SPTEST.TIMER_NAME( 1 ), TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_IDENT OF TM1" );
      TEXT_IO.PUT( "TA1 - timer 1 has id (" );
      UNSIGNED32_IO.PUT( SPTEST.TIMER_ID( 1 ), WIDTH => 8, BASE => 16 );
      TEXT_IO.PUT_LINE( ")" );
   
-- AFTER WHICH IS ALLOWED TO FIRE

      SPTEST.PRINT_TIME;

      TEXT_IO.PUT_LINE( "TA1 - timer_after - timer 1 in 3 seconds" );
      RTEMS.TIMER.FIRE_AFTER( 
         TMID, 
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - task_suspend( SELF )" );
      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );
          
      SPTEST.PRINT_TIME;

-- AFTER WHICH IS RESET AND ALLOWED TO FIRE

      TEXT_IO.PUT_LINE( "TA1 - timer_after - timer 1 in 3 seconds" );
      RTEMS.TIMER.FIRE_AFTER( 
         TMID, 
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - 1 second" );
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      SPTEST.PRINT_TIME;

      TEXT_IO.PUT_LINE( "TA1 - timer_reset - timer 1" );
      RTEMS.TIMER.RESET( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_RESET" );
          
      TEXT_IO.PUT_LINE( "TA1 - task_suspend( SELF )" );
      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

      SPTEST.PRINT_TIME;

TEST_SUPPORT.PAUSE;

-- 
-- Reset the time since we do not know how long the user waited
-- before pressing <cr> at the pause.  This insures that the
-- actual output matches the screen.
--

      TIME := ( 1988, 12, 31, 9, 0, 7, 0 );

      RTEMS.CLOCK.SET( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_SET" );

-- after which is canceled

      TEXT_IO.PUT_LINE( "TA1 - timer_after - timer 1 in 3 seconds" );
      RTEMS.TIMER.FIRE_AFTER( 
         TMID, 
         3 * TEST_SUPPORT.TICKS_PER_SECOND,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_AFTER" );

      TEXT_IO.PUT_LINE( "TA1 - timer_cancel - timer 1" );
      RTEMS.TIMER.CANCEL( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );
          
-- when which is allowed to fire

      SPTEST.PRINT_TIME;

      RTEMS.CLOCK.GET_TOD( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET_TOD" );

      TIME.SECOND := TIME.SECOND + 3;

      TEXT_IO.PUT_LINE( "TA1 - timer_when - timer 1 in 3 seconds" );
      RTEMS.TIMER.FIRE_WHEN( 
         TMID, 
         TIME,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN" );

      TEXT_IO.PUT_LINE( "TA1 - task_suspend( SELF )" );
      RTEMS.TASKS.SUSPEND( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_SUSPEND" );

      SPTEST.PRINT_TIME;

-- when which is canceled

      RTEMS.CLOCK.GET_TOD( TIME, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "CLOCK_GET_TOD" );

      TIME.SECOND := TIME.SECOND + 3;

      TEXT_IO.PUT_LINE( "TA1 - timer_when - timer 1 in 3 seconds" );
      RTEMS.TIMER.FIRE_WHEN( 
         TMID, 
         TIME,
         SPTEST.DELAYED_RESUME'ACCESS,
         RTEMS.NULL_ADDRESS,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_FIRE_WHEN" );

      TEXT_IO.PUT_LINE( "TA1 - task_wake_after - 1 second" );
      RTEMS.TASKS.WAKE_AFTER( TEST_SUPPORT.TICKS_PER_SECOND, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER" );
          
      SPTEST.PRINT_TIME;

      TEXT_IO.PUT_LINE( "TA1 - timer_cancel - timer 1" );
      RTEMS.TIMER.CANCEL( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_CANCEL" );
          
-- delete
          
      TEXT_IO.PUT_LINE(
         "TA1 - task_wake_after - YIELD (only task at priority)"
      );
      RTEMS.TASKS.WAKE_AFTER( RTEMS.YIELD_PROCESSOR, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_WAKE_AFTER YIELD" );
          
      TEXT_IO.PUT_LINE( "TA1 - timer_delete - timer 1" );
      RTEMS.TIMER.DELETE( TMID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TIMER_DELETE" );
          
      TEST_SUPPORT.ADA_TEST_END;
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

end SPTEST;
