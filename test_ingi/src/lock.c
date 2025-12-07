#include "lock.h"

static inline int xchg(volatile int *addr, int newval) {
    int old;
    __asm__ __volatile__(
        "lock xchg %0, %1"
        : "=r"(old), "+m"(*addr)
        : "0"(newval)
        : "memory");
    return old;
}

void lock_init(lock_t *l) {
    l->flag = 0;
}

void lock(lock_t *l) {
    while (xchg(&l->flag, 1) == 1) {
        // attente active (ne rien à faire ici)
    }
}

void unlock(lock_t *l) {
    l->flag = 0;
}
