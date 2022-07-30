/*****************************************************************************
 * csim.c: Cache simulator main program
 * NAME:    Huang
 * DATE:    07/30/2022
 * WEBSITE: huangblog.com
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include "cachelab.h"

// global variables
int v = 0;    // Optional, verbose mode
int s;        // number of set index bits
int S;        // number of sets
int E;        // associativity, number of lines per set
int b;        // number of block bits, block size = 2^b
char t[100];  // name of the trace file

// answer variables
int hits = 0;
int misses = 0;
int evictions = 0;

typedef struct {
    int valid;
    int tag;
    int lru_counter;
} cache_line, **Cache;  // for naming

void parse_args(int, char **);
void print_usage();
Cache new_cache();
void free_cache(Cache);
void parse_trace(FILE *, Cache);
void update_cache(Cache, unsigned);
void update_counter(Cache);

/**
 * new_cache - create a new cache
 *
 * @return cache[S][E]
 */
Cache new_cache() {
    Cache cache = (Cache)malloc(S * sizeof(cache_line *));
    if (cache == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }
    for (int i = 0; i < S; i++) {
        cache[i] = (cache_line *)malloc(E * sizeof(cache_line));
        if (cache[i] == NULL) {
            printf("Error: malloc failed\n");
            exit(1);
        }
        for (int j = 0; j < E; j++) {
            cache[i][j].valid = 0;
            cache[i][j].tag = -1;
            cache[i][j].lru_counter = -1;
        }
    }
    return cache;
}

/**
 * free_cache - free the cache
 */
void free_cache(Cache cache) {
    for (int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);
}

/**
 * parse_trace - parse the trace file
 *
 * In fp, the format of each line is:
 *   [space]operatin address, size
 * - space: optional, exists in all besides operation I
 * - operation:
 *   - I: instruction load
 *   - L: data load
 *   - S: data store
 *   - M: data modify
 * - address: hexadecimal memory address
 * - size: number of bytes accessed by the operation
 */
void parse_trace(FILE *fp, Cache cache) {
    char operation;
    unsigned address;
    int size;

    while (fscanf(fp, " %c %x,%d", &operation, &address, &size) > 0) {
        if (operation != 'I' && v) {
            // verbose mode
            printf("%c %x,%d", operation, address, size);
        }
        switch (operation) {
            case 'I':
                break;
            case 'L':
                update_cache(cache, address);
                break;
            case 'M': // load, then store
                update_cache(cache, address);
            case 'S':
                update_cache(cache, address);
                break;
            default:
                printf("Error: unknown operation\n");
                exit(1);
        }
        if (operation != 'I' && v) {
            printf("\n");
        }
        update_counter(cache);
    }
}

/**
 * update_cache - update the cache
 *
 * 1. check if is hit
 * 2. check if there is an empty line
 * 3. evict the LRU line
 */
void update_cache(Cache cache, unsigned address) {
    int index = (address >> b) & ((1 << s) - 1);
    int tag = address >> (s + b);

    // check if is hit
    for (int i = 0; i < E; i++) {
        if (cache[index][i].valid && cache[index][i].tag == tag) {
            hits++;
            cache[index][i].lru_counter = 0;
            if (v) {
                printf(" hit");
            }
            return;
        }
    }
    misses++;
    if (v) {
        printf(" miss");
    }

    // check if there is an empty line
    for (int i = 0; i < E; i++) {
        if (!cache[index][i].valid) {
            cache[index][i].valid = 1;
            cache[index][i].tag = tag;
            cache[index][i].lru_counter = 0;
            return;
        }
    }
    evictions++;
    if (v) {
        printf(" eviction");
    }

    // evict the LRU line
    int evict_line = 0;
    int max_counter = -1;
    for (int i = 0; i < E; i++) {
        if (cache[index][i].lru_counter > max_counter) {
            max_counter = cache[index][i].lru_counter;
            evict_line = i;
        }
    }
    cache[index][evict_line].valid = 1;
    cache[index][evict_line].tag = tag;
    cache[index][evict_line].lru_counter = 0;
}

/**
 * update_counter - update the lru counter of the cache line
 */
void update_counter(Cache cache) {
    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            if (cache[i][j].valid == 1) {
                cache[i][j].lru_counter++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Parse the command line arguments.
    parse_args(argc, argv);

    // Initialize the cache.
    Cache cache = new_cache();

    // Parse the trace file.
    FILE *fp = fopen(t, "r");
    if (fp == NULL) {
        printf("Error: cannot open file %s\n", t);
        exit(1);
    }
    parse_trace(fp, cache);

    printSummary(hits, misses, evictions);

    free_cache(cache);
    fclose(fp);

    return 0;
}

/**
 * parse_args - Parses the command line arguments
 */
void parse_args(int argc, char *argv[]) {
    int opt = 0;
    while (-1 != (opt = getopt(argc, argv, "hvs:E:b:t:"))) {
        switch (opt) {
            case 'h':
                print_usage();
                exit(0);
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                S = 1 << s;
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(t, optarg);
                break;
            default:
                print_usage();
                exit(1);
        }
    }
    if (s <= 0 || E <= 0 || b <= 0 || strlen(t) == 0) {
        print_usage();
        exit(1);
    }
}

/**
 * print_usage - Prints the usage of the program
 */
void print_usage() {
    printf(
        "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
        "Options:\n"
        "  -h         Print this help message.\n"
        "  -v         Optional verbose flag.\n"
        "  -s <num>   Number of set index bits.\n"
        "  -E <num>   Number of lines per set.\n"
        "  -b <num>   Number of block bits.\n"
        "  -t <file>  Trace file.\n\n"
        "Examples:\n"
        "  linux> ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
        "  linux> ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n\n");
}
