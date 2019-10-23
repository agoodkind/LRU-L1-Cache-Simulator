//
//  first.c
//  pa4
//
//  Alexander Goodkind
//  amg540
//

#include "first.h"
#undef PA4_DEBUG

unsigned long B, C, E, S;
int b, c, s, e, /*t,*/ prefetch, cache_misses, cache_hits, memory_reads, memory_writes, pf_cache_misses, pf_cache_hits, pf_memory_reads, pf_memory_writes;
char associativity_level;

int main(int argc, char * argv[]) {
    
    // ./first <cache size> <associativity> <cache policy> <block size> <trace file>
    
    if (argc != 6) {
        printf("error");
        return 0;
    }
    
    /*block size*/
    B = atoi(argv[4]); // b ex 4 so 2^4=16, b = 4
    
    /*cache size*/
    C = atoi(argv[1]); // ex: 32 * 1024 = 32KB = 2^15, c = 15
    
    if (is_power_of_two(B) && is_power_of_two(C)) {
        b = log2(B);
        c = log2(C);
    } else {
        printf("error");
        return 0;
    }
    
    char * associativity = argv[2]; // assoc:n, largest being 32, ex. 4
    char * cache_policy = argv[3]; // lru
    
    if (strcmp(associativity, "direct") == 0) { // direct mapped
        E = 1; // E = 2^0 = 1
        S = C / B;
        associativity_level = 'd';
    } else if (associativity[5] == ':') { // assoc:n
        unsigned long assoc_num = 0;
        sscanf(associativity, "assoc:%ld", &assoc_num); // E = assoc level
        if (is_power_of_two(assoc_num)) {
            E = assoc_num;
            e = log2(E);
        } else {
            printf("error");
            return 0;
        }
        
        associativity_level = 'n';
        
        S = C / B / E;
    } else {
        // fully associative
        // S = 1
        // S*E = C / B
        // S = 1 = 1*E = C/B
        // E = C / B
        S = 1;
        E = C / B;
        
        associativity_level = 'a';
    }
    
    FILE * trace_file = fopen(argv[5], "r");
    
    s = (int) log2(S);
    e = (int) log2(E);
    //    t = 48 - b - s;
    
    //    printf("%d", strcmp(cache_policy, "lru"));
    
    if (trace_file == NULL || e < 0 || e > 48 || c < 0 || c > 48 || b < 0 || b > 48 || s < 0 || s > 48 || strcmp(cache_policy, "lru")) { // input validation
        printf("error");
        return 0;
    }
#ifdef PA4_DEBUG
    printf("B: %lu\n", B);
    printf("C: %lu\n", C);
    printf("E: %lu\n", E);
    printf("S: %lu\n", S);
    printf("b: %d\n", b);
    printf("c: %d\n", c);
    printf("e: %d\n", e);
    printf("s: %d\n", s);
#endif

    CacheQueue * cache = NULL;  // the cache to be used
    cache = create_cache();
    
    CacheQueue * pf_cache = NULL;
    pf_cache = create_cache();
    
    // printf("\nprefetch: %d\n", prefetch);
    // read
    // write
    
    unsigned long address;
    char mode;
    char input[48];
    
    while (fgets(input, 40, trace_file)) {
        
        if (input[0] == '#') {
            break;
        } else {
            sscanf(input,"%*x: %c %lx\n", &mode, &address);
            
            if (mode == 'R') {
                prefetch = 0;
                read_cache(cache, address); // no prefetch
                prefetch = 1;
                read_cache(pf_cache, address); // with prefetch
            } else if (mode == 'W') {
                prefetch = 0;
                write_cache(cache, address);
                prefetch = 1;
                write_cache(pf_cache, address);
            }
        }
        
    }
    
    fclose(trace_file);
    
    printf("no-prefetch\n");
    printf("Memory reads: %d\n", memory_reads);
    printf("Memory writes: %d\n", memory_writes);
    printf("Cache hits: %d\n", cache_hits);
    printf("Cache misses: %d\n", cache_misses);
    printf("with-prefetch\n");
    printf("Memory reads: %d\n", pf_memory_reads);
    printf("Memory writes: %d\n", pf_memory_writes);
    printf("Cache hits: %d\n", pf_cache_hits);
    printf("Cache misses: %d\n", pf_cache_misses);
    
    free_cache(cache); // needed
    free_cache(pf_cache);
    

    
    return 0;
}

