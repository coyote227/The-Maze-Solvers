#include <algorithm>
#include <chrono>
#include <functional>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct Cell {
    int row;
    int col;
};

bool operator==(const Cell& a, const Cell& b) {
    return a.row == b.row && a.col == b.col;
}

struct Maze {
    int height{};
    int width{};
    Cell start{};
    Cell end{};
    vector<string> grid;
};

struct Result {
    string name;
    vector<Cell> path;
    double timeMs{};
};

bool readMaze(const string& filename, Maze& maze) {
    ifstream input(filename);
    if (!(input >> maze.height >> maze.width
                >> maze.start.row >> maze.start.col
                >> maze.end.row >> maze.end.col)) {
        return false;
    }

    string line;
    getline(input, line);
    maze.grid.resize(maze.height);
    for (string& row : maze.grid) {
        if (!getline(input, row) || static_cast<int>(row.size()) < maze.width) {
            return false;
        }
    }
    return true;
}

bool isOpen(const Maze& maze, int row, int col) {
    return row >= 0 && row < maze.height &&
           col >= 0 && col < maze.width &&
           maze.grid[row][col] != '1';
}

const int rowMove[4] = {-1, 0, 1, 0};
const int colMove[4] = {0, 1, 0, -1};

vector<Cell> makePath(const Maze& maze, const vector<vector<Cell>>& parent) {
    vector<Cell> path;
    Cell current = maze.end;

    while (!(current == maze.start)) {
        path.push_back(current);
        Cell previous = parent[current.row][current.col];
        if (previous.row == -1) return {};
        current = previous;
    }

    path.push_back(maze.start);
    reverse(path.begin(), path.end());
    return path;
}

vector<Cell> solveBfs(const Maze& maze) {
    vector<vector<bool>> visited(maze.height, vector<bool>(maze.width));
    vector<vector<Cell>> parent(maze.height, vector<Cell>(maze.width, {-1, -1}));
    queue<Cell> open;
    open.push(maze.start);
    visited[maze.start.row][maze.start.col] = true;

    while (!open.empty()) {
        Cell current = open.front();
        open.pop();
        if (current == maze.end) return makePath(maze, parent);

        for (int direction = 0; direction < 4; ++direction) {
            int row = current.row + rowMove[direction];
            int col = current.col + colMove[direction];
            if (isOpen(maze, row, col) && !visited[row][col]) {
                visited[row][col] = true;
                parent[row][col] = current;
                open.push({row, col});
            }
        }
    }
    return {};
}

vector<Cell> solveDijkstra(const Maze& maze) {
    using Node = pair<int, Cell>;
    auto compare = [](const Node& a, const Node& b) { return a.first > b.first; };

    vector<vector<int>> distance(
        maze.height, vector<int>(maze.width, numeric_limits<int>::max()));
    vector<vector<Cell>> parent(maze.height, vector<Cell>(maze.width, {-1, -1}));
    priority_queue<Node, vector<Node>, decltype(compare)> open(compare);

    distance[maze.start.row][maze.start.col] = 0;
    open.push({0, maze.start});

    while (!open.empty()) {
        auto [cost, current] = open.top();
        open.pop();
        if (cost != distance[current.row][current.col]) continue;
        if (current == maze.end) return makePath(maze, parent);

        for (int direction = 0; direction < 4; ++direction) {
            int row = current.row + rowMove[direction];
            int col = current.col + colMove[direction];
            if (!isOpen(maze, row, col)) continue;

            int newCost = cost + 1;
            if (newCost < distance[row][col]) {
                distance[row][col] = newCost;
                parent[row][col] = current;
                open.push({newCost, {row, col}});
            }
        }
    }
    return {};
}

vector<Cell> solveDfs(const Maze& maze) {
    vector<vector<bool>> visited(maze.height, vector<bool>(maze.width));
    vector<vector<Cell>> parent(maze.height, vector<Cell>(maze.width, {-1, -1}));
    stack<Cell> open;
    open.push(maze.start);
    visited[maze.start.row][maze.start.col] = true;

    while (!open.empty()) {
        Cell current = open.top();
        open.pop();
        if (current == maze.end) return makePath(maze, parent);

        for (int direction = 0; direction < 4; ++direction) {
            int row = current.row + rowMove[direction];
            int col = current.col + colMove[direction];
            if (isOpen(maze, row, col) && !visited[row][col]) {
                visited[row][col] = true;
                parent[row][col] = current;
                open.push({row, col});
            }
        }
    }
    return {};
}

