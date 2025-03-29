#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include "multi-lookup.h"
#include "util.h"
#include "array.h"

// ------------------- Requester Thread Function -------------------
/*
 * Each requester thread:
 * - Picks an available input file
 * - Reads hostnames line by line
 * - Pushes hostnames into the shared array (blocks if full)
 * - Writes each hostname to the requester log
 */
void* requester_thread(void* arg) {
    requester_args_t* args = (requester_args_t*)arg;
    int serviced = 0;
    char hostname[MAX_NAME_LENGTH];

    while (1) {
        // Lock shared file index to safely assign a file
        pthread_mutex_lock(args->file_mutex);
        if (*(args->file_index) >= args->total_files) {
            pthread_mutex_unlock(args->file_mutex);
            break;
        }
        int index = (*args->file_index)++;
        pthread_mutex_unlock(args->file_mutex);

        // Open the input file
        FILE* infile = fopen(args->input_files[index], "r");
        if (!infile) {
            fprintf(stderr, "invalid file %s\n", args->input_files[index]);
            continue;
        }

        printf("Thread %lu: reading file %s\n", pthread_self(), args->input_files[index]);

        // Read hostnames and push into shared array
        while (fgets(hostname, sizeof(hostname), infile)) {
            hostname[strcspn(hostname, "\n")] = '\0'; // Remove newline

            printf("Thread %lu: pushing hostname '%s'\n", pthread_self(), hostname);
            shared_array_push(args->shared_array, hostname);

            pthread_mutex_lock(args->log_mutex);
            fprintf(args->requester_log, "%s\n", hostname);
            pthread_mutex_unlock(args->log_mutex);

            printf("Thread %lu: logged hostname '%s'\n", pthread_self(), hostname);
        }

        fclose(infile);
        serviced++;
        printf("Thread %lu: finished file %s\n", pthread_self(), args->input_files[index]);
    }

    printf("thread %lu serviced %d files\n", pthread_self(), serviced);
    return NULL;
}

// ------------------- Resolver Thread Function -------------------
/*
 * Each resolver thread:
 * - Pops a hostname from the shared array (blocks if empty)
 * - Resolves it to an IP address (or NOT_RESOLVED)
 * - Logs the result to the resolver log
 * - Exits when requesters are done and buffer is empty
 */
void* resolver_thread(void* arg) {
    resolver_args_t* args = (resolver_args_t*)arg;
    char hostname[MAX_NAME_LENGTH];
    char ip[MAX_IP_LENGTH];
    int resolved_count = 0;

    while (1) {
        pthread_mutex_lock(&args->shared_array->buffer_mutex);

        // Wait until there is data or requesters are done
        while (args->shared_array->current_size == 0) {
            pthread_mutex_lock(args->requester_done_mutex);
            int done = *(args->requester_done);
            pthread_mutex_unlock(args->requester_done_mutex);

            if (done) {
                // No more hostnames will be added and buffer is empty
                pthread_mutex_unlock(&args->shared_array->buffer_mutex);
                printf("Thread %lu: exiting - no more data.\n", pthread_self());
                printf("thread %lu resolved %d hostnames\n", pthread_self(), resolved_count);
                return NULL;
            }

            pthread_cond_wait(&args->shared_array->buffer_not_empty, &args->shared_array->buffer_mutex);
        }

        // Pop the hostname
        strncpy(hostname, args->shared_array->storage[args->shared_array->next_remove_index], MAX_NAME_LENGTH);
        hostname[MAX_NAME_LENGTH - 1] = '\0';

        args->shared_array->next_remove_index = (args->shared_array->next_remove_index + 1) % ARRAY_CAPACITY;
        args->shared_array->current_size--;

        pthread_cond_signal(&args->shared_array->buffer_not_full);
        pthread_mutex_unlock(&args->shared_array->buffer_mutex);

        // Do the DNS lookup
        printf("Thread %lu: resolving '%s'\n", pthread_self(), hostname);
        if (dnslookup(hostname, ip, sizeof(ip)) == UTIL_FAILURE) {
            strcpy(ip, "NOT_RESOLVED");
        }

        // Write to the resolver log
        pthread_mutex_lock(args->log_mutex);
        fprintf(args->resolver_log, "%s, %s\n", hostname, ip);
        pthread_mutex_unlock(args->log_mutex);

        printf("Thread %lu: resolved '%s' → '%s'\n", pthread_self(), hostname, ip);
        resolved_count++;
    }
}

