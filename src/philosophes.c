#include <stdio.h>      // Inclut les fonctions d'entrée/sortie standard (printf, fprintf, etc.)
#include <stdlib.h>     // Inclut les fonctions standard (malloc, free, atoi, EXIT_SUCCESS/FAILURE)
#include <pthread.h>    // Inclut les types et fonctions pour les threads POSIX (pthread_t, mutex, etc.)

// Structure pour passer plusieurs infos à chaque thread philosophe
typedef struct {
    int id;        // numéro du philosophe (0, 1, 2, ..., N-1)
    int n_philo;   // nombre total de philosophes
} philo_args_t;

// Pointeur vers un tableau de mutex représentant les fourchettes
pthread_mutex_t *forks;   // forks[i] = mutex pour la fourchette i

// Fonction exécutée par chaque thread philosophe
void *philosopher(void *arg) {
    // On caste le void* reçu en philo_args_t*
    philo_args_t *p = (philo_args_t *)arg;
    int id = p->id;          // identifiant du philosophe
    int n = p->n_philo;      // nombre total de philosophes

    // Indices des fourchettes gauche et droite
    int left = id;               // la fourchette à gauche a l’indice id
    int right = (id + 1) % n;    // la fourchette à droite est (id+1) modulo n

    // Pour éviter le deadlock : on impose un ordre de prise des fourchettes
    // On prend toujours d’abord la fourchette avec l’indice le plus petit
    int first = left < right ? left : right;
    int second = left < right ? right : left;

    // Boucle principale : le philosophe répète penser/manger 1 000 000 fois
    for (long i = 0; i < 1000000; i++) {

        // Phase "penser" : ici on ne fait rien (pas de sleep, pas de calcul)
        // juste un commentaire pour marquer l’intention

        // Prendre la première fourchette (mutex lock sur forks[first])
        pthread_mutex_lock(&forks[first]);

        // Prendre la deuxième fourchette (mutex lock sur forks[second])
        pthread_mutex_lock(&forks[second]);

        // Phase "manger" : on pourrait faire un mini calcul,
        // mais pour l’instant on ne fait rien non plus, c’est juste le temps de verrouillage

        // Reposer la deuxième fourchette (déverrouiller forks[second])
        pthread_mutex_unlock(&forks[second]);

        // Reposer la première fourchette (déverrouiller forks[first])
        pthread_mutex_unlock(&forks[first]);
    }

    // Quand le philosophe a fini ses 1 000 000 cycles, le thread se termine
    return NULL;   // valeur de retour du thread (non utilisée ici)
}

// Fonction principale
int main(int argc, char *argv[]) {
    // On vérifie qu’on a bien exactement 1 argument après le nom du programme
    if (argc != 2) {
        // Message d’erreur sur la sortie d’erreur (stderr)
        fprintf(stderr, "Usage: %s N_PHILOSOPHES\n", argv[0]);
        return EXIT_FAILURE;   // on arrête le programme avec un code d’erreur
    }

    // Conversion de l’argument (chaîne) en entier
    int N = atoi(argv[1]);

    // Vérification que N est raisonnable (>1, sinon ça n’a pas de sens)
    if (N <= 1) {
        fprintf(stderr, "N doit être > 1\n");
        return EXIT_FAILURE;
    }

    // Allocation dynamique du tableau de mutex (une fourchette par philosophe)
    forks = malloc(N * sizeof(pthread_mutex_t));
    if (!forks) {                  // si malloc renvoie NULL, il y a une erreur
        perror("malloc");          // affiche un message basé sur errno
        return EXIT_FAILURE;
    }

    // Initialisation de chaque mutex du tableau forks
    for (int i = 0; i < N; i++) {
        if (pthread_mutex_init(&forks[i], NULL) != 0) {  // NULL = attributs par défaut
            perror("pthread_mutex_init");
            return EXIT_FAILURE;
        }
    }

    // Allocation d’un tableau de threads (un thread par philosophe)
    pthread_t *threads = malloc(N * sizeof(pthread_t));

    // Allocation d’un tableau d’arguments (un philo_args_t par philosophe)
    philo_args_t *args = malloc(N * sizeof(philo_args_t));

    // Vérifier que les deux malloc ont réussi
    if (!threads || !args) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    // Création des N threads philosophes
    for (int i = 0; i < N; i++) {
        args[i].id = i;        // l’id du philosophe i
        args[i].n_philo = N;   // même nombre de philosophes pour tous

        // pthread_create lance la fonction philosopher dans un nouveau thread
        // &threads[i] : l’identifiant du thread créé sera stocké ici
        // NULL : attributs du thread (par défaut)
        // philosopher : fonction à exécuter
        // &args[i] : argument passé à la fonction philosopher
        if (pthread_create(&threads[i], NULL, philosopher, &args[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Attendre que tous les threads aient terminé
    for (int i = 0; i < N; i++) {
        // pthread_join bloque jusqu’à ce que le thread threads[i] se termine
        pthread_join(threads[i], NULL);  // on ignore la valeur de retour (de philosopher)
    }

    // Détruire tous les mutex une fois qu’ils ne sont plus utilisés
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    // Libérer toute la mémoire allouée dynamiquement
    free(forks);
    free(threads);
    free(args);

    // Tout s’est bien passé
    return EXIT_SUCCESS;
}