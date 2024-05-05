//
// Created by nickgalis on 5/4/24.
//
#include <stdlib.h>
#ifndef CS410_FINAL_CACHE_H
#define CS410_FINAL_CACHE_H


typedef struct Node {
    char* key;
    char* value;
    size_t data_size;
    struct Node* next;
    char* start_pos_in_pool;
    size_t size_in_pool;
} Node;

typedef struct cache {
    Node* head;
    Node* tail;
    int size; // number of nodes in the list
    size_t total_size; // total size of data in the list
    int max_size; // total capacity of the list
    char *memory_pool;
} Cache;

Cache* new_cache(int max_size);

Node* new_node(const char* key, char* value, size_t size);

void add_to_cache(const char* key, char* value, size_t size, Cache* cache);

Node* search_cache(const char* key, Cache* cache);

#endif //CS410_FINAL_CACHE_H
