#include <pthread.h> // Inclut les types et fonctions pour les threads POSIX (pthread_t, mutex, etc.)
#include <semaphore.h> // Fournit le type sem_t et les fonctions de gestion des sémaphores (sem_init, sem_wait, sem_post, sem_destroy).
#include <stdio.h> // Inclut les fonctions d'entrée/sortie standard (printf, fprintf, etc.)
#include <stdlib.h> // Inclut les fonctions standard (malloc, free, atoi, EXIT_SUCCESS/FAILURE)

#define BUFFER_SIZE 8
#define TOTAL_ITEMS 131072
#define WORK_ITERS 10000

int buffer[BUFFER_SIZE];
int in_idx = 0;
int out_idx = 0;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

typedef struct {
    int id;
    int count;
} thread_args_t;

static void do_work(void) {
    for (int i = 0; i < WORK_ITERS; i++) {
        // Ne rien faire
    }
}

void* producer(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    int id = args->id;
    int to_produce = args->count;

    for (int k = 0; k < to_produce; k++) {
        do_work();

        sem_wait(&empty);

        pthread_mutex_lock(&mutex);
        buffer[in_idx] = id;
        in_idx = (in_idx + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);

        sem_post(&full);
    }

    return NULL;
}

void* consumer(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    int to_consume = args->count;

    for (int k = 0; k < to_consume; k++) {
        sem_wait(&full);

        pthread_mutex_lock(&mutex);
        int item = buffer[out_idx];
        (void) item;
        out_idx = (out_idx + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);

        sem_post(&empty);
        do_work();
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NB_PRODUCTEURS NB_CONSOMMATEURS\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nb_prod = atoi(argv[1]);
    int nb_cons = atoi(argv[2]);

    if (nb_prod <= 0 || nb_cons <= 0) {
        fprintf(stderr, "NB_PRODUCTEURS et NB_CONSOMMATEURS doivent être > 0\n");
        return EXIT_FAILURE;
    }

    thread_args_t *prod_args = malloc(nb_prod*sizeof(thread_args_t));
    thread_args_t *cons_args = malloc(nb_cons*sizeof(thread_args_t));
    pthread_t *prod_threads = malloc(nb_prod*sizeof(pthread_t));
    pthread_t *cons_threads = malloc(nb_cons*sizeof(pthread_t));

    if (!prod_args || !cons_args || !prod_threads || !cons_threads) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    int base_prod = TOTAL_ITEMS / nb_prod;
    int rest_prod = TOTAL_ITEMS % nb_prod;
    for (int i = 0; i < nb_prod; i++) {
        prod_args[i].id = i;
        prod_args[i].count = base_prod + (i < rest_prod ? 1 : 0);
    }

    int base_cons = TOTAL_ITEMS / nb_cons;
    int rest_cons = TOTAL_ITEMS % nb_cons;
    for (int i = 0; i < nb_cons; i++) {
        cons_args[i].id = i;
        cons_args[i].count = base_cons + (i < rest_cons ? 1 : 0);
    }

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    for (int i = 0; i < nb_prod; i++) {
        if (pthread_create(&prod_threads[i], NULL, producer, &prod_args[i]) != 0) {
            perror("pthread_create producer");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_cons; i++) {
        if (pthread_create(&cons_threads[i], NULL, consumer, &cons_args[i]) != 0) {
            perror("pthread_create consumer");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_prod; i++) {
        pthread_join(prod_threads[i], NULL);
    }

    for (int i = 0; i < nb_cons; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    free(prod_args);
    free(cons_args);
    free(prod_threads);
    free(cons_threads);

    return EXIT_SUCCESS;
}
