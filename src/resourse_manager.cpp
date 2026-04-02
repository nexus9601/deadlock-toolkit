#include "resource_manager.h"
#include <iomanip>

void ResourceManager::addResource(const std::string& name, int total) {
    int id = (int)resources.size();
    resources.emplace_back(id, name, total);
}

void ResourceManager::addProcess(const std::string& name, int priority) {
    int id = (int)processes.size();
    Process p(id, name, (int)resources.size(), priority);
    processes.push_back(p);
}

void ResourceManager::setMaximum(int pid, const std::vector<int>& max) {
    if (pid < 0 || pid >= (int)processes.size()) return;
    processes[pid].maximum = max;
    processes[pid].computeNeed();
}

void ResourceManager::setAllocation(int pid, const std::vector<int>& alloc) {
    if (pid < 0 || pid >= (int)processes.size()) return;
    // Release old allocation
    for (int j = 0; j < (int)resources.size(); ++j)
        resources[j].available += processes[pid].allocation[j];
    // Set new allocation
    processes[pid].allocation = alloc;
    // Deduct from available
    for (int j = 0; j < (int)resources.size(); ++j)
        resources[j].available -= alloc[j];
    processes[pid].computeNeed();
}

std::vector<int> ResourceManager::getAvailable() const {
    std::vector<int> avail;
    for (const auto& r : resources)
        avail.push_back(r.available);
    return avail;
}

void ResourceManager::displayState() const {
    int n = numProcesses();
    int m = numResources();

    std::cout << "\n========== SYSTEM STATE ==========\n";

    // Header
    std::cout << std::left << std::setw(10) << "Process";
    for (auto& r : resources) std::cout << std::setw(6) << r.name;
    std::cout << " | ";
    for (auto& r : resources) std::cout << std::setw(6) << r.name;
    std::cout << " | ";
    for (auto& r : resources) std::cout << std::setw(6) << r.name;
    std::cout << "\n";

    std::cout << std::setw(10) << "";
    std::cout << "--- Allocation ---   --- Maximum ---   ----- Need -----\n";

    for (int i = 0; i < n; ++i) {
        const auto& p = processes[i];
        std::cout << std::left << std::setw(10) << p.name;
        for (int j = 0; j < m; ++j) std::cout << std::setw(6) << p.allocation[j];
        std::cout << " | ";
        for (int j = 0; j < m; ++j) std::cout << std::setw(6) << p.maximum[j];
        std::cout << " | ";
        for (int j = 0; j < m; ++j) std::cout << std::setw(6) << p.need[j];
        std::cout << "\n";
    }

    std::cout << "\nAvailable: ";
    for (auto& r : resources) std::cout << r.name << "=" << r.available << "  ";
    std::cout << "\n==================================\n";
}

void ResourceManager::displayTable() const {
    std::cout << "\n--- Resource Summary ---\n";
    std::cout << std::left << std::setw(12) << "Resource"
              << std::setw(8)  << "Total"
              << std::setw(12) << "Allocated"
              << std::setw(10) << "Available" << "\n";
    for (const auto& r : resources) {
        int allocated = r.total - r.available;
        std::cout << std::setw(12) << r.name
                  << std::setw(8)  << r.total
                  << std::setw(12) << allocated
                  << std::setw(10) << r.available << "\n";
    }
}

void ResourceManager::reset() {
    for (auto& p : processes) {
        p.finished  = false;
        p.deadlocked = false;
    }
    // Recompute available from total - sum of allocations
    for (auto& r : resources) r.available = r.total;
    for (auto& p : processes)
        for (int j = 0; j < (int)resources.size(); ++j)
            resources[j].available -= p.allocation[j];
}
