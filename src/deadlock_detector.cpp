#include "deadlock_detector.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// Build Wait-For Graph:
// Process i waits for process j if:
//   i needs a resource type that j currently holds
std::vector<std::vector<int>> DeadlockDetector::buildWaitForGraph() const {
    int n = rm.numProcesses();
    int m = rm.numResources();
    std::vector<std::vector<int>> graph(n);

    for (int i = 0; i < n; ++i) {
        if (rm.processes[i].finished) continue;
        // Check if process i is waiting (need > 0 and cannot proceed)
        std::vector<int> avail = rm.getAvailable();
        bool waiting = false;
        for (int j = 0; j < m; ++j)
            if (rm.processes[i].need[j] > avail[j]) { waiting = true; break; }

        if (!waiting) continue;

        // Find which resources process i is waiting for
        for (int resType = 0; resType < m; ++resType) {
            if (rm.processes[i].need[resType] <= avail[resType]) continue;
            // Process i needs resType but not enough available
            // Find who holds resType
            for (int j = 0; j < n; ++j) {
                if (i == j || rm.processes[j].finished) continue;
                if (rm.processes[j].allocation[resType] > 0) {
                    // i waits for j
                    graph[i].push_back(j);
                }
            }
        }

        // Remove duplicate edges
        std::sort(graph[i].begin(), graph[i].end());
        graph[i].erase(std::unique(graph[i].begin(), graph[i].end()), graph[i].end());
    }
    return graph;
}

bool DeadlockDetector::dfsCycle(int v,
    const std::vector<std::vector<int>>& graph,
    std::vector<int>& color,
    std::vector<int>& parent,
    std::vector<int>& cycleNodes) const
{
    color[v] = 1; // gray — in current DFS path
    for (int u : graph[v]) {
        if (color[u] == 1) {
            // Found back edge => cycle
            // Trace cycle from u to v
            cycleNodes.push_back(u);
            int cur = v;
            while (cur != u) {
                cycleNodes.push_back(cur);
                cur = parent[cur];
            }
            cycleNodes.push_back(u); // close the cycle
            std::reverse(cycleNodes.begin(), cycleNodes.end());
            return true;
        }
        if (color[u] == 0) {
            parent[u] = v;
            if (dfsCycle(u, graph, color, parent, cycleNodes))
                return true;
        }
    }
    color[v] = 2; // black — fully processed
    return false;
}

DetectionResult DeadlockDetector::detect() {
    int n = rm.numProcesses();
    auto graph = buildWaitForGraph();

    std::vector<int> color(n, 0);
    std::vector<int> parent(n, -1);
    std::vector<int> cycleNodes;

    DetectionResult result;
    result.deadlockFound = false;

    for (int i = 0; i < n; ++i) {
        if (color[i] == 0) {
            if (dfsCycle(i, graph, color, parent, cycleNodes)) {
                result.deadlockFound = true;
                break;
            }
        }
    }

    if (result.deadlockFound) {
        // Unique deadlocked processes
        std::set<int> dlSet(cycleNodes.begin(), cycleNodes.end());
        result.deadlockedProcesses = std::vector<int>(dlSet.begin(), dlSet.end());
        for (auto& p : rm.processes)
            if (dlSet.count(p.id)) p.deadlocked = true;
        result.message = "DEADLOCK DETECTED involving " +
                         std::to_string(result.deadlockedProcesses.size()) + " processes.";
    } else {
        result.message = "No deadlock detected. System is running normally.";
    }
    return result;
}

void DeadlockDetector::displayWaitForGraph() const {
    auto graph = buildWaitForGraph();
    int n = rm.numProcesses();
    std::cout << "\n====== WAIT-FOR GRAPH ======\n";
    bool anyEdge = false;
    for (int i = 0; i < n; ++i) {
        for (int j : graph[i]) {
            std::cout << "  " << rm.processes[i].name
                      << "  -->  " << rm.processes[j].name
                      << "  (waiting for resource held by "
                      << rm.processes[j].name << ")\n";
            anyEdge = true;
        }
    }
    if (!anyEdge) std::cout << "  No wait-for edges — no waiting processes.\n";
    std::cout << "============================\n";
}

void DeadlockDetector::displayReport(const DetectionResult& result) const {
    std::cout << "\n====== DEADLOCK DETECTION REPORT ======\n";
    std::cout << result.message << "\n";
    if (result.deadlockFound) {
        std::cout << "\nDeadlocked Processes:\n";
        for (int pid : result.deadlockedProcesses)
            std::cout << "  [" << pid << "] " << rm.processes[pid].name << "\n";
        std::cout << "\nThese processes are waiting in a circular dependency\n"
                  << "and will never proceed without intervention.\n";
    }
    std::cout << "========================================\n";
}
