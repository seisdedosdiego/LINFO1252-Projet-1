#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "lock.h"

#define TOTAL_SECTIONS 32768
#define WORK_ITERS 10000

static lock_t global_lock;

static long shared_counter = 0;

static void do_work(void) {
    for (int i = 0; i < WORK_ITERS; i++) {
        __asm__ __volatile__("" ::: "memory");
    }
}

typedef struct {
    int iterations;
} thread_args_t;

static void *worker(void *arg) {
    thread_args_t *targ = (thread_args_t *)arg;
    int iters = targ->iterations;

    for (int i = 0; i < iters; i++) {
        lock(&global_lock);

        do_work();
        shared_counter++;

        unlock(&global_lock);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NB_THREADS\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nthreads = atoi(argv[1]);
    if (nthreads <= 0) {
        fprintf(stderr, "NB_THREADS doit être > 0\n");
        return EXIT_FAILURE;
    }

    if (TOTAL_SECTIONS % nthreads != 0) {
        fprintf(stderr, "NB_THREADS doit diviser %d (ici, valeurs attendues: 1,2,4,8,16,32)\n",
                TOTAL_SECTIONS);
        return EXIT_FAILURE;
    }

    int iters_per_thread = TOTAL_SECTIONS / nthreads;

    lock_init(&global_lock);

    pthread_t *threads = malloc(nthreads * sizeof(pthread_t));
    thread_args_t *args = malloc(nthreads * sizeof(thread_args_t));
    if (!threads || !args) {
        perror("malloc");
        free(threads);
        free(args);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nthreads; i++) {
        args[i].iterations = iters_per_thread;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            perror("pthread_create");
            free(threads);
            free(args);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (shared_counter != TOTAL_SECTIONS) {
        fprintf(stderr,
                "Erreur: shared_counter = %ld, devrait être %d\n",
                shared_counter, TOTAL_SECTIONS);
        free(threads);
        free(args);
        return EXIT_FAILURE;
    }

    free(threads);
    free(args);

    return EXIT_SUCCESS;
}
