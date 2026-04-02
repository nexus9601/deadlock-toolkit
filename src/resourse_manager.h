#pragma once
#include "process.h"
#include "resource.h"
#include <vector>
#include <string>
#include <iostream>

// Manages all processes and resources in the system
class ResourceManager {
public:
    std::vector<Process>  processes;
    std::vector<Resource> resources;

    ResourceManager() = default;

    // Add a new resource type
    void addResource(const std::string& name, int total);

    // Add a new process
    void addProcess(const std::string& name, int priority = 1);

    // Set maximum demand for a process
    void setMaximum(int pid, const std::vector<int>& max);

    // Set current allocation for a process
    void setAllocation(int pid, const std::vector<int>& alloc);

    // Get available resources vector
    std::vector<int> getAvailable() const;

    // Display current state (matrices)
    void displayState() const;

    // Display resource allocation table
    void displayTable() const;

    // Reset all processes (for simulation restart)
    void reset();

    int numProcesses() const { return (int)processes.size(); }
    int numResources() const { return (int)resources.size(); }
};
