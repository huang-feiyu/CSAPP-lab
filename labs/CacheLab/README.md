# CacheLab

This lab will help you understand the impact that cache memories can have on
the performance of your C programs.

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

## Part B

> Optimizing Matrix Transpose.

Given cache:
* all: 1 KB = 2<sup>10</sup> bytes => 32 blocks, 256 ints
* Directly mapped: E=1
* block size: 32 bytes; b=5 => 16 ints
* Sets: 32 sets; s=5

What we need to do is to write a function `transpose_submit()` to transpose a
matrix.

<strong>*</strong> 32 by 32

Use Blocking strategy, the Block size is 8 by 8. So we just need to do stuff
for an 8 by 8 matrix.

The naive solution is simple:
```c
for (i = 0; i < N; i += 8) {
    for (j = 0; j < M; j += 8) {
        for (int k = 0; k < 8; k++) {
            for (int l = 0; l < 8; l++) {
                B[j + l][i + k] = A[i + k][j + l];
            }
        }
    }
}
```

We got 343 misses, but the goal is less than 300 misses - We can do better.
While reading or writing to matrix, the cache will move the row to its specific
cache_line. We can use local variables to store the row, then while writing to
B, it is OK to replace the cache line.

288 misses, got full score.

<strong>*</strong> 64 by 64

8 points if misses < 1300, 0 points if misses > 2000

8 by 8 Block will cause a lot of misses, because cache reads continuous memory.
We can use 4 by 4 block.

And there is 4 more variables we can use. So every time we need to process
2 lines(8 by 4), it can optimize a little.

1646 misses, got 4.3/8.0 points.

<strong>*</strong> 61 by 67

10 points if misses < 2000, 0 points if misses > 3000

61 by 67 matrix is a bit tricky, because it is not a square matrix.

Split it to <s>48 by 48</s> 60 * 64 square matrix, and then we can use the same
idea as above. To process the rest, we just need to use two loop to process the
rest.

2041 misses, roughly the full score.

---

End:

```
Cache Lab summary:
                        Points   Max pts      Misses
Csim correctness          27.0        27
Trans perf 32x32           8.0         8         288
Trans perf 64x64           4.2         8        1636
Trans perf 61x67           9.6        10        2041
          Total points    48.8        53
```
