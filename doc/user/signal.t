@c
@c  COPYRIGHT (c) 1996.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifinfo
@node Signal Manager, Signal Manager Introduction, EVENT_RECEIVE - Receive event condition, Top
@end ifinfo
@chapter Signal Manager
@ifinfo
@menu
* Signal Manager Introduction::
* Signal Manager Background::
* Signal Manager Operations::
* Signal Manager Directives::
@end menu
@end ifinfo

@ifinfo
@node Signal Manager Introduction, Signal Manager Background, Signal Manager, Signal Manager
@end ifinfo
@section Introduction

The signal manager provides the capabilities required
for asynchronous communication.  The directives provided by the
signal manager are:

@itemize @bullet
@item @code{signal_catch} - Establish an ASR
@item @code{signal_send} - Send signal set to a task
@end itemize

@ifinfo
@node Signal Manager Background, Signal Manager Definitions, Signal Manager Introduction, Signal Manager
@end ifinfo
@section Background
@ifinfo
@menu
* Signal Manager Definitions::
* A Comparison of ASRs and ISRs::
* Building a Signal Set::
* Building an ASR's Mode::
@end menu
@end ifinfo

@ifinfo
@node Signal Manager Definitions, A Comparison of ASRs and ISRs, Signal Manager Background, Signal Manager Background
@end ifinfo
@subsection Signal Manager Definitions

The signal manager allows a task to optionally define
an asynchronous signal routine (ASR).  An ASR is to a task what
an ISR is to an application's set of tasks.  When the processor
is interrupted, the execution of an application is also
interrupted and an ISR is given control.  Similarly, when a
signal is sent to a task, that task's execution path will be
"interrupted" by the ASR.  Sending a signal to a task has no
effect on the receiving task's current execution state.

A signal flag is used by a task (or ISR) to inform
another task of the occurrence of a significant situation.
Thirty-two signal flags are associated with each task.  A
collection of one or more signals is referred to as a signal
set.  A signal set is posted when it is directed (or sent) to a
task. A pending signal is a signal that has been sent to a task
with a valid ASR, but has not been processed by that task's ASR.


@ifinfo
@node A Comparison of ASRs and ISRs, Building a Signal Set, Signal Manager Definitions, Signal Manager Background
@end ifinfo
@subsection A Comparison of ASRs and ISRs

The format of an ASR is similar to that of an ISR
with the following exceptions:

@itemize @bullet
@item ISRs are scheduled by the processor hardware.  ASRs are
scheduled by RTEMS.

@item ISRs do not execute in the context of a task and may
invoke only a subset of directives.  ASRs execute in the context
of a task and may execute any directive.

@item When an ISR is invoked, it is passed the vector number
as its argument.  When an ASR is invoked, it is passed the
signal set as its argument.

@item An ASR has a task mode which can be different from that
of the task.  An ISR does not execute as a task and, as a
result, does not have a task mode.
@end itemize

@ifinfo
@node Building a Signal Set, Building an ASR's Mode, A Comparison of ASRs and ISRs, Signal Manager Background
@end ifinfo
@subsection Building a Signal Set

A signal set is built by a bitwise OR of the desired
signals.  The set of valid signals is @code{SIGNAL_0} through
@code{SIGNAL_31}.  If a signal is not explicitly specified in the
signal set, then it is not present.  Signal values are
specifically designed to be mutually exclusive, therefore
bitwise OR and addition operations are equivalent as long as
each signal appears exactly once in the component list.

This example demonstrates the signal parameter used
when sending the signal set consisting of 
@code{SIGNAL_6}, @code{SIGNAL_15}, and @code{SIGNAL_31}.  
The signal parameter provided to the signal_send directive should be
@code{SIGNAL_6 @value{OR} SIGNAL_15 @value{OR} SIGNAL_31}.

@ifinfo
@node Building an ASR's Mode, Signal Manager Operations, Building a Signal Set, Signal Manager Background
@end ifinfo
@subsection Building an ASR's Mode

In general, an ASR's mode is built by a bitwise OR of
the desired mode components.  The set of valid mode components
is the same as those allowed with the task_create and task_mode
directives.  A complete list of mode options is provided in the
following table:

