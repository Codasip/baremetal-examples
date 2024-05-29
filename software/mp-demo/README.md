# mp-demo

Demonstrates use of MP API to execute simple function on each hart.

The demo increments a global variable from each hart, and prints out
the result, which should be equal to number of harts on the given target.

The demo is only intended for targets implementing the A (atomic) extension.
