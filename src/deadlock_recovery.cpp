#include "deadlock_recovery.h"
#include <iostream>
#include <algorithm>
#include <numeric>

void DeadlockRecovery::releaseProcess(int pid, std::vector<std::string>& log) {
    auto& p = rm.processes[pid];
    std::string entry = "  Terminating " + p.name + ": releasing resources [";
    for (int j = 0; j < rm.numResources(); ++j) {
        rm.resources[j].available += p.allocation[j];
        entry += rm.resources[j].name + "=" + std::to_string(p.allocation[j]);
        if (j + 1 < rm.numResources()) entry += ", ";
        p.allocation[j] = 0;
        p.need[j] = p.maximum[j];
    }
    entry += "]";
    p.finished  = true;
    p.deadlocked = false;
    log.push_back(entry);
}

int DeadlockRecovery::terminationCost(int pid) const {
    // Cost = inversePriority * totalAllocation
    const auto& p = rm.processes[pid];
    int totalAlloc = 0;
    for (int a : p.allocation) totalAlloc += a;
    // Lower priority and less work done = cheaper to terminate
    return (11 - p.priority) * 10 + totalAlloc;
}

RecoveryResult DeadlockRecovery::terminateAll(const DetectionResult& detection) {
    RecoveryResult result;
    result.actionsLog.push_back("Strategy: TERMINATE ALL deadlocked processes.");
    for (int pid : detection.deadlockedProcesses) {
        releaseProcess(pid, result.actionsLog);
        result.terminatedProcesses.push_back(pid);
    }
    result.success = true;
    result.message = "Terminated " + std::to_string(result.terminatedProcesses.size()) +
                     " processes. Deadlock resolved.";
    return result;
}

RecoveryResult DeadlockRecovery::terminateOne(const DetectionResult& detection) {
    RecoveryResult result;
    result.actionsLog.push_back("Strategy: TERMINATE ONE process at a time.");

    // Sort by termination cost (ascending — cheapest first)
    std::vector<int> victims = detection.deadlockedProcesses;
    std::sort(victims.begin(), victims.end(), [&](int a, int b) {
        return terminationCost(a) < terminationCost(b);
    });

    DeadlockDetector detector(rm);
    for (int pid : victims) {
        result.actionsLog.push_back("  Checking if deadlock still exists...");
        auto check = detector.detect();
        if (!check.deadlockFound) break;

        result.actionsLog.push_back("  Deadlock persists. Choosing victim with lowest cost.");
        releaseProcess(pid, result.actionsLog);
        result.terminatedProcesses.push_back(pid);
    }

    result.success = true;
    result.message = "Terminated " + std::to_string(result.terminatedProcesses.size()) +
                     " process(es) to resolve deadlock.";
    return result;
}

RecoveryResult DeadlockRecovery::preemptResources(const DetectionResult& detection) {
    RecoveryResult result;
    result.actionsLog.push_back("Strategy: RESOURCE PREEMPTION.");

    // Pick victim = deadlocked process with highest allocation (most to preempt)
    int victim = -1;
    int maxAlloc = -1;
    for (int pid : detection.deadlockedProcesses) {
        int total = 0;
        for (int a : rm.processes[pid].allocation) total += a;
        if (total > maxAlloc) { maxAlloc = total; victim = pid; }
    }

    if (victim == -1) {
        result.success = false;
        result.message = "No suitable victim for preemption.";
        return result;
    }

    auto& p = rm.processes[victim];
    result.actionsLog.push_back("  Victim selected: " + p.name);
    result.actionsLog.push_back("  Preempting all resources from " + p.name);

    // Preempt — release resources back to pool
    std::string entry = "  Releasing from " + p.name + ": [";
    for (int j = 0; j < rm.numResources(); ++j) {
        rm.resources[j].available += p.allocation[j];
        entry += rm.resources[j].name + "=" + std::to_string(p.allocation[j]);
        if (j + 1 < rm.numResources()) entry += ", ";
        p.allocation[j] = 0;
        p.need[j] = p.maximum[j];
    }
    entry += "]";
    result.actionsLog.push_back(entry);
    result.actionsLog.push_back("  Process " + p.name + " rolled back to initial state.");
    result.preemptedFrom.push_back(victim);
    p.deadlocked = false;

    result.success = true;
    result.message = "Preempted resources from " + p.name + ". Deadlock may be resolved.";
    return result;
}

RecoveryResult DeadlockRecovery::recover(const DetectionResult& detection,
                                          RecoveryStrategy strategy) {
    if (!detection.deadlockFound) {
        return {false, "No deadlock to recover from.", {}, {}, {}};
    }
    switch (strategy) {
        case RecoveryStrategy::TERMINATE_ALL:      return terminateAll(detection);
        case RecoveryStrategy::TERMINATE_ONE:      return terminateOne(detection);
        case RecoveryStrategy::PREEMPT_RESOURCES:  return preemptResources(detection);
        default:                                   return terminateAll(detection);
    }
}

void DeadlockRecovery::displayReport(const RecoveryResult& result) const {
    std::cout << "\n====== RECOVERY REPORT ======\n";
    for (const auto& log : result.actionsLog)
        std::cout << log << "\n";
    std::cout << "\nOutcome: " << result.message << "\n";
    if (!result.terminatedProcesses.empty()) {
        std::cout << "Terminated: ";
        for (int pid : result.terminatedProcesses)
            std::cout << rm.processes[pid].name << " ";
        std::cout << "\n";
    }
    std::cout << "=============================\n";
}
