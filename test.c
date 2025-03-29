#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PRODUCERS 2 // Number of producer threads to create
#define NUM_CONSUMERS 2 // Number of consumer threads to create
#define NUM_ITEMS 5 // Number of items each producer will insert

shared_array arr; // Shared buffer used by producers and consumers

void* producer(void* arg) {
    int producer_id = *((int*)arg); // Extract the producer ID
    for (int i = 0; i < NUM_ITEMS; i++) {
        char item[50];
        snprintf(item, sizeof(item), "Item %d from Producer %d", i, producer_id); // Create item string
        array_insert(&arr, strdup(item)); // Insert item into the shared array
        printf("Producer %d inserted %s into the shared buffer.\n", producer_id, item); // Print insertion status
    }
    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *((int*)arg); // Extract the consumer ID
    for (int i = 0; i < NUM_ITEMS; i++) {
        char *item = array_remove(&arr); // Remove item from the shared array
        printf("Consumer %d removed  %s from the buffer.\n", consumer_id, item); // Print removal status
        free(item); // Free allocated memory to prevent memory leaks
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS]; // Arrays for producer and consumer threads
    int thread_ids[NUM_PRODUCERS + NUM_CONSUMERS]; // Array for thread IDs

    array_init(&arr); // Initialize the shared buffer

    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &thread_ids[i]);
    }

    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        thread_ids[NUM_PRODUCERS + i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &thread_ids[NUM_PRODUCERS + i]);
    }

    // Wait for all producer threads to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    // Wait for all consumer threads to finish
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    array_destroy(&arr); // Clean up shared buffer resources just as a double case for protection
    return 0;
}
