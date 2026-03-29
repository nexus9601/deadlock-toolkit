/**
 * Deadlock Toolkit — Real-Time Detection, Prevention & Recovery
 *
 * Entry point for the command-line interface.
 * Implements:
 *   - Banker's Algorithm (deadlock prevention)
 *   - Wait-For Graph cycle detection (deadlock detection)
 *   - Process termination & resource preemption (deadlock recovery)
 *   - Custom scenario simulation
 *
 * Build: make
 * Run:   ./deadlock_toolkit
 */

#include "simulation.h"

int main() {
    Simulation sim;
    sim.run();
    return 0;
}
