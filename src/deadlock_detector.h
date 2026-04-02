#pragma once
#include "resource_manager.h"
#include <vector>
#include <string>
#include <set>


struct DetectionResult {
    bool deadlockFound;
    std::vector<int> deadlockedProcesses; // IDs of processes in deadlock
    std::vector<std::pair<int,int>> cycle; // Edges forming the cycle
    std::string message;
};

// Detects deadlocks using Wait-For Graph (WFG) cycle detection
class DeadlockDetector {
public:
    explicit DeadlockDetector(ResourceManager& rm) : rm(rm) {}

    // Run deadlock detection on current state
    DetectionResult detect();

    // Display the Wait-For Graph
    void displayWaitForGraph() const;

    // Display detection report
    void displayReport(const DetectionResult& result) const;

private:
    ResourceManager& rm;

    // Build adjacency list for Wait-For Graph
    // Edge i->j means process i is waiting for a resource held by process j
    std::vector<std::vector<int>> buildWaitForGraph() const;

    // DFS-based cycle detection
    bool dfsCycle(int v,
                  const std::vector<std::vector<int>>& graph,
                  std::vector<int>& color,   // 0=white,1=gray,2=black
                  std::vector<int>& parent,
                  std::vector<int>& cycleNodes) const;
};
