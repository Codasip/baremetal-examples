# plic-priority

Demonstrates use of PLIC (Platform Level Interrupt Controller) API to configure
priority of external interrupts and their priority threshold.

The demo uses a GPIO peripheral to trigger external interrupts. It configures
which of the interrupts will be enabled by the threshold, and the order,
in which the interrupts are handled by the priority.

The demo is only intended for targets with PLIC.