CacheQueue * create_cache(void) {
    CacheQueue * cache = (CacheQueue *) calloc(S, sizeof(CacheQueue));
    
    int i;
    
    for (i = 0; i < S; i++) {
        cache[i].head = NULL;
        cache[i].tail = NULL;
        cache[i].count = 0;
    }
    
    return cache;
}

void read_cache(CacheQueue * cache, unsigned long address) {
    // need tag, set index, block index
    // use bit operations to get and store them
    unsigned long set_index = 0;
    //unsigned long block_index = 0;
    unsigned long tag = 0;
    
    //block_index = address & ((1 << b) - 1);
    tag = address >> (s + b);
    
    //    printf("%llu %llu %llu %llu", set_index, block_index, tag_index, i);
    
    if (s) {
        set_index = (address & ((1 << (s + b)) - 1)) >> b;
    }
    
    
    // when set_index == 0, cache == &cache[set_index]
    
    CacheQueue * set = &cache[set_index];
    
#ifdef PA4_DEBUG
    //set->count == 2 || address == 0xbf8ef49c
    NULL;
#endif
    
#ifdef PA4_DEBUG
    printf("=============BEFORE=============\n");
    printf("is_in_set set_%lx: %d\n", set_index, is_in_set(set, tag));
    printf("address: %lx\n", address);
    printf("set: %lx\n", set_index);
    printf("tag: %lx\n", tag);
    printf("full set: %d\n", is_set_full(set));
    printf("empty set: %d\n", is_set_empty(set));
    printf("non-full set: %d\n", is_set_available(set));
    printf("set count: %d\n", set->count);
    printf("hit: %d\n", cache_hits);
    printf("pf_hit: %d\n", pf_cache_hits);
    printf("miss: %d\n", pf_cache_misses);
    printf("==========================\n");
#endif
    
    if (is_in_set(set, tag)) {
        
        if (associativity_level != 'd' || E == 1) {
            pop_node(set, tag);
            enqueue(set, tag);
            
        } // don't do anything for direct mapped
        
        increment_counter('h'); // move to back of queue
        
    } else { // miss
        
        enqueue(set, tag);
        
        if (prefetch) {
            prefetch_block(cache, address);
        }
        
        increment_counter('m');
        increment_counter('r');
    }
    
#ifdef PA4_DEBUG
    printf("=============AFTER=============\n");
    printf("is_in_set set_%lx: %d\n", set_index, is_in_set(set, tag));
    printf("address: %lx\n", address);
    printf("set: %lx\n", set_index);
    printf("tag: %lx\n", tag);
    printf("full set: %d\n", is_set_full(set));
    printf("empty set: %d\n", is_set_empty(set));
    printf("non-full set: %d\n", is_set_available(set));
    printf("set count: %d\n", set->count);
    printf("hit: %d\n", cache_hits);
    printf("miss: %d\n", cache_misses);
    printf("pf_hit: %d\n", pf_cache_hits);
    printf("miss: %d\n", pf_cache_misses);
    printf("==========================\n");
#endif

}

void write_cache(CacheQueue * cache, unsigned long address) {
    // if write miss: the block is first read from memory (one memory read), and then followed by a memory write.
    increment_counter('w');
    read_cache(cache, address);
}

void free_cache(CacheQueue * cache) {
    int i;
    
    for (i = 0; i < S; i++) {
        if (!is_set_empty(&cache[i])) {
            QueueNode * current = cache[i].head; // just start at head and go until NULL
            QueueNode * next = NULL;
            while (current != NULL) {
                next = current->next;
                free(current);
                current = next;
            }
        }
    }
    
    free(cache);
}

int is_power_of_two(double num) { // returns int cast of num if proper power of 2, else returns 0
    return round(log2(num)) == log2(num);
}

void increment_counter(char counter) {
    switch(counter) {
        case 'w':
            if (prefetch) {
                pf_memory_writes++;
            } else {
                memory_writes++;
            }
            break;
        case 'r':
            if (prefetch) {
                pf_memory_reads++;
            } else {
                memory_reads++;
            }
            break;
        case 'h':
            if (prefetch) {
                pf_cache_hits++;
            } else {
                cache_hits++;
            }
            break;
        case 'm':
            if (prefetch) {
                pf_cache_misses++;
            } else {
                cache_misses++;
            }
            break;
        default:
            break;
    }
}

int is_set_available(CacheQueue * set) {
    // checks if set has space in it
    return set->count < E;
}

int is_set_empty(CacheQueue * set) {
    // checks if set is completely empty
    if (set->head == NULL && set->tail == NULL) {
        return 1;
    } else {
        return 0;
    }
}