// ------------------- Main Function -------------------
/*
 * Orchestrates the program:
 * - Parses arguments
 * - Starts requester and resolver threads
 * - Waits for threads to finish
 * - Coordinates shutdown and cleanup
 */
int main(int argc, char* argv[]) {
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <#requesters> <#resolvers> <requester_log> <resolver_log> <input files...>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_requesters = atoi(argv[1]);
    int num_resolvers = atoi(argv[2]);

    // Validate thread and file count
    if (num_requesters > MAX_REQUESTER_THREADS || num_resolvers > MAX_RESOLVER_THREADS || argc - 5 > MAX_INPUT_FILES) {
        fprintf(stderr, "Error: thread or input file limits exceeded\n");
        return EXIT_FAILURE;
    }

    // Open output log files
    FILE* requester_log = fopen(argv[3], "w");
    FILE* resolver_log = fopen(argv[4], "w");
    if (!requester_log || !resolver_log) {
        perror("Error opening log files");
        return EXIT_FAILURE;
    }

    // Setup input files
    char** input_files = &argv[5];
    int total_files = argc - 5;

    // Initialize shared buffer
    shared_array_t shared_array;
    shared_array_init(&shared_array, ARRAY_SIZE);

    // Initialize mutexes
    pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t requester_done_mutex = PTHREAD_MUTEX_INITIALIZER;

    // Shared state
    int file_index = 0;
    int requester_done = 0;

    // Thread arrays
    pthread_t requesters[num_requesters];
    pthread_t resolvers[num_resolvers];

    // Thread arguments
    requester_args_t req_args = {
        .requester_log = requester_log,
        .log_mutex = &log_mutex,
        .input_files = input_files,
        .file_index = &file_index,
        .total_files = total_files,
        .shared_array = &shared_array,
        .file_mutex = &file_mutex
    };

    resolver_args_t res_args = {
        .resolver_log = resolver_log,
        .log_mutex = &log_mutex,
        .shared_array = &shared_array,
        .requester_done = &requester_done,
        .requester_done_mutex = &requester_done_mutex
    };

    // Start timer
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Create requester threads
    printf("[main] Creating %d requester threads...\n", num_requesters);
    for (int i = 0; i < num_requesters; i++) {
        pthread_create(&requesters[i], NULL, requester_thread, &req_args);
    }

    // Create resolver threads
    printf("[main] Creating %d resolver threads...\n", num_resolvers);
    for (int i = 0; i < num_resolvers; i++) {
        pthread_create(&resolvers[i], NULL, resolver_thread, &res_args);
    }

    // Wait for all requester threads to finish
    for (int i = 0; i < num_requesters; i++) {
        pthread_join(requesters[i], NULL);
    }
    printf("[main] All requester threads have finished.\n");

    // Signal that no more hostnames will be added
    pthread_mutex_lock(&requester_done_mutex);
    requester_done = 1;
    pthread_mutex_unlock(&requester_done_mutex);

    // Wake up any resolver threads that are blocked on empty buffer
    pthread_mutex_lock(&shared_array.buffer_mutex);
    pthread_cond_broadcast(&shared_array.buffer_not_empty);
    pthread_mutex_unlock(&shared_array.buffer_mutex);
    printf("[main] Broadcast sent to wake resolver threads.\n");

    // Wait for all resolver threads to finish
    for (int i = 0; i < num_resolvers; i++) {
        pthread_join(resolvers[i], NULL);
    }
    printf("[main] All resolver threads have finished.\n");

    // End timer
    gettimeofday(&end, NULL);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("./multi-lookup: total time is %f seconds\n", time_spent);

    // Cleanup
    fclose(requester_log);
    fclose(resolver_log);
    shared_array_cleanup(&shared_array);

    printf("[main] Program exiting successfully.\n");
    return EXIT_SUCCESS;
}

