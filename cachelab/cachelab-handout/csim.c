#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef struct cache_block
{
    int valid;
    int tag;
    int time_tamp;
} Cache_block;

typedef struct cache
{
    int S;//sets
    int E;//blocks
    int B;//bytes
    Cache_block **block;
} Cache;

Cache * cache;
int miss_count = 0;
int verbose = 0;
int eviction_count = 0;
int hit_count = 0;
char t[50];
extern char *optarg;
void initCache(int s, int E, int b) {
    int S = 1 << s;
    int B = 1 << b;
    cache = (Cache *) malloc(sizeof(Cache));
    cache->S = S;
    cache->E = E;
    cache->B = B;
    cache->block = (Cache_block **)malloc(sizeof(Cache_block *) * S);
    for (int i = 0; i < S; i++)
    {
        
        cache->block[i] = (Cache_block *)malloc(sizeof(Cache_block) * E);
        for (int j = 0; j < E; j++)
        {
            cache->block[i][j].valid = 0;
            cache->block[i][j].tag = -1;
            cache->block[i][j].time_tamp = 0;
        }
    }
}
int getIndex(int op_offset, int op_tag) {
    for(int i = 0; i < cache->E; i ++) {
        if(cache->block[op_offset][i].valid && cache->block[op_offset][i].tag == op_tag) {
            return i;
        }
    }
    return -1;
}

int isFull(int op_set) {
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->block[op_set][i].valid == 0)
        {
            return i;
        }
    }
    return -1;
}

void update(int i, int op_set, int op_tag)
{
    cache->block[op_set][i].valid = 1;
    cache->block[op_set][i].tag = op_tag;
    for (int k = 0; k < cache->E; k++)
    {
        if (cache->block[op_set][k].valid == 1)
        {
            cache->block[op_set][k].time_tamp++;
        }
    }
    cache->block[op_set][i].time_tamp = 0;
}

int findLRU(int op_set) {
    int max_index = 0;
    int max_time = 0;
    for(int i = 0; i < cache->E; i ++) {
        if(cache->block[op_set][i].valid == 1) {
            if(max_time < cache->block[op_set][i].time_tamp) {
                max_time = cache->block[op_set][i].time_tamp;
                max_index = i;
            }
        }
    }
    return max_index;
}

void updateInfo(int op_set, int op_tag) {
    int index = getIndex(op_set, op_tag);
    if (index == -1)
    {
        miss_count++;
        if (verbose)
        {
            printf("miss ");
        }
        int i = isFull(op_set);
        if (i == -1)
        {
            eviction_count++;
            if (verbose)
            {
                printf("eviction\n");
            }
            i = findLRU(op_set);
        }
        update(i, op_set, op_tag);
    }
    else {
        hit_count++;
        if (verbose)
        {
            printf("hit\n");
        }
        update(index, op_set, op_tag);
    }
}

void freeCache() {
    free(cache);
    cache = NULL;
}

void getTrace(int s, int E, int b) {
    FILE *p_file;
    p_file = fopen(t, "r");
    if (p_file == NULL)
    {
        exit(-1);
    }
    char identifier;
    unsigned address;
    int size;

    while (fscanf(p_file, " %c %x,%d", &identifier, &address, &size) > 0)
    {
        int op_tag = address >> (s + b);
        int op_set = (address >> b) & ((unsigned)(-1) >> (8 * sizeof(unsigned) - s));
        switch (identifier)
        {
            case 'M':
                updateInfo(op_set, op_tag);
                updateInfo(op_set, op_tag);
                break;
            case 'L':
                updateInfo(op_set, op_tag);
                break;
            case 'S':
                updateInfo(op_set, op_tag);
                break;
        }
    }
    fclose(p_file);
}



int main(int argc, char** argv)
{
    char opt;
    int s, E, b;

    while (-1 != (opt = getopt(argc, argv, "hvs:E:b:t:")))
    {
        switch (opt)
        {
            case 'h':
                printf("input format wrong");
                exit(0);
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
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
                printf("input format wrong!");
                exit(-1);
        }
    }
    initCache(s, E, b);
    getTrace(s, E, b);
    freeCache();
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
