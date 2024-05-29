# cache-counter-demo

Demonstrates use of Codasip cache management API to retrieve information 
about cache hits and misses.

The demo compares the number of cache hits and misses when accessing memory
sequentially and randomly.

Results depend on cache configuration of the target and the length of test
data. However, both tests should conduct the same number of memory accesses,
and the sum of hits and misses should be the same. Sequential access is
expected to produce fewer misses than random access.

The demo is only intended for targets with L31 core.
