# Maze Solver Comparison

A maze-generation and pathfinding comparison project built with Python, C++, and Tkinter. It compares Dijkstra, BFS, DFS, and a linear reachability scan using runtime, memory, and path results.

## Project structure

| Folder | Purpose |
| --- | --- |
| `src/` | Main C++ solver. |
| `src/legacy/` | Earlier standalone C++ demonstrations. |
| `scripts/` | Maze generator and results GUI. |
| `data/` | Input maze files. |
| `output/` | Generated reports and solution files. |
| `build/` | Compiled executables. |

## Build and run

From the project root:

```powershell
g++ -std=c++17 -O2 src/main.cpp -o build/main.exe
.\build\main.exe
python scripts/report_gui.py
```

To generate a new maze first:

```powershell
python scripts/maze.py
```

The generator uses Turtle and requires a desktop Python environment.

## Generated files

- `output/results.txt` — results for the current maze.
- `output/fastest.txt` — fastest solver and path coordinates.
- `output/benchmark.csv` — five-run benchmarks across `11x21`, `21x41`, `31x61`, and `41x81` mazes.

The GUI displays the current-maze report, benchmark table, and fastest path visualization.

## Benchmark details

Each algorithm is run five times for every maze size. The CSV contains average, minimum, and maximum runtime, peak process working-set memory, and path length. Memory is measured at the operating-system process level, so it represents overall process footprint rather than isolated allocations for one solver.

## Maze format

`data/maze.txt` contains the height and width, start coordinates, end coordinates, and maze rows. The grid uses `1` for walls and `0` for open cells.
