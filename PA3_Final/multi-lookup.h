// multi-lookup.h
#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <netdb.h>
#include <pthread.h>
#include "array.h" // includes my shared array header from PA3

// Constants required by the assignment
#define ARRAY_SIZE 10
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_NAME_LENGTH 255
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

// Struct to pass data to requester threads
typedef struct {
    FILE* requester_log;
    pthread_mutex_t* log_mutex;
    char** input_files;
    int* file_index;
    int total_files;
    shared_array_t* shared_array;
    pthread_mutex_t* file_mutex;
} requester_args_t;

// Struct to pass data to resolver threads
typedef struct {
    FILE* resolver_log;
    pthread_mutex_t* log_mutex;
    shared_array_t* shared_array;
    int* requester_done;
    pthread_mutex_t* requester_done_mutex;
} resolver_args_t;

#endif // MULTI_LOOKUP_H

