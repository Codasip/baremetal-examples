# rdtime

This demo showcases the `rdtime` instruction to measure elapsed time in user mode.
If `rdtime` is unsupported, emulation code is run in machine mode, using CLINT.

## Example Output (from FPGA Platforms)
On a core (e.g. A730) that has the `rdtime` instruction:

```
Welcome to the rdtime demo!

Using rdtime to meassure 100ms delay:
Time before    : 12278736617
Time after     : 12291236842
Time elapsed   : 12500225

Using rdtime   : Native
Average rdtime : 2 cycles
```

On a core (e.g. L31) that does not have the `rdtime` instruction and so it is trapped (as it's an
illegal instruction) and emulated in this example code:

```
Welcome to the rdtime demo!

Using rdtime to meassure 100ms delay:
Time before    : 182943882
Time after     : 185445022
Time elapsed   : 2501140

Using rdtime   : Emulation
Average rdtime : 665 cycles

Bye.
Info: _exit: Exited normally.
```
