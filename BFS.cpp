#include <bits/stdc++.h>
using namespace std;

struct Point {
    int r, c;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Start timer
    clock_t start_time = clock();

    ifstream fin("maze.txt");
    if (!fin) {
        cerr << "Error: could not open file 'maze.txt'\n";
        return 1;
    }

    int rows, cols;
    fin >> rows >> cols;
    int sr, sc, gr, gc;
    fin >> sr >> sc >> gr >> gc;
    fin.ignore();

    vector<string> maze(rows);
    for (int i = 0; i < rows; i++)
        getline(fin, maze[i]);
    fin.close();

    vector<vector<int>> dist(rows, vector<int>(cols, -1));
    vector<vector<Point>> parent(rows, vector<Point>(cols, {-1, -1}));
    queue<Point> q;
    vector<Point> visited_order; // store all visited cells

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    q.push({sr, sc});
    dist[sr][sc] = 0;

    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        visited_order.push_back({r, c});

        if (r == gr && c == gc)
            break;

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                maze[nr][nc] != '1' && dist[nr][nc] == -1) {
                dist[nr][nc] = dist[r][c] + 1;
                parent[nr][nc] = {r, c};
                q.push({nr, nc});
            }
        }
    }

    ofstream fout("solution.txt");
    if (!fout) {
        cerr << "Error: could not create 'solution.txt'\n";
        return 1;
    }

    if (dist[gr][gc] == -1) {
        fout << "No path found.\n";
        fout.close();
        cout << "No path found.\n";
        return 0;
    }

    // Reconstruct path
    vector<Point> path;
    for (Point p = {gr, gc}; p.r != -1; p = parent[p.r][p.c])
        path.push_back(p);
    reverse(path.begin(), path.end());

    // Mark path in maze
    for (auto &p : path)
        if (maze[p.r][p.c] == '0')
            maze[p.r][p.c] = '.';

    // Compute execution time and memory used
    clock_t end_time = clock();
    double exec_time = double(end_time - start_time) / CLOCKS_PER_SEC;

    // Rough memory usage estimate
    size_t memory_used = sizeof(maze)
        + rows * cols * (sizeof(int) * 2 + sizeof(Point))
        + sizeof(dist) + sizeof(parent) + sizeof(q) + sizeof(path)
        + sizeof(visited_order);

    // Write to file
    fout << "Path found! Length = " << dist[gr][gc] << "\n\n";
    fout << "Traversed cells: " << visited_order.size() << "\n";
    fout << "Path cells: " << path.size() << "\n\n";

    fout << "Path (row, col):\n";
    for (auto &p : path)
        fout << "(" << p.r << ", " << p.c << ")\n";

    fout << "\nAll traversed cells:\n";
    for (auto &p : visited_order)
        fout << "(" << p.r << ", " << p.c << ")\n";

    fout << "\nMaze with path ('.' marks path):\n";
    for (auto &row : maze)
        fout << row << '\n';

    fout << "\nExecution Time: " << fixed << setprecision(5) << exec_time << " seconds\n";
    fout << "Approx Memory Used: " << memory_used / 1024.0 << " KB\n";

    fout.close();

    cout << "Solution saved to 'bfs_solution.txt'\n";
    cout << "Execution Time: " << exec_time << " seconds\n";
    cout << "Approx Memory Used: " << memory_used / 1024.0 << " KB\n";

    return 0;
}
