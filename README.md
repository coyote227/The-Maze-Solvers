# Maze Solver Comparison

A maze-generation and pathfinding comparison project built with Python, C++, and Tkinter. It compares Dijkstra, BFS, DFS, and a linear reachability scan using the same maze data and records runtime, memory, and path results.

## Features

- Random maze generation with depth-first search.
- Turtle visualization for generated mazes.
- Four C++ pathfinding algorithms.
- Single-maze result output for the GUI.
- Repeatable multi-size performance benchmark.
- Average, minimum, and maximum runtime measurements.
- Peak process working-set memory measurement.
- CSV output suitable for spreadsheets and charts.
- Benchmark table in the Tkinter report window.

## Build and run

From the project directory:

```powershell
g++ -std=c++17 -O2 main.cpp -o main.exe
.\main.exe
python report_gui.py
```

Running `main.exe` creates:

- `results.txt` — detailed results for the current `maze.txt`.
- `fastest.txt` — the fastest solver and its path coordinates.
- `benchmark.csv` — performance results across multiple maze sizes.

Run `python maze.py` first if you want to generate a new maze. The generator uses Turtle and requires a desktop Python environment.

## Benchmark design

The benchmark uses deterministic generated mazes with these dimensions:

```text
11 x 21
21 x 41
31 x 61
41 x 81
```

Each algorithm is run five times for every maze size. `benchmark.csv` contains:

- Algorithm name
- Maze height and width
- Number of runs
- Average runtime in milliseconds
- Minimum runtime in milliseconds
- Maximum runtime in milliseconds
- Peak process working-set memory in KB
- Path length in moves

The memory value is measured at the process level using Windows peak working-set counters, or the process maximum resident set size on Unix-like systems. It is an operating-system measurement and is reported for the benchmark process, so values can vary slightly between machines and should be treated as a footprint comparison rather than an isolated allocation count for one solver.

## Input format

`maze.txt` contains the height and width, start coordinates, end coordinates, and maze rows. The grid uses `1` for walls and `0` for open cells.

## Project files

| File | Purpose |
| --- | --- |
| `maze.py` | Generates and displays a maze. |
| `maze.txt` | Input maze for the C++ program. |
| `main.cpp` | Runs the solvers and benchmark suite. |
| `report_gui.py` | Displays the current results and fastest path. |
| `benchmark.csv` | Generated multi-size benchmark data. |
| `MazeSequentialIterator.cpp` | Optional animated wall-following demonstration. |
| `BFS.cpp`, `custom.cpp`, `stl.cpp` | Earlier standalone demonstrations. |

## Interpreting the results

BFS and Dijkstra should produce shortest paths because every maze move has equal cost. DFS can find a valid path without guaranteeing the shortest route. The linear scan repeatedly propagates reachability and is included as a simple baseline; its runtime grows more quickly as the maze becomes larger.