@itemize @bullet
@item PREEMPT is masked by PREEMPT_MASK and enables preemption
@item NO_PREEMPT is masked by PREEMPT_MASK and disables preemption
@item NO_TIMESLICE is masked by TIMESLICE_MASK and disables timeslicing
@item TIMESLICE is masked by TIMESLICE_MASK and enables timeslicing
@item ASR is masked by ASR_MASK and enables ASR processing
@item NO_ASR is masked by ASR_MASK and disables ASR processing
@item INTERRUPT_LEVEL(0) is masked by INTERRUPT_MASK and enables all interrupts
@item INTERRUPT_LEVEL(n) is masked by INTERRUPT_MASK and sets interrupts level n
@end itemize

Mode values are specifically designed to be mutually
exclusive, therefore bitwise OR and addition operations are
equivalent as long as each mode appears exactly once in the
component list.  A mode component listed as a default is not
required to appear in the mode list, although it is a good
programming practice to specify default components.  If all
defaults are desired, the mode DEFAULT_MODES should be specified
on this call.

This example demonstrates the mode parameter used
with the signal_catch to establish an ASR which executes at
interrupt level three and is non-preemptible.  The mode should
be set to
@code{INTERRUPT_LEVEL(3) @value{OR} NO_PREEMPT} to indicate the
desired processor mode and interrupt level.

@ifinfo
@node Signal Manager Operations, Establishing an ASR, Building an ASR's Mode, Signal Manager
@end ifinfo
@section Operations
@ifinfo
@menu
* Establishing an ASR::
* Sending a Signal Set::
* Processing an ASR::
@end menu
@end ifinfo

@ifinfo
@node Establishing an ASR, Sending a Signal Set, Signal Manager Operations, Signal Manager Operations
@end ifinfo
@subsection Establishing an ASR

The signal_catch directive establishes an ASR for the
calling task.  The address of the ASR and its execution mode are
specified to this directive.  The ASR's mode is distinct from
the task's mode.  For example, the task may allow preemption,
while that task's ASR may have preemption disabled.  Until a
task calls signal_catch the first time, its ASR is invalid, and
no signal sets can be sent to the task.

A task may invalidate its ASR and discard all pending
signals by calling signal_catch with a value of NULL for the
ASR's address.  When a task's ASR is invalid, new signal sets
sent to this task are discarded.

A task may disable ASR processing (NO_ASR) via the
task_mode directive.  When a task's ASR is disabled, the signals
sent to it are left pending to be processed later when the ASR
is enabled.

Any directive that can be called from a task can also
be called from an ASR.  A task is only allowed one active ASR.
Thus, each call to signal_catch replaces the previous one.

Normally, signal processing is disabled for the ASR's
execution mode, but if signal processing is enabled for the ASR,
the ASR must be reentrant.

@ifinfo
@node Sending a Signal Set, Processing an ASR, Establishing an ASR, Signal Manager Operations
@end ifinfo
@subsection Sending a Signal Set

The signal_send directive allows both tasks and ISRs
to send signals to a target task.  The target task and a set of
signals are specified to the signal_send directive.  The sending
of a signal to a task has no effect on the execution state of
that task.  If the task is not the currently running task, then
the signals are left pending and processed by the task's ASR the
next time the task is dispatched to run.  The ASR is executed
immediately before the task is dispatched.  If the currently
running task sends a signal to itself or is sent a signal from
an ISR, its ASR is immediately dispatched to run provided signal
processing is enabled.

If an ASR with signals enabled is preempted by
another task or an ISR and a new signal set is sent, then a new
copy of the ASR will be invoked, nesting the preempted ASR.
Upon completion of processing the new signal set, control will
return to the preempted ASR.  In this situation, the ASR must be
reentrant.

Like events, identical signals sent to a task are not
queued.  In other words, sending the same signal multiple times
to a task (without any intermediate signal processing occurring
for the task), has the same result as sending that signal to
that task once.

@ifinfo
@node Processing an ASR, Signal Manager Directives, Sending a Signal Set, Signal Manager Operations
@end ifinfo
@subsection Processing an ASR

