#ifndef ARRAY_H
#define ARRAY_H

#include <pthread.h>

#define ARRAY_CAPACITY 10     // Must match ARRAY_SIZE in multi-lookup.h
#define MAX_NAME_LENGTH 255   // Used to size strings

typedef struct {
    char storage[ARRAY_CAPACITY][MAX_NAME_LENGTH];  // Fixed-size buffer of strings
    int next_insert_index;  // Index where the next item will be inserted
    int next_remove_index;  // Index of the next item to remove
    int current_size;       // Number of items currently in the buffer

    pthread_mutex_t buffer_mutex;         // Mutex for buffer access
    pthread_cond_t buffer_not_empty;      // Condition for consumer to wait on
    pthread_cond_t buffer_not_full;       // Condition for producer to wait on
} shared_array_t;

// Initializes the shared array and its synchronization primitives
int shared_array_init(shared_array_t* arr, int size);

// Pushes a hostname into the buffer (blocks if full)
int shared_array_push(shared_array_t* arr, const char* item);

// Pops a hostname from the buffer into the provided char array (blocks if empty)
int shared_array_pop(shared_array_t* arr, char* item_out);

// Frees any resources (not strictly needed with static array but good practice)
void shared_array_cleanup(shared_array_t* arr);

#endif // ARRAY_H

