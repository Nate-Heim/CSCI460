#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int shared_array_init(shared_array_t *arr, int size) {
    (void)size; // Ignore dynamic sizing since we use fixed ARRAY_CAPACITY
    arr->next_insert_index = 0;
    arr->next_remove_index = 0;
    arr->current_size = 0;
    pthread_mutex_init(&arr->buffer_mutex, NULL);
    pthread_cond_init(&arr->buffer_not_empty, NULL);
    pthread_cond_init(&arr->buffer_not_full, NULL);
    return 0;
}

int shared_array_push(shared_array_t *arr, const char *item) {
    pthread_mutex_lock(&arr->buffer_mutex);

    while (arr->current_size == ARRAY_CAPACITY) {
        pthread_cond_wait(&arr->buffer_not_full, &arr->buffer_mutex);
    }

    // Copy item into the buffer
    strncpy(arr->storage[arr->next_insert_index], item, MAX_NAME_LENGTH);
    arr->storage[arr->next_insert_index][MAX_NAME_LENGTH - 1] = '\0';

    arr->next_insert_index = (arr->next_insert_index + 1) % ARRAY_CAPACITY;
    arr->current_size++;

    pthread_cond_signal(&arr->buffer_not_empty);
    pthread_mutex_unlock(&arr->buffer_mutex);
    return 0;
}

int shared_array_pop(shared_array_t *arr, char *item_out) {
    pthread_mutex_lock(&arr->buffer_mutex);

    while (arr->current_size == 0) {
        pthread_cond_wait(&arr->buffer_not_empty, &arr->buffer_mutex);
    }

    strncpy(item_out, arr->storage[arr->next_remove_index], MAX_NAME_LENGTH);
    item_out[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination

    arr->next_remove_index = (arr->next_remove_index + 1) % ARRAY_CAPACITY;
    arr->current_size--;

    pthread_cond_signal(&arr->buffer_not_full);
    pthread_mutex_unlock(&arr->buffer_mutex);
    return 0;
}

void shared_array_cleanup(shared_array_t *arr) {
    pthread_mutex_destroy(&arr->buffer_mutex);
    pthread_cond_destroy(&arr->buffer_not_empty);
    pthread_cond_destroy(&arr->buffer_not_full);
}

