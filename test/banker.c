#include "types.h"
#include "stat.h"
#include "user.h"

#define N 5
#define M 3

int Allocation[N][M] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
};

int Max[N][M] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
};

int Available[M] = {3, 3, 2};
int Need[N][M];

void calculate_need() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            Need[i][j] = Max[i][j] - Allocation[i][j];
        }
    }
}

int is_safe(int safe_seq[]) {
    int Work[M];
    int Finish[N];

    for (int j = 0; j < M; j++) Work[j] = Available[j];
    for (int i = 0; i < N; i++) Finish[i] = 0;

    int count = 0;
    while (count < N) {
        int found = 0;
        for (int i = 0; i < N; i++) {
            if (!Finish[i]) {
                int can_allocate = 1;
                for (int j = 0; j < M; j++) {
                    if (Need[i][j] > Work[j]) {
                        can_allocate = 0;
                        break;
                    }
                }
                if (can_allocate) {
                    for (int j = 0; j < M; j++) Work[j] += Allocation[i][j];
                    safe_seq[count++] = i;
                    Finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) return 0;
    }
    return 1;
}

void request_resources(int p_id, int req[]) {
    printf(1, "\n--- Process P%d requesting: [%d, %d, %d] ---\n", p_id, req[0], req[1], req[2]);

    for (int j = 0; j < M; j++) {
        if (req[j] > Need[p_id][j]) {
            printf(1, "Error: Process P%d exceeded maximum claim.\n", p_id);
            return;
        }
    }

    for (int j = 0; j < M; j++) {
        if (req[j] > Available[j]) {
            printf(1, "Wait: Insufficient resources available.\n");
            return;
        }
    }

    for (int j = 0; j < M; j++) {
        Available[j] -= req[j];
        Allocation[p_id][j] += req[j];
        Need[p_id][j] -= req[j];
    }

    int safe_seq[N];
    if (is_safe(safe_seq)) {
        printf(1, "Request GRANTED. System is in SAFE state.\nSafe Sequence: ");
        for (int i = 0; i < N; i++) {
            printf(1, "P%d%s", safe_seq[i], (i == N - 1) ? "\n" : " -> ");
        }
    } else {
        for (int j = 0; j < M; j++) {
            Available[j] += req[j];
            Allocation[p_id][j] -= req[j];
            Need[p_id][j] += req[j];
        }
        printf(1, "Request denied -- would lead to unsafe state. Allocation rolled back.\n");
    }
}

int main(int argc, char *argv[]) {
    int safe_seq[N];
    calculate_need();

    printf(1, "=== Initial State Verification ===\n");
    if (is_safe(safe_seq)) {
        printf(1, "Initial system state is SAFE.\nSafe Sequence: ");
        for (int i = 0; i < N; i++) {
            printf(1, "P%d%s", safe_seq[i], (i == N - 1) ? "\n" : " -> ");
        }
    }

    // Scenario 1: Safe request
    int req1[M] = {1, 0, 2};
    request_resources(1, req1);

    // Scenario 2: Unsafe request
    int req2[M] = {3, 3, 0};
    request_resources(4, req2);

    exit();
}