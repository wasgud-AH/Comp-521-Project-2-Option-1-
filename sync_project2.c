#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    long long iterations;
    int use_mutex;
} worker_arg_t;

static long long shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end)
{
    /* TODO:
     * Subtract start from end and return the elapsed time in seconds.
     *
     * Requirements:
     * 1. Subtract tv_sec values. done
     * 2. Subtract tv_nsec values. done
     * 3. If the nanosecond result is negative, borrow one second. done
     * 4. Return the result as a double. done
     */

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

    /* TODO:
     * Reject the input when:
     * 1. errno is not zero;
     * 2. no characters were converted;
     * 3. extra characters remain after the number;
     * 4. value is less than or equal to zero.
     *
     * Print an error containing label and exit with status 1.
     */
    (void)label;

    return value;
}

static int parse_mutex_mode(const char *text)
{
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);

    /* TODO:
     * Accept only:
     * 0 = no mutex
     * 1 = mutex enabled
     *
     * Reject invalid input and exit with status 1.
     */
    (void)value;
    (void)end;

    return 0;
}

static void *worker_main(void *argument)
{
    worker_arg_t *worker = argument;

    /* TODO:
     * Repeat worker->iterations times.
     *
     * If worker->use_mutex is nonzero:
     *     lock counter_mutex;
     *     increment shared_counter;
     *     unlock counter_mutex.
     *
     * Otherwise:
     *     increment shared_counter without the mutex.
     *
     * Check the return values of pthread_mutex_lock() and
     * pthread_mutex_unlock(). Exit with an error if either fails.
     */
    (void)worker;

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

    /* TODO:
     * Allocate the pthread_t array and worker_arg_t array.
     * Check both allocations.
     */
    (void)threads;
    (void)arguments;

    shared_counter = 0;

    base_iterations = total_iterations / workers;
    remainder = total_iterations % workers;

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime");
        free(threads);
        free(arguments);
        return 1;
    }

    /* TODO:
     * Create workers threads.
     *
     * For thread i:
     * 1. Set arguments[i].use_mutex.
     * 2. Assign base_iterations plus one extra iteration when
     *    i is less than remainder.
     * 3. Call pthread_create().
     * 4. Track how many threads were successfully created.
     */
    (void)base_iterations;
    (void)remainder;
    (void)created;

    /* TODO:
     * Join each successfully created thread using pthread_join().
     * Report an error if pthread_join() fails.
     */

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
