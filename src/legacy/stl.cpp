#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>

using MazeRow = std::vector<char>;
using Maze = std::vector<MazeRow>;

int main()
{
    std::ifstream infile("maze.txt");
    if (!infile) {
        std::cerr << "Cannot open maze.txt\n";
        return 1;
    }

    Maze maze;
    std::string line;
    while (std::getline(infile, line)) {
        MazeRow row;
        std::copy(line.begin(), line.end(), std::back_inserter(row));
        maze.push_back(row);
    }
    infile.close();

    std::cout << "Maze size: " << maze.size() << " rows\n";

    // Iterate through the maze using STL iterators
    for (auto rowIt = maze.begin(); rowIt != maze.end(); ++rowIt) {
        // rowIt is iterator to a MazeRow
        for (auto colIt = rowIt->begin(); colIt != rowIt->end(); ++colIt) {
            char c = *colIt;
            // For demonstration: print the char
            std::cout << c;
        }
        std::cout << "\n";
    }

    // Example: find all openings (say char ' ' spaces) and list their positions
    for (size_t r = 0; r < maze.size(); ++r) {
        for (size_t c = 0; c < maze[r].size(); ++c) {
            if (maze[r][c] == ' ') {
                std::cout << "Open space at (" << r << ", " << c << ")\n";
            }
        }
    }

    return 0;
}