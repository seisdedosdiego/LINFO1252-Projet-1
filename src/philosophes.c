#include <pthread.h> // Inclut les types et fonctions pour les threads POSIX (pthread_t, mutex, etc.)
#include <stdio.h>   // Inclut les fonctions d'entrée/sortie standard (printf, fprintf, etc.)
#include <stdlib.h>  // Inclut les fonctions standard (malloc, free, atoi, EXIT_SUCCESS/FAILURE)
#include <stdbool.h> // Permet d'utiliser le type bool avec les valeurs true et false.

#define NB_CYCLES 1000000

int NB_PHILOSOPHES;

pthread_t *phil;            
pthread_mutex_t *baguette;

void mange(void) {/*Phase "manger" : on fait rien*/}

void* philosophe (void* arg) {
    int id = *(int *)arg;

    int left = id;
    int right = (left + 1) % NB_PHILOSOPHES;

    int first = (left < right) ? left : right;
    int second = (left < right) ? right : left;

    for (long i = 0; i < NB_CYCLES; i++) {
        // Phase "penser" : on fait rien
        pthread_mutex_lock(&baguette[first]);
        pthread_mutex_lock(&baguette[second]);
        mange();
        pthread_mutex_unlock(&baguette[second]);
        pthread_mutex_unlock(&baguette[first]);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NB_PHILOSOPHES\n", argv[0]);
        return EXIT_FAILURE;
    }

    NB_PHILOSOPHES = atoi(argv[1]);
    if (NB_PHILOSOPHES <= 1) {
        fprintf(stderr, "NB_PHILOSOPHES doit être > 1\n");
        return EXIT_FAILURE;
    }

    phil = malloc(NB_PHILOSOPHES * sizeof(pthread_t));
    baguette = malloc(NB_PHILOSOPHES * sizeof(pthread_mutex_t));
    int *ids = malloc(NB_PHILOSOPHES * sizeof(int));

    if (!phil || !baguette || !ids) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        if (pthread_mutex_init(&baguette[i], NULL) != 0) {
            perror("pthread_mutex_init");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        ids[i] = i;
        if (pthread_create(&phil[i], NULL, philosophe, &ids[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        pthread_join(phil[i], NULL);
    }

    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        pthread_mutex_destroy(&baguette[i]);
    }

    free(phil);
    free(baguette);
    free(ids);

    return EXIT_SUCCESS;
}