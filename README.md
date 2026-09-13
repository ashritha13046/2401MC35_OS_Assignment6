# Operating Systems Lab - Assignment 6: Deadlock Handling & Synchronization in xv6

## Overview

This implementation provides solutions for four deadlock and synchronization problems in xv6-riscv:

1. Banker's Algorithm for deadlock avoidance.
2. Deadlock detection using a Resource Allocation / Wait-For Graph.
3. Deadlock prevention using resource ordering.
4. Combined synchronization and deadlock avoidance using multiple shared resources.

## Deadlock Handling & Architectural Decisions

### 1. Banker's Algorithm (`user/bankers.c`)

The Banker's Algorithm is implemented as a user-space xv6 program using the standard resource-allocation structures:

- `Allocation[n][m]`: Resources currently allocated to each process.
- `Max[n][m]`: Maximum resource requirement of each process.
- `Available[m]`: Currently available instances of each resource.
- `Need[n][m]`: Remaining resource requirement, calculated as `Max - Allocation`.

A hardcoded scenario containing 5 processes and 3 resource types is used.

The program implements:

- **Safety Algorithm**: Determines whether the system is in a safe state and prints a valid safe sequence.
- **Resource Request Algorithm**: Checks whether a request can be granted, temporarily allocates the resources, runs the Safety Algorithm, and either commits the allocation or rolls it back if the resulting state is unsafe.

Two test scenarios are included:

- A request that is safely granted.
- A request that is rejected because it would result in an unsafe state.

### 2. Deadlock Detection (`user/deadlockdetect.c`)

Deadlock detection is implemented using a **Wait-For Graph** constructed from the allocation and request matrices.

An edge:

`Pi -> Pj`

indicates that process `Pi` is waiting for a resource currently held by process `Pj`.

A DFS-based cycle detection algorithm is used to determine whether a deadlock exists.

Two hardcoded scenarios are tested:

- An acyclic graph with no deadlock.
- A cyclic graph demonstrating deadlock involving three or more processes.

When a deadlock is detected, the exact cycle of processes is printed.

### 3. Resource Ordering (`user/resourceorder_bad.c` and `user/resourceorder_fixed.c`)

Deadlock prevention is demonstrated using **resource ordering** with two shared locks.

#### Deadlocking Version

Two processes acquire the locks in opposite orders:

- Process A: `Lock1 -> Lock2`
- Process B: `Lock2 -> Lock1`

A deliberate delay is introduced between lock acquisitions to make the circular wait reproducible.

This demonstrates the classic deadlock condition where each process holds one resource while waiting for the other.

#### Fixed Version

Both processes acquire resources using the same global ordering:

`Lock1 -> Lock2`

Consistent resource ordering eliminates circular wait and allows both processes to complete successfully.

### 4. Combined Synchronization & Deadlock Avoidance (`user/syncdeadlock.c`)

A multi-resource synchronization problem is implemented using 5 xv6 processes.

The system models three shared resource types with limited instances, and each process requires two resources to complete its work.

Locks/semaphores are used to protect access to the shared resource pools.

A deadlock prevention/avoidance strategy is applied so that:

- Resources are not over-allocated.
- Processes do not remain permanently blocked.
- All processes can complete their work.
- Resources are correctly released after use.

Each process prints its resource requests, grants, work execution, and resource releases.

## Files Added

- `bankers.c`: Banker's Algorithm simulation.
- `deadlockdetect.c`: Wait-For Graph based deadlock detection.
- `resourceorder_bad.c`: Demonstrates deadlock using inconsistent resource ordering.
- `resourceorder_fixed.c`: Prevents deadlock using consistent resource ordering.
- `syncdeadlock.c`: Multi-process synchronization with deadlock avoidance.

## Output Logs

The `Output/` directory contains execution logs for all four questions:

- `q1_output.txt`: Banker's Algorithm output.
- `q2_output.txt`: Deadlock detection output.
- `q3_bad_output.txt`: Deadlocking resource-ordering scenario.
- `q3_fixed_output.txt`: Deadlock-free resource-ordering scenario.
- `q4_output.txt`: Combined synchronization and deadlock-avoidance output.

## Project Structure

    OS_Lab_Assignment_6/
    ├── bankers.c
    ├── deadlockdetect.c
    ├── resourceorder_bad.c
    ├── resourceorder_fixed.c
    ├── syncdeadlock.c
    ├── Output/
    │   ├── q1_output.txt
    │   ├── q2_output.txt
    │   ├── q3_bad_output.txt
    │   ├── q3_fixed_output.txt
    │   └── q4_output.txt
    ├── WriteUP_OSLAB6.pdf
    └── README.md

## Building and Running

Copy the user programs into the xv6 `user/` directory and add their corresponding entries to `UPROGS` in the xv6 `Makefile`.

Build and start xv6:

```bash
make clean
make
make qemu
