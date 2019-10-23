//
//  first.h
//  pa4
//
//  Alexander Goodkind
//  amg540
//

#ifndef first_h
#define first_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct _QueueNode {
    unsigned long tag;
    struct _QueueNode * next, * previous;
//    int valid;
} QueueNode;

typedef struct _CacheQueue {
    unsigned count;  // Number of filled frames
    QueueNode * head, * tail;
} CacheQueue;

CacheQueue * create_cache (void);
void increment_counter (char counter);
void read_cache (CacheQueue * cache, unsigned long address);
void free_cache (CacheQueue * cache);
void write_cache (CacheQueue * cache, unsigned long address);
int is_power_of_two (double num);
int is_set_available (CacheQueue * set);
int is_set_full (CacheQueue * set);
int is_set_empty (CacheQueue * set);
void dequeue (CacheQueue * set);
QueueNode * new_node (unsigned long tag_index);
void enqueue (CacheQueue * set, unsigned long tag_index);
int is_in_set (CacheQueue * set, unsigned long tag_index);
void prefetch_block(CacheQueue * set, unsigned long address);
QueueNode * get_node_pointer (CacheQueue * set, unsigned long tag);
void pop_node (CacheQueue * set, unsigned long tag);

#endif /* first_h */

