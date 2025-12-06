#include <pthread.h> // Inclut les types et fonctions pour les threads POSIX (pthread_t, mutex, etc.)
#include <semaphore.h> // Fournit le type sem_t et les fonctions de gestion des sémaphores (sem_init, sem_wait, sem_post, sem_destroy).
#include <stdio.h> // Inclut les fonctions d'entrée/sortie standard (printf, fprintf, etc.)
#include <stdlib.h> // Inclut les fonctions standard (malloc, free, atoi, EXIT_SUCCESS/FAILURE)

#define WORK_ITERS 10000
#define TOTAL_WRITES 6400
#define TOTAL_READS 25400

int readcount  = 0;
int writecount = 0;

sem_t db;
sem_t rmutex;
sem_t wmutex;
sem_t queue;

typedef struct {
    int id;
    int count;
} thread_args_t;

static void do_work(void) {
    for (int i = 0; i < WORK_ITERS; i++) {
        // Ne rien faire
    }
}

void* writer_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int to_write = args->count;

    for (int k = 0; k < to_write; k++) {
        sem_wait(&wmutex);
        writecount++;
        if (writecount == 1) {
            sem_wait(&queue);
        }
        sem_post(&wmutex);

        sem_wait(&db);
        do_work();
        sem_post(&db);

        sem_wait(&wmutex);
        writecount--;
        if (writecount == 0) {
            sem_post(&queue);
        }
        sem_post(&wmutex);
    }

    return NULL;
}

void* reader_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int to_read = args->count;

    for (int k = 0; k < to_read; k++) {
        sem_wait(&queue);
        sem_wait(&rmutex);
        readcount++;
        if (readcount == 1) {
            sem_wait(&db);
        }
        sem_post(&rmutex);

        sem_post(&queue);
        do_work();

        sem_wait(&rmutex);
        readcount--;
        if (readcount == 0) {
            sem_post(&db);
        }
        sem_post(&rmutex);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NB_ECRIVAINS NB_LECTEURS\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nb_writers = atoi(argv[1]);
    int nb_readers = atoi(argv[2]);

    if (nb_writers <= 0 || nb_readers <= 0) {
        fprintf(stderr, "NB_ECRIVAINS et NB_LECTEURS doivent être > 0\n");
        return EXIT_FAILURE;
    }

    pthread_t *writers = malloc(nb_writers*sizeof(pthread_t));
    pthread_t *readers = malloc(nb_readers*sizeof(pthread_t));
    thread_args_t *wargs = malloc(nb_writers*sizeof(thread_args_t));
    thread_args_t *rargs = malloc(nb_readers*sizeof(thread_args_t));

    if (!writers || !readers || !wargs || !rargs) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    int base_w = TOTAL_WRITES / nb_writers;
    int rest_w = TOTAL_WRITES % nb_writers;
    for (int i = 0; i < nb_writers; i++) {
        wargs[i].id = i;
        wargs[i].count = base_w + (i < rest_w ? 1 : 0);
    }

    int base_r = TOTAL_READS / nb_readers;
    int rest_r = TOTAL_READS % nb_readers;
    for (int i = 0; i < nb_readers; i++) {
        rargs[i].id = i;
        rargs[i].count = base_r + (i < rest_r ? 1 : 0);
    }

    sem_init(&db, 0, 1);
    sem_init(&rmutex, 0, 1);
    sem_init(&wmutex, 0, 1);
    sem_init(&queue, 0, 1);

    for (int i = 0; i < nb_writers; i++) {
        if (pthread_create(&writers[i], NULL, writer_thread, &wargs[i]) != 0) {
            perror("pthread_create writer");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_readers; i++) {
        if (pthread_create(&readers[i], NULL, reader_thread, &rargs[i]) != 0) {
            perror("pthread_create reader");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_writers; i++) {
        pthread_join(writers[i], NULL);
    }

    for (int i = 0; i < nb_readers; i++) {
        pthread_join(readers[i], NULL);
    }

    sem_destroy(&db);
    sem_destroy(&rmutex);
    sem_destroy(&wmutex);
    sem_destroy(&queue);

    free(writers);
    free(readers);
    free(wargs);
    free(rargs);

    return EXIT_SUCCESS;
}
