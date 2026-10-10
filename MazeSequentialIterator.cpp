#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN() system("cls")
    
    // Enable ANSI color codes on Windows 10+
    void enableWindowsColors() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
    }
#else
    #define CLEAR_SCREEN() system("clear")
    void enableWindowsColors() {} // No-op on non-Windows
#endif

using namespace std;

// Structure to represent a cell in the maze
struct Cell {
    int row, col;
    Cell(int r = 0, int c = 0) : row(r), col(c) {}
    bool operator==(const Cell& other) const {
        return row == other.row && col == other.col;
    }
};

// Directions: 0=North, 1=East, 2=South, 3=West
// For right-hand rule: turn right, go straight, turn left, turn back
// For left-hand rule: turn left, go straight, turn right, turn back

// Helper function to sleep for milliseconds (cross-platform)
void sleep_ms(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

// Visualize the maze with current position and path
void visualizeMaze(vector<string>& maze, int currentRow, int currentCol, 
                   vector<Cell>& path, int startRow, int startCol, 
                   int endRow, int endCol, int stepCount, bool useRightHand) {
    int height = maze.size();
    int width = maze[0].size();
    
    // Create a copy of the maze for visualization
    vector<string> display = maze;
    
    // Mark visited path cells (but not the current position)
    for (const Cell& cell : path) {
        if (cell.row == startRow && cell.col == startCol) {
            display[cell.row][cell.col] = 'S';
        } else if (cell.row == endRow && cell.col == endCol) {
            display[cell.row][cell.col] = 'E';
        } else if (!(cell.row == currentRow && cell.col == currentCol)) {
            display[cell.row][cell.col] = '*';
        }
    }
    
    // Mark current position with a special character
    if (currentRow >= 0 && currentRow < height && currentCol >= 0 && currentCol < width) {
        if (currentRow == startRow && currentCol == startCol) {
            display[currentRow][currentCol] = 'S';
        } else if (currentRow == endRow && currentCol == endCol) {
            display[currentRow][currentCol] = 'E';
        } else {
            display[currentRow][currentCol] = '@';  // Current position
        }
    }
    
    // Clear screen and print
    CLEAR_SCREEN();
    
    string ruleName = useRightHand ? "Right-Hand Rule" : "Left-Hand Rule";
    cout << "\n=== Maze Solver - " << ruleName << " ===" << endl;
    cout << "Step: " << stepCount << " | Path Length: " << path.size() << " | Current: (" 
         << currentRow << ", " << currentCol << ")" << endl;
    cout << "Legend: S=Start, E=End, @=Current, *=Path, 1=Wall, 0=Empty\n" << endl;
    
    // Print the maze
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char ch = display[i][j];
            // Add some color coding (basic, works on most terminals)
            if (ch == '@') {
                cout << "\033[1;33m@\033[0m";  // Yellow for current
            } else if (ch == 'S') {
                cout << "\033[1;32mS\033[0m";  // Green for start
            } else if (ch == 'E') {
                cout << "\033[1;31mE\033[0m";  // Red for end
            } else if (ch == '*') {
                cout << "\033[1;36m*\033[0m";  // Cyan for path
            } else if (ch == '1') {
                cout << "\033[1;37m1\033[0m";  // White for walls
            } else {
                cout << ch;
            }
        }
        cout << endl;
    }
    cout << endl;
}

