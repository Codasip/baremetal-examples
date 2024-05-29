# privilege-interrupts-delegated

Demonstrates delegation of interrupts to lower privilege modes.

Demo configures CLINT to periodically trigger interrupts, while the core cycles
in user mode. Timer interrupts get handled in machine mode, and every second
interrupt also triggers a supervisor software interrupt which is delegated
to the supervisor mode.

The demo is only intended for targets supporting supervisor and user modes.
