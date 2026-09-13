#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_P 10

int adj[MAX_P][MAX_P];
int visited[MAX_P];
int rec_stack[MAX_P];
int parent_node[MAX_P];
int cycle_start = -1, cycle_end = -1;

void init_graph(int n) {
    for (int i = 0; i < n; i++) {
        visited[i] = 0;
        rec_stack[i] = 0;
        parent_node[i] = -1;
        for (int j = 0; j < n; j++) {
            adj[i][j] = 0;
        }
    }
    cycle_start = -1;
    cycle_end = -1;
}

// Build wait-for graph: If Pi requests Rk and Pj holds Rk, Pi -> Pj
void build_wait_for_graph(int n, int m, int alloc[][m], int req[][m]) {
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < m; k++) {
            if (req[i][k] > 0) {
                for (int j = 0; j < n; j++) {
                    if (i != j && alloc[j][k] > 0) {
                        adj[i][j] = 1;
                    }
                }
            }
        }
    }
}

int dfs(int u, int n) {
    visited[u] = 1;
    rec_stack[u] = 1;

    for (int v = 0; v < n; v++) {
        if (adj[u][v]) {
            if (!visited[v]) {
                parent_node[v] = u;
                if (dfs(v, n)) return 1;
            } else if (rec_stack[v]) {
                cycle_start = v;
                cycle_end = u;
                return 1;
            }
        }
    }
    rec_stack[u] = 0;
    return 0;
}

void print_cycle() {
    int cycle[MAX_P];
    int len = 0;
    cycle[len++] = cycle_start;
    for (int curr = cycle_end; curr != cycle_start && curr != -1; curr = parent_node[curr]) {
        cycle[len++] = curr;
    }
    cycle[len++] = cycle_start;

    printf(1, "Deadlock Cycle Detected: ");
    for (int i = len - 1; i >= 0; i--) {
        printf(1, "P%d%s", cycle[i], (i == 0) ? "\n" : " -> ");
    }
}

void run_scenario(char *title, int n, int m, int alloc[][m], int req[][m]) {
    printf(1, "\n========================================\n");
    printf(1, "%s\n", title);
    printf(1, "========================================\n");
    init_graph(n);
    build_wait_for_graph(n, m, alloc, req);

    printf(1, "Constructed Wait-For Graph Edges:\n");
    int edge_count = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (adj[i][j]) {
                printf(1, "  P%d -> P%d\n", i, j);
                edge_count++;
            }
        }
    }
    if (edge_count == 0) printf(1, "  (None)\n");

    int has_deadlock = 0;
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            if (dfs(i, n)) {
                has_deadlock = 1;
                break;
            }
        }
    }

    if (has_deadlock) {
        print_cycle();
    } else {
        printf(1, "Result: No Deadlock Detected (Acyclic wait-for graph).\n");
    }
}

int main(int argc, char *argv[]) {
    // Scenario 1: Acyclic (No deadlock)
    int n1 = 3, m1 = 3;
    int alloc1[3][3] = {
        {1, 0, 0}, // P0 holds R0
        {0, 1, 0}, // P1 holds R1
        {0, 0, 1}  // P2 holds R2
    };
    int req1[3][3] = {
        {0, 1, 0}, // P0 wants R1 (waits for P1)
        {0, 0, 1}, // P1 wants R2 (waits for P2)
        {0, 0, 0}  // P2 needs nothing
    };
    run_scenario("Scenario 1: Acyclic Wait-For Graph", n1, m1, alloc1, req1);

    // Scenario 2: 3-process cycle (Deadlock: P0 -> P1 -> P2 -> P0)
    int n2 = 4, m2 = 4;
    int alloc2[4][4] = {
        {1, 0, 0, 0}, // P0 holds R0
        {0, 1, 0, 0}, // P1 holds R1
        {0, 0, 1, 0}, // P2 holds R2
        {0, 0, 0, 1}  // P3 holds R3
    };
    int req2[4][4] = {
        {0, 1, 0, 0}, // P0 requests R1 -> waits on P1
        {0, 0, 1, 0}, // P1 requests R2 -> waits on P2
        {1, 0, 0, 0}, // P2 requests R0 -> waits on P0 (Cycle!)
        {0, 0, 1, 0}  // P3 requests R2 -> waits on P2 (not in cycle)
    };
    run_scenario("Scenario 2: Circular Wait (3+ Processes)", n2, m2, alloc2, req2);

    exit();
}