# 🔒 Deadlock Toolkit — Real-Time Detection, Prevention & Recovery

A comprehensive C++ toolkit that detects, prevents, and recovers from deadlocks in real-time. Implements Banker's Algorithm, cycle-based deadlock detection via Wait-For Graphs, and multiple recovery strategies — all visualized through an interactive web interface.

---

## 📁 Project Structure

```
deadlock-toolkit/
├── src/
│   ├── process.h              # Process data structure
│   ├── resource.h             # Resource data structure
│   ├── resource_manager.h/.cpp  # Resource allocation management
│   ├── banker.h/.cpp          # Banker's Algorithm (prevention)
│   ├── deadlock_detector.h/.cpp # Deadlock detection (cycle detection)
│   ├── deadlock_recovery.h/.cpp # Recovery strategies
│   ├── simulation.h/.cpp      # Simulation engine
│   └── main.cpp               # CLI entry point
├── web/
│   └── index.html             # Interactive visualization website
├── docs/
│   └── project_overview.md    # Full project documentation
├── Makefile
└── README.md
```

---

## 🚀 Features

| Feature | Description |
|---|---|
| **Banker's Algorithm** | Prevents deadlocks by checking system safety before granting resources |
| **Deadlock Detection** | Cycle detection in Wait-For Graph using DFS |
| **Recovery** | Process termination and resource preemption strategies |
| **Simulation** | Custom scenario input with predefined test cases |
| **Web Visualization** | Interactive resource allocation graph with real-time updates |

---

## 🛠️ Build and Run

### Prerequisites
- g++ with C++17 support
- A modern browser (for the web UI)

### Compile
```bash
make
```

### Run CLI
```bash
./deadlock_toolkit
```

### Web Interface
Open `web/index.html` in any browser — no server needed.

---

## 🔑 Algorithms

### Banker's Algorithm (Prevention)
1. Maintains Allocation, Maximum, Need, and Available matrices
2. Before granting a resource request, runs the Safety Algorithm
3. Grants the request only if the resulting state is safe

### Deadlock Detection (Wait-For Graph)
1. Builds a Wait-For Graph from current allocations
2. Runs DFS-based cycle detection
3. Reports all processes involved in deadlock

### Recovery Strategies
- Process Termination: Kills cheapest process to break cycle
- Resource Preemption: Preempts resources from victim processes

---

## Author
OS Project — Deadlock Toolkit
Language: C++17 | Web: HTML5 / Vanilla JS / SVG
