#include <pthread.h> // Inclut les types et fonctions pour les threads POSIX (pthread_t, mutex, etc.)
#include <stdio.h>   // Inclut les fonctions d'entrée/sortie standard (printf, fprintf, etc.)
#include <stdlib.h>  // Inclut les fonctions standard (malloc, free, atoi, EXIT_SUCCESS/FAILURE)
#include <stdbool.h>

#define NB_CYCLES 1000000   // nombre de cycles penser/manger

// Ces deux variables ne sont plus des constantes #define
int NB_PHILOSOPHES;         // sera lu sur la ligne de commande

pthread_t *phil;            // tableau dynamique de threads
pthread_mutex_t *baguette;  // tableau dynamique de mutex (baguettes/fourchettes)

void mange(int id) {
    // Dans le projet, on ne fait ni sleep ni affichage pour ne pas fausser les temps.
    // On laisse une fonction vide pour représenter l'action de manger.
    (void)id; // pour éviter un warning "unused parameter"
}

void* philosophe (void* arg)
{
    int id = *(int *)arg;          // on récupère l'id passé en argument

    int left  = id;                // baguette gauche
    int right = (left + 1) % NB_PHILOSOPHES; // baguette droite

    // Pour éviter le deadlock :
    // chaque philosophe prend toujours d'abord la baguette d'indice plus petit
    int first  = (left < right) ? left  : right;
    int second = (left < right) ? right : left;

    for (long i = 0; i < NB_CYCLES; i++) {

        // Phase "pense" : on ne fait rien (pas de sleep, pas d'affichage)

        // Prend la première baguette (mutex lock)
        pthread_mutex_lock(&baguette[first]);

        // Prend la deuxième baguette
        pthread_mutex_lock(&baguette[second]);

        // Mange (action symbolique, fonction vide)
        mange(id);

        // Relâche la deuxième baguette
        pthread_mutex_unlock(&baguette[second]);

        // Relâche la première baguette
        pthread_mutex_unlock(&baguette[first]);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NB_PHILOSOPHES\n", argv[0]);
        return EXIT_FAILURE;
    }

    NB_PHILOSOPHES = atoi(argv[1]);
    if (NB_PHILOSOPHES <= 1) {
        fprintf(stderr, "NB_PHILOSOPHES doit être > 1\n");
        return EXIT_FAILURE;
    }

    // Allocation des tableaux dynamiques
    phil     = malloc(NB_PHILOSOPHES * sizeof(pthread_t));
    baguette = malloc(NB_PHILOSOPHES * sizeof(pthread_mutex_t));
    int *ids = malloc(NB_PHILOSOPHES * sizeof(int));

    if (!phil || !baguette || !ids) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    // Initialisation des mutex (une baguette par philosophe)
    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        if (pthread_mutex_init(&baguette[i], NULL) != 0) {
            perror("pthread_mutex_init");
            return EXIT_FAILURE;
        }
    }

    // Création des threads philosophes
    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        ids[i] = i;  // on stocke l'id dans un tableau séparé
        if (pthread_create(&phil[i], NULL, philosophe, &ids[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Attente de la fin de tous les philosophes
    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        pthread_join(phil[i], NULL);
    }

    // Nettoyage
    for (int i = 0; i < NB_PHILOSOPHES; i++) {
        pthread_mutex_destroy(&baguette[i]);
    }

    free(phil);
    free(baguette);
    free(ids);

    return EXIT_SUCCESS;
}