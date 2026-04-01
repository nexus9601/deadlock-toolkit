#pragma once
#include "resource_manager.h"
#include "banker.h"
#include "deadlock_detector.h"
#include "deadlock_recovery.h"
#include <string>

// Simulation engine — ties everything together
class Simulation {
public:
    Simulation() = default;

    // Run the main interactive menu
    void run();

private:
    ResourceManager rm;
    BankersAlgorithm* banker = nullptr;
    DeadlockDetector* detector = nullptr;
    DeadlockRecovery* recovery = nullptr;

    // Menu handlers
    void menuSetup();
    void menuLoadScenario();
    void menuRunBankersCheck();
    void menuRequestResource();
    void menuDetectDeadlock();
    void menuRecover();
    void menuDisplayState();
    void menuReset();

    // Preloaded scenarios
    void loadBankersClassicExample();
    void loadDeadlockScenario();
    void loadDiningPhilosophers();

    // Utility
    void printHeader() const;
    void printMenu() const;
    void initAlgorithms();
    void cleanup();
};
