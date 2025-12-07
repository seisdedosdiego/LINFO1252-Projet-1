#include "lock.h"

static inline int xchg(volatile int *addr, int newval) {
    int old;
    __asm__ __volatile__(
        "lock xchg %0, %1"    // échange atomique old <-> *addr
        : "=r"(old), "+m"(*addr)
        : "0"(newval)
        : "memory");
    return old;              // ancienne valeur de *addr
}

void lock_init(lock_t *l) {
    l->flag = 0;             // 0 = libre
}

void lock(lock_t *l) {
    // Tant que l’ancienne valeur était 1, quelqu’un tient déjà le verrou
    // -> on tourne (busy-wait) jusqu’à ce qu’on obtienne un 0.
    while (xchg(&l->flag, 1) == 1) {
        // attente active (rien à faire ici)
    }
}

void unlock(lock_t *l) {
    // relâcher le verrou : on remet le flag à 0
    l->flag = 0;
}