// Wall following algorithm with visualization (right-hand or left-hand rule)
vector<Cell> solveMazeWallFollowing(vector<string>& maze, int startRow, int startCol, 
                                     int endRow, int endCol, bool useRightHand = true, 
                                     bool visualize = true, int delayMs = 100) {
    int height = maze.size();
    int width = maze[0].size();
    
    // Direction vectors: North, East, South, West
    int dr[] = {-1, 0, 1, 0};
    int dc[] = {0, 1, 0, -1};
    
    vector<Cell> path;
    int currentRow = startRow;
    int currentCol = startCol;
    
    // Determine initial direction - try to find a valid direction from start
    int direction = 1; // Start facing East (right)
    for (int d = 0; d < 4; d++) {
        int newRow = currentRow + dr[d];
        int newCol = currentCol + dc[d];
        if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && 
            maze[newRow][newCol] != '1') {
            direction = d;
            break;
        }
    }
    
    int maxSteps = height * width * 4; // Safety limit to prevent infinite loops
    int stepCount = 0;
    
    path.push_back(Cell(currentRow, currentCol));
    
    // Initial visualization
    if (visualize) {
        visualizeMaze(maze, currentRow, currentCol, path, startRow, startCol, endRow, endCol, stepCount, useRightHand);
        sleep_ms(delayMs);
    }
    
    while (stepCount < maxSteps) {
        // Check if we reached the end
        if (currentRow == endRow && currentCol == endCol) {
            if (visualize) {
                visualizeMaze(maze, currentRow, currentCol, path, startRow, startCol, endRow, endCol, stepCount, useRightHand);
                cout << "SUCCESS! Path found in " << stepCount << " steps!" << endl;
                sleep_ms(2000); // Pause to show final result
            }
            return path;
        }
        
        // Determine priority order based on hand rule
        // For right-hand: try right (turn right), forward, left, back
        // For left-hand: try left (turn left), forward, right, back
        int priorities[4];
        if (useRightHand) {
            // Right-hand rule: right, forward, left, back
            priorities[0] = (direction + 1) % 4; // Turn right
            priorities[1] = direction;            // Go forward
            priorities[2] = (direction + 3) % 4; // Turn left
            priorities[3] = (direction + 2) % 4; // Turn back
        } else {
            // Left-hand rule: left, forward, right, back
            priorities[0] = (direction + 3) % 4; // Turn left
            priorities[1] = direction;            // Go forward
            priorities[2] = (direction + 1) % 4; // Turn right
            priorities[3] = (direction + 2) % 4; // Turn back
        }
        
        bool moved = false;
        // Try each direction in priority order
        for (int i = 0; i < 4; i++) {
            int tryDir = priorities[i];
            int newRow = currentRow + dr[tryDir];
            int newCol = currentCol + dc[tryDir];
            
            // Check if the cell is valid (not a wall and within bounds)
            if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && 
                maze[newRow][newCol] != '1') {
                
                // Move to the new cell
                currentRow = newRow;
                currentCol = newCol;
                direction = tryDir;
                
                // Add to path (avoid consecutive duplicates)
                if (path.empty() || !(path.back() == Cell(currentRow, currentCol))) {
                    path.push_back(Cell(currentRow, currentCol));
                }
                
                moved = true;
                stepCount++;
                
                // Visualize after each move
                if (visualize) {
                    visualizeMaze(maze, currentRow, currentCol, path, startRow, startCol, endRow, endCol, stepCount, useRightHand);
                    sleep_ms(delayMs);
                }
                
                break;
            }
        }
        
        // If we couldn't move, we're stuck (shouldn't happen in a valid maze)
        if (!moved) {
            break;
        }
    }
    
    // If we didn't reach the end, return empty path
    return vector<Cell>();
}

void printMazeWithPath(vector<string>& maze, vector<Cell>& path, int startRow, int startCol, int endRow, int endCol) {
    int height = maze.size();
    int width = maze[0].size();
    
    // Create a copy of the maze for visualization
    vector<string> display = maze;
    
    // Mark path cells
    for (const Cell& cell : path) {
        if (cell.row == startRow && cell.col == startCol) {
            display[cell.row][cell.col] = 'S';
        } else if (cell.row == endRow && cell.col == endCol) {
            display[cell.row][cell.col] = 'E';
        } else {
            display[cell.row][cell.col] = '*';
        }
    }
    
    // Print the maze
    cout << "\nMaze with path (S=Start, E=End, *=Path, 1=Wall, 0=Empty):\n";
    for (int i = 0; i < height; i++) {
        cout << display[i] << endl;
    }
}

int main() {
    // Enable ANSI colors on Windows
    enableWindowsColors();
    
    ifstream file("maze.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open maze.txt" << endl;
        return 1;
    }
    
    int height, width;
    int startRow, startCol;
    int endRow, endCol;
    
    // Read dimensions
    file >> height >> width;
    // Read start position
    file >> startRow >> startCol;
    // Read end position
    file >> endRow >> endCol;
    
    // Read the maze
    vector<string> maze(height);
    string line;
    getline(file, line); // Consume the newline after end position
    
    for (int i = 0; i < height; i++) {
        getline(file, maze[i]);
    }
    file.close();
    
    cout << "Maze dimensions: " << height << " x " << width << endl;
    cout << "Start: (" << startRow << ", " << startCol << ")" << endl;
    cout << "End: (" << endRow << ", " << endCol << ")" << endl;
    cout << "\nPress Enter to start visualization..." << endl;
    cin.get();
    
    // Solve using right-hand rule wall following with visualization
    vector<Cell> path = solveMazeWallFollowing(maze, startRow, startCol, endRow, endCol, true, true, 80);
    
    if (!path.empty()) {
        CLEAR_SCREEN();
        cout << "\n=== Right-Hand Rule Results ===" << endl;
        cout << "Path found! Length: " << path.size() << " cells" << endl;
        printMazeWithPath(maze, path, startRow, startCol, endRow, endCol);
        cout << "\nPress Enter to try Left-Hand Rule..." << endl;
        cin.get();
    } else {
        cout << "No path found with Right-Hand Rule!" << endl;
    }
    
    // Also try left-hand rule for comparison
    vector<Cell> path2 = solveMazeWallFollowing(maze, startRow, startCol, endRow, endCol, false, true, 80);
    
    if (!path2.empty()) {
        CLEAR_SCREEN();
        cout << "\n=== Left-Hand Rule Results ===" << endl;
        cout << "Path found! Length: " << path2.size() << " cells" << endl;
        printMazeWithPath(maze, path2, startRow, startCol, endRow, endCol);
    } else {
        cout << "No path found with Left-Hand Rule!" << endl;
    }
    
    cout << "\nPress Enter to exit..." << endl;
    cin.get();
    
    return 0;
}