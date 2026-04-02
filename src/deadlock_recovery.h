#pragma once
#include "resource_manager.h"
#include "deadlock_detector.h"
#include <vector>
#include <string>

enum class RecoveryStrategy {
    TERMINATE_ALL,       // Abort all deadlocked processes at once
    TERMINATE_ONE,       // Abort one process at a time (least priority first)
    PREEMPT_RESOURCES    // Preempt resources from victim process
};

struct RecoveryResult {
    bool success;
    std::string message;
    std::vector<std::string> actionsLog; // Step-by-step actions taken
    std::vector<int> terminatedProcesses;
    std::vector<int> preemptedFrom;
};

// Handles deadlock recovery after detection
class DeadlockRecovery {
public:
    explicit DeadlockRecovery(ResourceManager& rm) : rm(rm) {}

    // Recover using the given strategy
    RecoveryResult recover(const DetectionResult& detection,
                           RecoveryStrategy strategy);

    // Display recovery report
    void displayReport(const RecoveryResult& result) const;

    // Compute cost of terminating a process (based on priority & work done)
    int terminationCost(int pid) const;

private:
    ResourceManager& rm;

    RecoveryResult terminateAll(const DetectionResult& detection);
    RecoveryResult terminateOne(const DetectionResult& detection);
    RecoveryResult preemptResources(const DetectionResult& detection);

    // Release all resources held by a process
    void releaseProcess(int pid, std::vector<std::string>& log);
};
