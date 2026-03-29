#include "banker.h"
#include <iostream>
#include <iomanip>
#include <numeric>

BankerResult BankersAlgorithm::checkSafety() const {
    int n = rm.numProcesses();
    int m = rm.numResources();

    std::vector<std::vector<int>> need(n, std::vector<int>(m));
    std::vector<std::vector<int>> alloc(n, std::vector<int>(m));

    for (int i = 0; i < n; ++i) {
        need[i]  = rm.processes[i].need;
        alloc[i] = rm.processes[i].allocation;
    }
    return runSafetyAlgorithm(need, alloc, rm.getAvailable());
}

BankerResult BankersAlgorithm::runSafetyAlgorithm(
    const std::vector<std::vector<int>>& need,
    const std::vector<std::vector<int>>& allocation,
    const std::vector<int>& available) const
{
    int n = (int)need.size();
    int m = (int)available.size();

    std::vector<int>  work   = available;
    std::vector<bool> finish(n, false);
    std::vector<int>  safeSeq;

    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n; ++i) {
            if (finish[i]) continue;
            // Check if need[i] <= work
            bool canRun = true;
            for (int j = 0; j < m; ++j) {
                if (need[i][j] > work[j]) { canRun = false; break; }
            }
            if (canRun) {
                // Simulate: release allocation back to work
                for (int j = 0; j < m; ++j) work[j] += allocation[i][j];
                finish[i] = true;
                safeSeq.push_back(i);
                progress = true;
            }
        }
    }

    BankerResult result;
    result.isSafe = ((int)safeSeq.size() == n);
    result.safeSequence = safeSeq;
    result.message = result.isSafe
        ? "System is in a SAFE state."
        : "System is in an UNSAFE state — deadlock risk!";
    return result;
}

RequestResult BankersAlgorithm::requestResources(int pid, const std::vector<int>& request) {
    int m = rm.numResources();
    RequestResult result;

    // Step 1: Validate request against need
    for (int j = 0; j < m; ++j) {
        if (request[j] > rm.processes[pid].need[j]) {
            result.granted = false;
            result.message = "Error: Request exceeds declared maximum need!";
            return result;
        }
    }

    // Step 2: Validate against available
    std::vector<int> avail = rm.getAvailable();
    for (int j = 0; j < m; ++j) {
        if (request[j] > avail[j]) {
            result.granted = false;
            result.message = "Resources not available — process must wait.";
            return result;
        }
    }

    // Step 3: Pretend to allocate
    int n = rm.numProcesses();
    std::vector<std::vector<int>> tempNeed(n, std::vector<int>(m));
    std::vector<std::vector<int>> tempAlloc(n, std::vector<int>(m));
    std::vector<int> tempAvail = avail;

    for (int i = 0; i < n; ++i) {
        tempNeed[i]  = rm.processes[i].need;
        tempAlloc[i] = rm.processes[i].allocation;
    }
    for (int j = 0; j < m; ++j) {
        tempAvail[j]            -= request[j];
        tempAlloc[pid][j]       += request[j];
        tempNeed[pid][j]        -= request[j];
    }

    // Step 4: Run safety check on tentative state
    BankerResult safety = runSafetyAlgorithm(tempNeed, tempAlloc, tempAvail);

    if (safety.isSafe) {
        // Commit the allocation
        rm.setAllocation(pid, tempAlloc[pid]);
        result.granted = true;
        result.message = "Request GRANTED. System remains safe.";
    } else {
        result.granted = false;
        result.message = "Request DENIED. Granting would lead to unsafe state!";
    }
    return result;
}

void BankersAlgorithm::displaySafetyCheck() const {
    int n = rm.numProcesses();
    int m = rm.numResources();

    std::cout << "\n====== BANKER'S ALGORITHM — SAFETY CHECK ======\n";
    rm.displayState();

    std::vector<int>  work   = rm.getAvailable();
    std::vector<bool> finish(n, false);
    std::vector<int>  safeSeq;

    std::cout << "\nStep-by-step simulation:\n";
    std::cout << std::left
              << std::setw(5)  << "Step"
              << std::setw(12) << "Process"
              << std::setw(18) << "Need"
              << std::setw(18) << "Work(before)"
              << std::setw(18) << "Work(after)"
              << "Status\n";
    std::cout << std::string(80, '-') << "\n";

    int step = 1;
    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n; ++i) {
            if (finish[i]) continue;
            bool canRun = true;
            for (int j = 0; j < m; ++j)
                if (rm.processes[i].need[j] > work[j]) { canRun = false; break; }

            if (canRun) {
                std::vector<int> workBefore = work;
                for (int j = 0; j < m; ++j) work[j] += rm.processes[i].allocation[j];
                finish[i] = true;
                safeSeq.push_back(i);

                // Print step
                std::cout << std::setw(5) << step++;
                std::cout << std::setw(12) << rm.processes[i].name;
                std::string needStr = "[";
                for (int j = 0; j < m; ++j) needStr += std::to_string(rm.processes[i].need[j]) + (j<m-1?",":"");
                needStr += "]";
                std::cout << std::setw(18) << needStr;
                std::string wbStr = "[";
                for (int j = 0; j < m; ++j) wbStr += std::to_string(workBefore[j]) + (j<m-1?",":"");
                wbStr += "]";
                std::cout << std::setw(18) << wbStr;
                std::string waStr = "[";
                for (int j = 0; j < m; ++j) waStr += std::to_string(work[j]) + (j<m-1?",":"");
                waStr += "]";
                std::cout << std::setw(18) << waStr;
                std::cout << "CAN EXECUTE\n";
                progress = true;
            }
        }
    }

    std::cout << std::string(80, '-') << "\n";
    bool safe = ((int)safeSeq.size() == n);
    if (safe) {
        std::cout << "\n[SAFE] Safe Sequence: ";
        for (int i = 0; i < (int)safeSeq.size(); ++i) {
            std::cout << rm.processes[safeSeq[i]].name;
            if (i + 1 < (int)safeSeq.size()) std::cout << " -> ";
        }
        std::cout << "\n";
    } else {
        std::cout << "\n[UNSAFE] Deadlock risk detected!\n";
        for (int i = 0; i < n; ++i)
            if (!finish[i])
                std::cout << "  Process " << rm.processes[i].name << " cannot complete.\n";
    }
    std::cout << "===============================================\n";
}
