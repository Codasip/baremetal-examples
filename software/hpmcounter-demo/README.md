# hpmcounter-demo

Demonstrates use of, performance counter API to retrieve information about cpu events.

The demo compares hardware counters in sequence pairs. Sequence of loads that can
finish independently is compared with loads that depend on the result of the previous
one, and sequence of loads each depending on the results of the previous one,
intermitten with other instructions or grouped together.

Number of instructions executed for each tested pair should be the same.
Number of cycles spent in the sequences should, on the other hand be higher for
the less optimal sequence from the pair. In addition a hardware performance event,
capturing the loss of performance, was selected, for each core separately.