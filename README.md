# Deadlock Detection, Prevention, and Recovery Toolkit

## 1. Overview

This project is a **Deadlock Detection, Prevention, and Recovery Toolkit** built for the Operating Systems course.  
It simulates resource allocation among multiple processes, detects potential deadlocks, prevents unsafe allocations using **Banker's Algorithm**, and demonstrates basic **deadlock recovery** strategies.  
An additional **interactive web interface** visualizes the system state and resource allocation graph in real time.

## 2. Features

- **Deadlock detection**
  - Resource Allocation Graph (RAG) construction.
  - Cycle detection to identify deadlocked processes.
  - Banker's safety check to determine if the system is in a safe or unsafe state.

- **Deadlock prevention**
  - Implementation of **Banker's Algorithm** to decide whether a resource request can be safely granted.
  - System only moves from one safe state to another.

- **Deadlock recovery**
  - Simple victim selection strategy (e.g., process with maximum allocation).
  - Resource preemption and process termination/rollback.
  - System state updated and shown after recovery.

- **Interactive simulation**
  - User-defined number of processes and resource types.
  - Custom `Max`, `Allocation`, and `Available` matrices.
  - Step-by-step simulation of resource requests and system evolution.

- **Web-based visualization**
  - HTML/CSS/JavaScript frontend to:
    - Configure scenarios.
    - Run Banker's Algorithm.
    - Display matrices and safe sequence.
    - Visualize the resource allocation graph (RAG).
    - Show detection and recovery steps clearly in the browser.

## 3. System Architecture

The project has two main components:

1. **C++ Core (CLI Toolkit)**
   - Implements the core algorithms and data structures.
   - Menu-driven interface for simulations.

2. **Web Frontend**
   - Runs in the browser using JavaScript implementations of the same logic.
   - Provides an easy-to-understand visual representation for demonstrations.

### Directory Structure

```text
.
├── README.md
├── src/
│   ├── main.cpp
│   ├── banker.cpp
│   ├── banker.h
│   ├── rag.cpp
│   ├── rag.h
│   ├── recovery.cpp
│   ├── recovery.h
│   └── simulation.cpp
│       simulation.h
├── web/
│   ├── index.html
│   ├── style.css
│   └── app.js
├── docs/
│   ├── design.md
│   └── test-cases.md
└── examples/
    ├── scenario1.txt
    └── scenario2.txt