Asynchronous signals were designed to provide the
capability to generate software interrupts.  The processing of
software interrupts parallels that of hardware interrupts.  As a
result, the differences between the formats of ASRs and ISRs is
limited to the meaning of the single argument passed to an ASR.
The ASR should have the following calling sequence and adhere to
@value{LANGUAGE} calling conventions:

@ifset is-C
@example
rtems_asr user_routine(
  rtems_signal_set signals
);
@end example
@end ifset

@ifset is-Ada
@example
procedure User_Routine (
  Signals : in     RTEMS.Signal_Set
);
@end example
@end ifset

When the ASR returns to RTEMS the mode and execution
path of the interrupted task (or ASR) is restored to the context
prior to entering the ASR.

@ifinfo
@node Signal Manager Directives, SIGNAL_CATCH - Establish an ASR, Processing an ASR, Signal Manager
@end ifinfo
@section Directives
@ifinfo
@menu
* SIGNAL_CATCH - Establish an ASR::
* SIGNAL_SEND - Send signal set to a task::
@end menu
@end ifinfo

This section details the signal manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@ifinfo
@node SIGNAL_CATCH - Establish an ASR, SIGNAL_SEND - Send signal set to a task, Signal Manager Directives, Signal Manager Directives
@end ifinfo
@subsection SIGNAL_CATCH - Establish an ASR

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_signal_catch(
  rtems_asr_entry  asr_handler,
  rtems_mode mode
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Signal_Catch (
   ASR_Handler : in     RTEMS.ASR_Handler;
   Mode_Set    : in     RTEMS.Mode;
   Result      :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - always successful

@subheading DESCRIPTION:

This directive establishes an asynchronous signal
routine (ASR) for the calling task.  The asr_handler parameter
specifies the entry point of the ASR.  If asr_handler is NULL,
the ASR for the calling task is invalidated and all pending
signals are cleared.  Any signals sent to a task with an invalid
ASR are discarded.  The mode parameter specifies the execution
mode for the ASR.  This execution mode supersedes the task's
execution mode while the ASR is executing.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

The following task mode constants are defined by RTEMS:

@itemize @bullet
@item PREEMPT is masked by PREEMPT_MASK and enables preemption
@item NO_PREEMPT is masked by PREEMPT_MASK and disables preemption
@item NO_TIMESLICE is masked by TIMESLICE_MASK and disables timeslicing
@item TIMESLICE is masked by TIMESLICE_MASK and enables timeslicing
@item ASR is masked by ASR_MASK and enables ASR processing
@item NO_ASR is masked by ASR_MASK and disables ASR processing
@item INTERRUPT_LEVEL(0) is masked by INTERRUPT_MASK and enables all interrupts
@item INTERRUPT_LEVEL(n) is masked by INTERRUPT_MASK and sets interrupts level n
@end itemize

@page
@ifinfo
@node SIGNAL_SEND - Send signal set to a task, Partition Manager, SIGNAL_CATCH - Establish an ASR, Signal Manager Directives
@end ifinfo
@subsection SIGNAL_SEND - Send signal set to a task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
rtems_status_code rtems_signal_send(
  rtems_id         id,
  rtems_signal_set signal_set
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Signal_Send (
   ID         : in     RTEMS.ID;
   Signal_Set : in     RTEMS.Signal_Set;
   Result     :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{SUCCESSFUL} - signal sent successfully@*
@code{INVALID_ID} - task id invalid@*
@code{NOT_DEFINED} - ASR invalid

@subheading DESCRIPTION:

This directive sends a signal set to the task
specified in id.  The signal_set parameter contains the signal
set to be sent to the task.

If a caller sends a signal set to a task with an
invalid ASR, then an error code is returned to the caller.  If a
caller sends a signal set to a task whose ASR is valid but
disabled, then the signal set will be caught and left pending
for the ASR to process when it is enabled. If a caller sends a
signal set to a task with an ASR that is both valid and enabled,
then the signal set is caught and the ASR will execute the next
time the task is dispatched to run.

@subheading NOTES:

Sending a signal set to a task has no effect on that
task's state.  If a signal set is sent to a blocked task, then
the task will remain blocked and the signals will be processed
when the task becomes the running task.

Sending a signal set to a global task which does not
reside on the local node will generate a request telling the
remote node to send the signal set to the specified task.

