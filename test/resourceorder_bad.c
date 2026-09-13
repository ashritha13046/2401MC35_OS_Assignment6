#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

struct user_mutex {
    uint locked;
};

void mutex_lock(struct user_mutex *m) {
    while (xchg(&m->locked, 1) != 0) {
        sleep(1); // Relinquish CPU slice
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
    // Both processes share this memory pattern via fork
    struct user_mutex lock1 = {0};
    struct user_mutex lock2 = {0};

    printf(1, "=== Starting Deadlock Demonstration (Inverted Lock Order) ===\n");
    int pid = fork();

    if (pid < 0) {
        printf(1, "Fork failed\n");
        exit();
    }

    if (pid == 0) {
        // Child: Process B acquires Lock2 then Lock1
        printf(1, "[Process B] Attempting to acquire Lock2...\n");
        mutex_lock(&lock2);
        printf(1, "[Process B] Acquired Lock2.\n");

        delay(5); // Window to force circular wait

        printf(1, "[Process B] Waiting for Lock1...\n");
        mutex_lock(&lock1);
        printf(1, "[Process B] Acquired Lock1.\n");

        mutex_unlock(&lock1);
        mutex_unlock(&lock2);
        printf(1, "[Process B] Finished work and released locks.\n");
        exit();
    } else {
        // Parent: Process A acquires Lock1 then Lock2
        printf(1, "[Process A] Attempting to acquire Lock1...\n");
        mutex_lock(&lock1);
        printf(1, "[Process A] Acquired Lock1.\n");

        delay(5); // Window to force circular wait

        printf(1, "[Process A] Waiting for Lock2...\n");
        mutex_lock(&lock2);
        printf(1, "[Process A] Acquired Lock2.\n");

        mutex_unlock(&lock2);
        mutex_unlock(&lock1);
        printf(1, "[Process A] Finished work and released locks.\n");

        wait();
        printf(1, "Completed successfully.\n");
        exit();
    }
}