#pragma once
#include "resource_manager.h"
#include <vector>
#include <string>

// Result of Banker's Algorithm
struct BankerResult {
    bool isSafe;
    std::vector<int> safeSequence; // Order of process execution
    std::string message;
};

// Request result
struct RequestResult {
    bool granted;
    std::string message;
};

// Implements Banker's Algorithm for deadlock prevention
class BankersAlgorithm {
public:
    explicit BankersAlgorithm(ResourceManager& rm) : rm(rm) {}

    // Run the Safety Algorithm on current state
    // Returns a BankerResult with safe sequence if safe
    BankerResult checkSafety() const;

    // Handle resource request from process pid
    // Returns whether request was granted
    RequestResult requestResources(int pid, const std::vector<int>& request);

    // Display detailed step-by-step safety check
    void displaySafetyCheck() const;

private:
    ResourceManager& rm;

    // Internal safety check on given matrices (used for what-if analysis)
    BankerResult runSafetyAlgorithm(
        const std::vector<std::vector<int>>& need,
        const std::vector<std::vector<int>>& allocation,
        const std::vector<int>& available) const;
};
