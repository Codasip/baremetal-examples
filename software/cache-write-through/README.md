# cache-write-through

Demonstrates use of Codasip cache management API to configure cache policy.

The demo compares the time spent on the same number of memory writes with
cache write-through and write-back settings.

Results depend on cache configuration of the target and the length of test
data. Elapsed time is expected to be lower with the write-back setting.
This difference is not visible when using Codasip IA simulators.

The demo is only intended for targets with L31 core.
