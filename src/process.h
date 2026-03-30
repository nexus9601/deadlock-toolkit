#pragma once
#include <vector>
#include <string>

// Represents a process in the system
struct Process {
    int id;
    std::string name;
    std::vector<int> allocation;  // Resources currently allocated
    std::vector<int> maximum;     // Maximum resources needed
    std::vector<int> need;        // Remaining need = maximum - allocation
    int priority;                 // Priority (used in recovery selection)
    bool finished;                // Finished executing?
    bool deadlocked;              // Currently in deadlock?

    Process() : id(-1), priority(0), finished(false), deadlocked(false) {}

    Process(int id, const std::string& name, int numResources, int priority = 1)
        : id(id), name(name),
          allocation(numResources, 0),
          maximum(numResources, 0),
          need(numResources, 0),
          priority(priority),
          finished(false),
          deadlocked(false) {}

    // Recompute need from maximum - allocation
    void computeNeed() {
        for (size_t j = 0; j < allocation.size(); ++j)
            need[j] = maximum[j] - allocation[j];
    }

    // Check if this process can proceed given available resources
    bool canProceed(const std::vector<int>& available) const {
        for (size_t j = 0; j < need.size(); ++j)
            if (need[j] > available[j]) return false;
        return true;
    }
};
