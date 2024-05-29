# wfi-demo

The demo compares the number of cycles and instructions executed while waiting
for ten timer interrupts. First, in a run without simple busy wait, then with
wfi instructions present.

Number of executed instructions is expected to be significantly lower when wfi
instructions are used. Number of cycles elapsed depends on core implementation.
