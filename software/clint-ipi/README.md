# clint-ipi

Demonstrates use of CLINT API to issue IPIs.

The demo issues an IPI from each hart to the next available hart,
and prints out once the interrupt is triggered.

## Example Output
On a MP4 system you can expect a correctly synchronised output in terms of the strings,
but the order of the harts' output can be different on each run.

```
Welcome to the CLINT-IPI demo!

Hart3: sending IPI to hart0
Hart0: sending IPI to hart1
Hart2: sending IPI to hart3
Hart1: sending IPI to hart2
Hart1: MSIP interrupt received!
Hart0: MSIP interrupt received!
Hart2: MSIP interrupt received!
Hart3: MSIP interrupt received!
Bye.
Info: _exit: Exited normally.
```
