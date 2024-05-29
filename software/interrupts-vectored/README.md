# interrupts-vectored

Demonstrates how to setup a simple handler table for interrupt handling
in vectored mode.

The demo first configures the handlers, and then triggers an invalid
instruction exception, and a timer interrupt, which each get handled
by a handler function.
