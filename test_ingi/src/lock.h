#ifndef LOCK_H
#define LOCK_H

typedef struct {
    volatile int flag;   // 0 = libre, 1 = verrouillé
} lock_t;

void lock_init(lock_t *l);

void lock(lock_t *l);

void unlock(lock_t *l);

#endif
