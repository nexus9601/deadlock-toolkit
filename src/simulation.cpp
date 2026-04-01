#include "simulation.h"
#include <iostream>
#include <string>
#include <limits>

void Simulation::printHeader() const {
    std::cout << R"(
 ____                 _ _            _    _____           _ _    _ _   
|  _ \  ___  __ _  __| | | ___   ___| | _|_   _|__   ___ | | | _(_) |_ 
| | | |/ _ \/ _` |/ _` | |/ _ \ / __| |/ / | |/ _ \ / _ \| | |/ / | __|
| |_| |  __/ (_| | (_| | | (_) | (__|   <  | | (_) | (_) | |   <| | |_ 
|____/ \___|\__,_|\__,_|_|\___/ \___|_|\_\ |_|\___/ \___/|_|_|\_\_|\__|
                                                                          
       Real-Time Deadlock Detection, Prevention & Recovery Toolkit
)";
}

void Simulation::printMenu() const {
    std::cout << "\n========= MAIN MENU =========\n";
    std::cout << " 1. Custom Setup (define processes & resources)\n";
    std::cout << " 2. Load Preloaded Scenario\n";
    std::cout << " 3. Run Banker's Algorithm (Safety Check)\n";
    std::cout << " 4. Request Resource (Banker's Request Algorithm)\n";
    std::cout << " 5. Detect Deadlock (Wait-For Graph)\n";
    std::cout << " 6. Recover from Deadlock\n";
    std::cout << " 7. Display Current System State\n";
    std::cout << " 8. Reset System\n";
    std::cout << " 0. Exit\n";
    std::cout << "=============================\n";
    std::cout << "Enter choice: ";
}

void Simulation::initAlgorithms() {
    cleanup();
    banker   = new BankersAlgorithm(rm);
    detector = new DeadlockDetector(rm);
    recovery = new DeadlockRecovery(rm);
}

void Simulation::cleanup() {
    delete banker;   banker   = nullptr;
    delete detector; detector = nullptr;
    delete recovery; recovery = nullptr;
}

void Simulation::run() {
    printHeader();
    int choice = -1;
    while (choice != 0) {
        printMenu();
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        switch (choice) {
            case 1: menuSetup();           break;
            case 2: menuLoadScenario();    break;
            case 3: menuRunBankersCheck(); break;
            case 4: menuRequestResource(); break;
            case 5: menuDetectDeadlock();  break;
            case 6: menuRecover();         break;
            case 7: menuDisplayState();    break;
            case 8: menuReset();           break;
            case 0: std::cout << "Exiting. Goodbye!\n"; break;
            default: std::cout << "Invalid choice.\n";
        }
    }
    cleanup();
}

void Simulation::menuSetup() {
    rm = ResourceManager();
    int r, p;
    std::cout << "\n--- Custom Setup ---\n";
    std::cout << "Number of resource types: "; std::cin >> r;
    for (int i = 0; i < r; ++i) {
        std::string name; int total;
        std::cout << "  Resource " << i << " name: "; std::cin >> name;
        std::cout << "  Total instances: "; std::cin >> total;
        rm.addResource(name, total);
    }
    std::cout << "Number of processes: "; std::cin >> p;
    for (int i = 0; i < p; ++i) {
        std::string name; int priority;
        std::cout << "  Process " << i << " name: "; std::cin >> name;
        std::cout << "  Priority (1-10): "; std::cin >> priority;
        rm.addProcess(name, priority);
        std::vector<int> maxV(r), allocV(r);
        std::cout << "  Maximum demand [" << r << " values]: ";
        for (int j = 0; j < r; ++j) std::cin >> maxV[j];
        std::cout << "  Current allocation [" << r << " values]: ";
        for (int j = 0; j < r; ++j) std::cin >> allocV[j];
        rm.setMaximum(i, maxV);
        rm.setAllocation(i, allocV);
    }
    initAlgorithms();
    std::cout << "Setup complete!\n";
    rm.displayState();
}

void Simulation::menuLoadScenario() {
    std::cout << "\nSelect scenario:\n";
    std::cout << " 1. Banker's Classic (safe state)\n";
    std::cout << " 2. Deadlock Scenario\n";
    std::cout << " 3. Dining Philosophers (deadlock)\n";
    int s; std::cin >> s;
    rm = ResourceManager();
    if      (s == 1) loadBankersClassicExample();
    else if (s == 2) loadDeadlockScenario();
    else if (s == 3) loadDiningPhilosophers();
    else { std::cout << "Invalid.\n"; return; }
    initAlgorithms();
    rm.displayState();
}

void Simulation::loadBankersClassicExample() {
    // Classic Banker's example from OS textbooks
    // 3 resources: A(10), B(5), C(7)
    rm.addResource("A", 10);
    rm.addResource("B", 5);
    rm.addResource("C", 7);
    // 5 processes
    rm.addProcess("P0", 5); rm.addProcess("P1", 8);
    rm.addProcess("P2", 6); rm.addProcess("P3", 4);
    rm.addProcess("P4", 7);
    // Allocations
    rm.setAllocation(0, {0,1,0}); rm.setAllocation(1, {2,0,0});
    rm.setAllocation(2, {3,0,2}); rm.setAllocation(3, {2,1,1});
    rm.setAllocation(4, {0,0,2});
    // Maximum
    rm.setMaximum(0, {7,5,3}); rm.setMaximum(1, {3,2,2});
    rm.setMaximum(2, {9,0,2}); rm.setMaximum(3, {2,2,2});
    rm.setMaximum(4, {4,3,3});
    std::cout << "Loaded: Banker's Classic Example (SAFE state expected)\n";
}

void Simulation::loadDeadlockScenario() {
    // 2 resources, 2 processes — classic deadlock
    rm.addResource("R1", 1);
    rm.addResource("R2", 1);
    rm.addProcess("P0", 5);
    rm.addProcess("P1", 5);
    // P0 holds R1, needs R2; P1 holds R2, needs R1 => deadlock
    rm.setAllocation(0, {1,0}); rm.setMaximum(0, {1,1});
    rm.setAllocation(1, {0,1}); rm.setMaximum(1, {1,1});
    std::cout << "Loaded: Deadlock Scenario (DEADLOCK expected)\n";
}

void Simulation::loadDiningPhilosophers() {
    // 5 philosophers, 5 forks — deadlock scenario
    int n = 5;
    for (int i = 0; i < n; ++i)
        rm.addResource("Fork" + std::to_string(i), 1);
    for (int i = 0; i < n; ++i)
        rm.addProcess("Phil" + std::to_string(i), 5);
    // Each philosopher holds their left fork, waiting for right
    for (int i = 0; i < n; ++i) {
        std::vector<int> alloc(n, 0), maxV(n, 0);
        alloc[i]          = 1; // holds left fork
        maxV[i]           = 1;
        maxV[(i+1)%n]     = 1; // needs right fork
        rm.setAllocation(i, alloc);
        rm.setMaximum(i, maxV);
    }
    std::cout << "Loaded: Dining Philosophers (DEADLOCK expected)\n";
}

void Simulation::menuRunBankersCheck() {
    if (!banker) { std::cout << "No system setup. Please setup first.\n"; return; }
    banker->displaySafetyCheck();
}

void Simulation::menuRequestResource() {
    if (!banker) { std::cout << "No system setup. Please setup first.\n"; return; }
    int pid; std::cout << "Process ID (0-" << rm.numProcesses()-1 << "): "; std::cin >> pid;
    int m = rm.numResources();
    std::vector<int> req(m);
    std::cout << "Request vector [" << m << " values]: ";
    for (int j = 0; j < m; ++j) std::cin >> req[j];
    auto result = banker->requestResources(pid, req);
    std::cout << "\n" << result.message << "\n";
    rm.displayState();
}

void Simulation::menuDetectDeadlock() {
    if (!detector) { std::cout << "No system setup. Please setup first.\n"; return; }
    detector->displayWaitForGraph();
    auto result = detector->detect();
    detector->displayReport(result);
}

void Simulation::menuRecover() {
    if (!recovery) { std::cout << "No system setup. Please setup first.\n"; return; }
    auto detection = detector->detect();
    if (!detection.deadlockFound) {
        std::cout << "No deadlock detected — nothing to recover from.\n";
        return;
    }
    std::cout << "\nRecovery Strategy:\n";
    std::cout << " 1. Terminate ALL deadlocked processes\n";
    std::cout << " 2. Terminate ONE at a time\n";
    std::cout << " 3. Preempt resources from victim\n";
    int s; std::cin >> s;
    RecoveryStrategy strategy;
    if      (s == 1) strategy = RecoveryStrategy::TERMINATE_ALL;
    else if (s == 2) strategy = RecoveryStrategy::TERMINATE_ONE;
    else             strategy = RecoveryStrategy::PREEMPT_RESOURCES;
    auto result = recovery->recover(detection, strategy);
    recovery->displayReport(result);
    rm.displayState();
}

void Simulation::menuDisplayState() {
    if (rm.numProcesses() == 0) { std::cout << "No system setup.\n"; return; }
    rm.displayState();
    rm.displayTable();
    if (detector) detector->displayWaitForGraph();
}

void Simulation::menuReset() {
    rm.reset();
    std::cout << "System reset to initial state.\n";
    rm.displayState();
}
