#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    long long iterations;
    int use_mutex;
} worker_arg_t;

static long long shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end)
{

    time_t elapsed_time_seconds = end->tv_sec - start->tv_sec;
    double elapsed_time_nanoseconds = end->tv_nsec - start->tv_nsec;

    if (elapsed_time_nanoseconds < 0) {
        elapsed_time_seconds -= 1;
        elapsed_time_nanoseconds += 1000000000;
    }

    return (double)elapsed_time_seconds + (double)elapsed_time_nanoseconds / 1000000000.0;

}

static long long parse_positive_ll(const char *text, const char *label)
{
    char *end = NULL;
    long long value;

    errno = 0;
    value = strtoll(text, &end, 10);


    if (errno != 0 || text == end || *end != '\0' || value <= 0 ){
        fprintf(stderr, "Invalid %s: '%s' (must be a positive integer)\n", label, text);
        exit(1);
    }
    return value;
}

static int parse_mutex_mode(const char *text)
{
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);

    if (errno != 0 || text == end || *end != '\0' || (value != 0 && value != 1) ){
        fprintf(stderr, "Invalid use_mutex: '%s' (must be 0 or 1)\n",text);
        exit(1);
    }
    
    return (int)value;
    return 0;
}

static void *worker_main(void *argument)
{
    worker_arg_t *worker = argument;

    for (long long counter = 0; counter < worker->iterations; counter++)
    {
        if (worker->use_mutex != 0){
            int rc = pthread_mutex_lock(&counter_mutex);
            if(rc != 0){
                fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(rc));
                exit(1);
            }
            shared_counter++;
            rc = pthread_mutex_unlock(&counter_mutex);
            if(rc != 0){
                fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(rc));
                exit(1);
            }
        } else{
            shared_counter++;
        }
    }
    

    return NULL;
}

int main(int argc, char **argv)
{
    int workers;
    long long total_iterations;
    int use_mutex;
    int trial;
    pthread_t *threads = NULL;
    worker_arg_t *arguments = NULL;
    struct timespec start;
    struct timespec end;
    double elapsed;
    long long base_iterations;
    long long remainder;
    int created = 0;
    int create_failed = 0;
    int join_failed = 0;

    if (argc != 5) {
        fprintf(stderr,
                "Usage: %s WORKERS TOTAL_ITERATIONS USE_MUTEX TRIAL\n",
                argv[0]);
        return 1;
    }

    workers = (int)parse_positive_ll(argv[1], "workers");
    total_iterations = parse_positive_ll(argv[2], "total_iterations");
    use_mutex = parse_mutex_mode(argv[3]);
    trial = (int)parse_positive_ll(argv[4], "trial");

    if (workers > 128) {
        fprintf(stderr, "workers is too large\n");
        return 1;
    }

    if (total_iterations < workers) {
        fprintf(stderr, "total_iterations must be at least workers\n");
        return 1;
    }

    threads = malloc((size_t)workers * sizeof *threads);
    arguments = malloc((size_t)workers * sizeof *arguments);

    if (threads == NULL || arguments == NULL) {
        perror("malloc");
        free(threads);
        free(arguments);
        return 1;
    }

    shared_counter = 0;

    base_iterations = total_iterations / workers;
    remainder = total_iterations % workers;

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime");
        free(threads);
        free(arguments);
        return 1;
    }

    for(int i = 0; i < workers; i++){
        arguments[i].use_mutex = use_mutex;
        arguments[i].iterations = base_iterations + (i < remainder ? 1 : 0);

        int rc = pthread_create(&threads[i], NULL, worker_main, &arguments[i]);
        if(rc != 0){
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            create_failed = 1;
            break;
        }

        created++;
    }

    for (int i = 0; i < created; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(rc));
            join_failed = 1;
        }
    }

    if (create_failed || join_failed) {
        free(threads);
        free(arguments);
        return 1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        perror("clock_gettime");
        free(threads);
        free(arguments);
        return 1;
    }

    elapsed = elapsed_seconds(&start, &end);

    printf("%d,%lld,%d,%d,%lld,%.6f\n",
           workers,
           total_iterations,
           use_mutex,
           trial,
           shared_counter,
           elapsed);

    free(threads);
    free(arguments);

    return 0;
}
