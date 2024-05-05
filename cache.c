//
// Created by nickgalis on 5/4/24.
//

#include "cache.h"
#include <stdlib.h>
#include <string.h>

Cache* new_cache(int max_size) {
    Cache* cache = (Cache*) malloc(sizeof(Cache));
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    cache->total_size = 0;
    cache->max_size = max_size;
    cache->memory_pool = (char*) malloc(max_size * sizeof(char)); // Allocate Memory Pool
    return cache;
}

Node* new_node(const char* key, char* value, size_t size) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->key = strdup(key);
    node->value = value;
    node->data_size = size;
    node->next = NULL;
    return node;
}

void add_to_cache(const char* key, char* value, size_t size, Cache* cache) {

    // If the cache is at full capacity, remove the oldest (head) node
    while (cache->total_size + size > cache->max_size) {
        Node* oldHead = cache->head;
        cache->head = oldHead->next;
        cache->total_size -= oldHead->data_size;

        // update pointers and move them back
        Node* current = cache->head;
        while(current) {
            memmove(current->start_pos_in_pool - oldHead->data_size,
                    current->start_pos_in_pool,
                    current->size_in_pool);
            current->start_pos_in_pool -= oldHead->data_size;
            current = current->next;
        }

        // Freeing the memory occupied by the old node
        free(oldHead->key);
        // Don't free oldHead->value because it points to cache->memory_pool
        free(oldHead);

        cache->size--;
    }

    // When either cache is empty or room has been made in cache, we are adding the new node
    Node* node = new_node(key, value, size);
    strncpy(cache->memory_pool + cache->total_size, value, size);
    node->start_pos_in_pool = cache->memory_pool + cache->total_size;
    node->size_in_pool = size;

    if (cache->tail != NULL) {
        cache->tail->next = node;
    }
    cache->tail = node;
    if (cache->head == NULL) {
        cache->head = node;
    }

    cache->size++;
    cache->total_size += size;
}


Node* search_cache(const char* key, Cache* cache) {
    Node* current = cache->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;  // Return the node if found
        }
        current = current->next;
    }
    return NULL; 
}


