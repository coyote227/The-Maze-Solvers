#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

struct Cell {
    int row, col;
    Cell(int r = 0, int c = 0) : row(r), col(c) {}
    bool operator==(const Cell& other) const {
        return row == other.row && col == other.col;
    }
};

vector<Cell> solveMazeDijkstra(vector<string>& maze, int startRow, int startCol,
                               int endRow, int endCol) {
    int height = maze.size();
    int width = (height > 0 ? maze[0].size() : 0);
    if (height == 0 || width == 0) return {};

    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};
    const int INF = 999999999;

    vector<vector<int>> dist(height, vector<int>(width, INF));
    vector<vector<Cell>> parent(height, vector<Cell>(width, Cell(-1, -1)));

    struct Node {
        int dist, row, col;
        bool operator>(const Node& other) const { return dist > other.dist; }
    };

    auto isFree = [&](int r, int c) {
        return !(r < 0 || r >= height || c < 0 || c >= width || maze[r][c] == '1');
    };

    priority_queue<Node, vector<Node>, greater<Node>> pq;
    dist[startRow][startCol] = 0;
    pq.push({0, startRow, startCol});

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();
        if (cur.dist != dist[cur.row][cur.col]) continue;
        if (cur.row == endRow && cur.col == endCol) break;

        for (int k = 0; k < 4; k++) {
            int nr = cur.row + dr[k];
            int nc = cur.col + dc[k];
            if (!isFree(nr, nc)) continue;

            int nd = cur.dist + 1;
            if (nd < dist[nr][nc]) {
                dist[nr][nc] = nd;
                parent[nr][nc] = {cur.row, cur.col};
                pq.push({nd, nr, nc});
            }
        }
    }

    if (dist[endRow][endCol] == INF) return {};

    vector<Cell> path;
    for (Cell at = {endRow, endCol}; at.row != -1; at = parent[at.row][at.col]) {
        path.push_back(at);
        if (at.row == startRow && at.col == startCol) break;
    }
    reverse(path.begin(), path.end());
    return path;
}

int main() {
    ifstream file("maze.txt");
    if (!file) return 1;

    int height, width, startRow, startCol, endRow, endCol;
    file >> height >> width;
    file >> startRow >> startCol;
    file >> endRow >> endCol;

    vector<string> maze(height);
    string line;
    getline(file, line);
    for (int i = 0; i < height; i++) getline(file, maze[i]);
    file.close();

    vector<Cell> path = solveMazeDijkstra(maze, startRow, startCol, endRow, endCol);

    // ✅ Append instead of overwrite
    ofstream out("results.txt", ios::app);
    if (!out) return 1;

    out << "\n=== Dijkstra Maze Result ===\n";

    if (path.empty()) {
        out << "No path found.\n";
    } else {
        out << "Path length: " << path.size() << "\n\n";
        for (auto &cell : path) {
            out << "(" << cell.row << ", " << cell.col << ")\n";
            if (maze[cell.row][cell.col] == '0')
                maze[cell.row][cell.col] = '*';
        }
        maze[startRow][startCol] = 'S';
        maze[endRow][endCol] = 'E';

        out << "\nMaze With Path:\n";
        for (auto &row : maze) out << row << "\n";
    }

    out.close();
    cout << "Done. Output appended to results.txt\n";
    return 0;
}
