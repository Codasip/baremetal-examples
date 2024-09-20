# clic-priority

Demonstrates use of CLIC (Core-Local Interrupt Controller) API to configure
priority of external interrupts.

The demo uses a GPIO peripheral to trigger external interrupts. It configures
the order, in which the interrupts are handled by the priority.

Notice that the CLIC interrupt level is used to determine preemption (for nesting interrupts). 
In contrast, the interrupt priority does not affect preemption but is only used as a 
tie-breaker when there are multiple pending interrupts with the same interrupt level. 

The demo is only intended for targets with CLIC.