int is_set_full(CacheQueue * set) {
    return set->count == E;
}

void dequeue (CacheQueue * set) { // remove tail if full
    if (is_set_empty(set)) {
        return;
    }
    
    // If this is the only node in list, then remove both head and tail
    if (set->head == set->tail) {
        free(set->head);
        set->head = NULL;
        set->tail = NULL;
    } else {
        // Change tail and remove the previous tail
        QueueNode * temp_node = set->tail;
        set->tail = set->tail->previous;
        set->tail->next = NULL;
        free(temp_node);
    }
    
    set->count--;
}

void pop_node (CacheQueue * set, unsigned long tag) {
    QueueNode * node_to_pop = get_node_pointer(set, tag);
    if (node_to_pop != NULL){
        if (set->head == set->tail) { // only item in the list
            set->head = NULL;
            set->tail = NULL;
        } else if (node_to_pop == set->tail) { // at the end of list
            node_to_pop->previous->next = NULL;
            set->tail = node_to_pop->previous;
        } else if (node_to_pop == set->head) { // at the beginning of list
            set->head = node_to_pop->next;
            node_to_pop->next->previous = NULL;
        } else { // in the middle
            node_to_pop->next->previous = node_to_pop->previous;
            node_to_pop->previous->next = node_to_pop->next;
        }
        free(node_to_pop);
        set->count--;
    }
    // return NULL;
}

QueueNode * new_node (unsigned long tag) {
    QueueNode * temp_node = (QueueNode *) malloc(sizeof(QueueNode));
    temp_node->tag = tag;
    
    // Initialize prev and next as NULL
    temp_node->previous = NULL;
    temp_node->next = NULL;
//    temp_node->valid = 1;
    
    return temp_node;
}

void enqueue (CacheQueue * set, unsigned long tag) {
    QueueNode * new_block = new_node(tag);
    
    if (is_set_empty(set)) {
        // when empty set change both head and tail pointers
        set->head = new_block;
        set->tail = new_block;
    } else if (is_set_available(set)){ // when the set has space but isnt completely full
        new_block->next = set->head;
        set->head->previous = new_block;
        //        set->head->next stays the same
        set->head = new_block;
    
    } else if (is_set_full(set)){ // when set is completely full, we need to evict
        //        printf("here\n");
        // LRU
        dequeue(set); // make some space
        
        if (set->head == NULL) { // direct mapped
            set->head = new_block;
            set->tail = new_block;
        } else {
            new_block->next = set->head;
            set->head->previous = new_block;
            //        set->head->next stays the same
            set->head = new_block;
        }
        
    } else {
        free(new_block);
        return;
    }
    
    set->count++;
}

int is_in_set (CacheQueue * set, unsigned long tag) {
    if (is_set_empty(set)) {
        return 0;
    } else {
        QueueNode * current = set->head; // just start at head and go until NULL
        
        while (current != NULL) {
            if (current->tag == tag) {
                //            printf("tag from set: %llx\n", current->tag_index);
                return 1;
            }
            current = current->next;
        }
    }
    
    return 0;
}

QueueNode * get_node_pointer (CacheQueue * set, unsigned long tag) {
    if (is_set_empty(set)) {
        return NULL;
    } else {
        QueueNode * current = set->head; // just start at head and go until NULL
        
        while (current != NULL) {
            if (current->tag == tag) {
                //            printf("tag from set: %llx\n", current->tag_index);
                return current;
            }
            current = current->next;
        }
    }
    
    return NULL;
}

void prefetch_block(CacheQueue * cache, unsigned long address) {
    
    unsigned long pf_tag = (address + B) >> (s + b);
    unsigned long pf_set_index = 0;

    if (associativity_level != 'a') {
        pf_set_index = ((address + B) & ((1 << (s + b)) - 1)) >> b;
    }
    
    CacheQueue * set = &cache[pf_set_index];
    
#ifdef PA4_DEBUG
    printf("prefetching a %lu byte offset of %lx\n", B, address);
    printf("pf_set: %lx\n", pf_set_index);
    printf("pf_tag: %lx\n", pf_tag);
    printf("pf_is_in_set: %d\n", is_in_set(set, pf_tag));
#endif
    
    if (is_in_set(set, pf_tag)) { // no need to prefetch if already in set
        return;
    } else {
        enqueue(set, pf_tag);
        increment_counter('r');
    }
}
// this is where he died
