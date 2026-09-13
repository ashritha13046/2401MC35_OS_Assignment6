#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

struct user_mutex {
    uint locked;
};

void mutex_lock(struct user_mutex *m) {
    while (xchg(&m->locked, 1) != 0) {
        sleep(1);
    }
}

void mutex_unlock(struct user_mutex *m) {
    xchg(&m->locked, 0);
}

void delay(int cycles) {
    volatile int dummy = 0;
    for (int i = 0; i < cycles * 200000; i++) {
        dummy += i;
    }
}

int main(int argc, char *argv[]) {
    struct user_mutex lock1 = {0};
    struct user_mutex lock2 = {0};

    printf(1, "=== Starting Deadlock Prevention (Enforced Global Lock Ordering) ===\n");
    int pid = fork();

    if (pid < 0) {
        printf(1, "Fork failed\n");
        exit();
    }

    if (pid == 0) {
        // Child: Process B strictly obeys global hierarchy (Lock1 -> Lock2)
        printf(1, "[Process B] Attempting to acquire Lock1 (Order: 1)...\n");
        mutex_lock(&lock1);
        printf(1, "[Process B] Acquired Lock1.\n");

        delay(3);

        printf(1, "[Process B] Attempting to acquire Lock2 (Order: 2)...\n");
        mutex_lock(&lock2);
        printf(1, "[Process B] Acquired Lock2.\n");

        printf(1, "[Process B] Working with both locks held...\n");
        delay(3);

        mutex_unlock(&lock2);
        mutex_unlock(&lock1);
        printf(1, "[Process B] Released Lock2 and Lock1.\n");
        exit();
    } else {
        // Parent: Process A strictly obeys global hierarchy (Lock1 -> Lock2)
        printf(1, "[Process A] Attempting to acquire Lock1 (Order: 1)...\n");
        mutex_lock(&lock1);
        printf(1, "[Process A] Acquired Lock1.\n");

        delay(3);

        printf(1, "[Process A] Attempting to acquire Lock2 (Order: 2)...\n");
        mutex_lock(&lock2);
        printf(1, "[Process A] Acquired Lock2.\n");

        printf(1, "[Process A] Working with both locks held...\n");
        delay(3);

        mutex_unlock(&lock2);
        mutex_unlock(&lock1);
        printf(1, "[Process A] Released Lock2 and Lock1.\n");

        wait();
        printf(1, "=== Finished: All processes completed without deadlock. ===\n");
        exit();
    }
}