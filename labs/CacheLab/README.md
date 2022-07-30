# CacheLab

This lab will help you understand the impact that cache memories can have on the
performance of your C programs.

* [WriteUp](http://csapp.cs.cmu.edu/3e/cachelab.pdf)
* [Cache Lab Implementation and Blocking](http://www.cs.cmu.edu/afs/cs/academic/class/15213-f15/www/recitations/rec07.pdf)

## Part A

> Implement a cache simulator. (NOT a cache, do not store contents)

* LRU: Least Recently Used
* cache: `cache_line[S][E]`
    * S = 2<sup>s</sup> (number of sets)
    * E (associativity)

```bash
cache_line:
########################################
# valid #      tag       # lru_counter #
########################################

address:
########################################
# tag #        index          # offset #
########################################
```

1. Parse the command line arguments.
2. Initialize the cache.
3. Parse the trace file.
    * check if is hit
    * check if there is an empty line
    * evict the LRU line

It is easy to implement the cache simulator.
