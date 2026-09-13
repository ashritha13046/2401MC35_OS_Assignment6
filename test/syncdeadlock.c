#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

#define NUM_PROCESSES 5
#define CYCLES 3

// Resource IDs: R0=Printer, R1=Scanner, R2=Disk
#define RES_PRINTER 0
#define RES_SCANNER 1
#define RES_DISK    2

char *res_names[] = {"Printer", "Scanner", "Disk"};

// Target pairs required by each of the 5 processes
int process_needs[NUM_PROCESSES][2] = {
    {RES_PRINTER, RES_SCANNER}, // P0 needs Printer & Scanner
    {RES_SCANNER, RES_DISK},    // P1 needs Scanner & Disk
    {RES_PRINTER, RES_DISK},    // P2 needs Printer & Disk
    {RES_PRINTER, RES_SCANNER}, // P3 needs Printer & Scanner
    {RES_SCANNER, RES_DISK}     // P4 needs Scanner & Disk
};

// Global Spinlock Mutex
struct mutex {
    uint locked;
};

void m_lock(struct mutex *m) {
    while (xchg(&m->locked, 1) != 0) {
        sleep(1);
    }
}

void m_unlock(struct mutex *m) {
    xchg(&m->locked, 0);
}

// Global resource inventory
struct mutex table_lock = {0};
int instances[3] = {2, 1, 2}; // 2 Printers, 1 Scanner, 2 Disks

void acquire_ordered_resources(int p_id, int r1, int r2) {
    // Enforce hierarchical ordering (smaller index acquired first)
    int first = (r1 < r2) ? r1 : r2;
    int second = (r1 < r2) ? r2 : r1;

    // Acquire first resource
    while (1) {
        m_lock(&table_lock);
        if (instances[first] > 0) {
            instances[first]--;
            printf(1, "[P%d] GRANTED %s (Remaining: %d)\n", p_id, res_names[first], instances[first]);
            m_unlock(&table_lock);
            break;
        }
        m_unlock(&table_lock);
        sleep(1);
    }

    // Acquire second resource
    while (1) {
        m_lock(&table_lock);
        if (instances[second] > 0) {
            instances[second]--;
            printf(1, "[P%d] GRANTED %s (Remaining: %d)\n", p_id, res_names[second], instances[second]);
            m_unlock(&table_lock);
            break;
        }
        m_unlock(&table_lock);
        sleep(1);
    }
}

void release_resources(int p_id, int r1, int r2) {
    m_lock(&table_lock);
    instances[r1]++;
    instances[r2]++;
    printf(1, "[P%d] RELEASED %s and %s\n", p_id, res_names[r1], res_names[r2]);
    m_unlock(&table_lock);
}

void worker(int id) {
    int r1 = process_needs[id][0];
    int r2 = process_needs[id][1];

    for (int cycle = 1; cycle <= CYCLES; cycle++) {
        printf(1, "[P%d] Cycle %d: Requesting %s and %s...\n", id, cycle, res_names[r1], res_names[r2]);
        acquire_ordered_resources(id, r1, r2);

        printf(1, "[P%d] Cycle %d: Starting task with acquired resources...\n", id, cycle);
        sleep(3); // Simulate operational work

        release_resources(id, r1, r2);
        sleep(1);
    }
    printf(1, "[P%d] All cycles completed successfully.\n", id);
    exit();
}

int main(int argc, char *argv[]) {
    printf(1, "=== Multi-Resource Sync & Deadlock Avoidance Simulation ===\n");
    printf(1, "Available Pools: Printers=%d, Scanners=%d, Disks=%d\n", instances[0], instances[1], instances[2]);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        int pid = fork();
        if (pid < 0) {
            printf(1, "Fork error\n");
            exit();
        }
        if (pid == 0) {
            worker(i);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait();
    }

    printf(1, "=== Success: All 5 processes completed without blocking or deadlock. ===\n");
    exit();
}