vector<Cell> solveLinear(const Maze& maze) {
    vector<vector<bool>> reachable(maze.height, vector<bool>(maze.width));
    vector<vector<Cell>> parent(maze.height, vector<Cell>(maze.width, {-1, -1}));
    reachable[maze.start.row][maze.start.col] = true;

    bool changed = true;
    while (changed) {
        changed = false;
        for (int row = 0; row < maze.height; ++row) {
            for (int col = 0; col < maze.width; ++col) {
                if (!isOpen(maze, row, col) || reachable[row][col]) continue;
                for (int direction = 0; direction < 4; ++direction) {
                    int previousRow = row + rowMove[direction];
                    int previousCol = col + colMove[direction];
                    if (isOpen(maze, previousRow, previousCol) &&
                        reachable[previousRow][previousCol]) {
                        reachable[row][col] = true;
                        parent[row][col] = {previousRow, previousCol};
                        changed = true;
                        break;
                    }
                }
            }
        }
    }

    return reachable[maze.end.row][maze.end.col] ? makePath(maze, parent) : vector<Cell>{};
}

Result runSolver(const string& name, const Maze& maze,
                 const function<vector<Cell>(const Maze&)>& solver) {
    auto start = chrono::steady_clock::now();
    vector<Cell> path = solver(maze);
    auto finish = chrono::steady_clock::now();
    double timeMs = chrono::duration<double, milli>(finish - start).count();
    return {name, move(path), timeMs};
}

void printPathMaze(const Maze& maze, const vector<Cell>& path) {
    vector<string> display = maze.grid;
    for (const Cell& cell : path) {
        if (!(cell == maze.start) && !(cell == maze.end)) {
            display[cell.row][cell.col] = '*';
        }
    }
    for (const string& row : display) cout << row << '\n';
}

void saveResults(const Maze& maze, const vector<Result>& results, const Result& fastest) {
    ofstream output("results.txt");
    output << "Maze: " << maze.height << 'x' << maze.width
           << "  Start: (" << maze.start.row << ',' << maze.start.col << ')'
           << "  End: (" << maze.end.row << ',' << maze.end.col << ")\n\n";

    for (const Result& result : results) {
        output << "Algorithm: " << result.name << '\n'
               << "Time (ms): " << result.timeMs << '\n'
               << "Path length (moves): "
               << (result.path.empty() ? 0 : result.path.size() - 1) << '\n'
               << "Path cells: " << result.path.size() << "\n\n";
    }
    output << "FASTEST: " << fastest.name << " (" << fastest.timeMs << " ms)\n";
    output << "Fastest path length (moves): "
           << (fastest.path.empty() ? 0 : fastest.path.size() - 1) << '\n';

    ofstream fastestOutput("fastest.txt");
    fastestOutput << fastest.name << '\n';
    for (const Cell& cell : fastest.path) {
        fastestOutput << cell.row << ' ' << cell.col << '\n';
    }
}

int main() {
    Maze maze;
    if (!readMaze("maze.txt", maze)) {
        cerr << "Could not read maze.txt\n";
        return 1;
    }

    vector<Result> results = {
        runSolver("Dijkstra", maze, solveDijkstra),
        runSolver("BFS", maze, solveBfs),
        runSolver("DFS", maze, solveDfs),
        runSolver("Linear Scan", maze, solveLinear)
    };

    const Result* fastest = &results.front();
    for (const Result& result : results) {
        cout << '\n' << result.name << '\n'
             << "Time: " << result.timeMs << " ms\n"
             << "Path length: "
             << (result.path.empty() ? 0 : result.path.size() - 1) << " moves\n";
        printPathMaze(maze, result.path);
        if (result.timeMs < fastest->timeMs) fastest = &result;
    }

    cout << "\nFastest: " << fastest->name << " ("
         << fastest->timeMs << " ms)\n";
    saveResults(maze, results, *fastest);
    cout << "Results saved to results.txt and fastest.txt\n";
    return 0;
}
