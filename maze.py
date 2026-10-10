import random
import turtle

def generate_maze(width, height):
    # Initialize the maze grid with walls
    maze = [[1 for _ in range(width)] for _ in range(height)]

    # Define directions (dx, dy) for moving: (right, down, left, up)
    directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]

    # Start DFS from a random cell
    start_x, start_y = random.randrange(1, height - 1, 2), random.randrange(1, width - 1, 2)
    stack = [(start_x, start_y)]
    maze[start_x][start_y] = 0  # Mark as path

    while stack:
        current_x, current_y = stack[-1]
        
        # Find unvisited neighbors
        unvisited_neighbors = []
        for dx, dy in directions:
            nx, ny = current_x + 2 * dx, current_y + 2 * dy
            if 0 < nx < height - 1 and 0 < ny < width - 1 and maze[nx][ny] == 1:
                unvisited_neighbors.append((nx, ny, dx, dy))

        if unvisited_neighbors:
            # Choose a random unvisited neighbor
            next_x, next_y, dx, dy = random.choice(unvisited_neighbors)
            
            # Carve a path between current cell and neighbor
            maze[current_x + dx][current_y + dy] = 0
            maze[next_x][next_y] = 0
            stack.append((next_x, next_y))
        else:
            stack.pop()  # Backtrack

    return maze

def print_maze(maze):
    for row in maze:
        print("".join(["#" if cell == 1 else " " for cell in row]))

def find_start_end_points(maze):
    """Find start and end points in the maze."""
    height = len(maze)
    width = len(maze[0]) if height > 0 else 0
    
    # Find start point (first path cell from top-left)
    start_row, start_col = None, None
    for r in range(height):
        for c in range(width):
            if maze[r][c] == 0:
                start_row, start_col = r, c
                break
        if start_row is not None:
            break
    
    # Find end point (last path cell from bottom-right)
    end_row, end_col = None, None
    for r in range(height - 1, -1, -1):
        for c in range(width - 1, -1, -1):
            if maze[r][c] == 0:
                end_row, end_col = r, c
                break
        if end_row is not None:
            break
    
    return (start_row, start_col), (end_row, end_col)

def save_maze_to_file(maze, filename="maze.txt"):
    """
    Save maze to a text file in a format suitable for C++ parsing.
    Format:
    - First line: height width
    - Second line: start_row start_col
    - Third line: end_row end_col
    - Each subsequent line: row of the maze (1 = wall, 0 = path, S = start, E = end)
    """
    height = len(maze)
    width = len(maze[0]) if height > 0 else 0
    
    (start_row, start_col), (end_row, end_col) = find_start_end_points(maze)
    
    with open(filename, 'w') as f:
        # Write dimensions
        f.write(f"{height} {width}\n")
        
        # Write start and end positions
        if start_row is not None and end_row is not None:
            f.write(f"{start_row} {start_col}\n")
            f.write(f"{end_row} {end_col}\n")
        
        # Write maze grid (1 = wall, 0 = path, S = start, E = end)
        for r in range(height):
            row_str = ""
            for c in range(width):
                if r == start_row and c == start_col:
                    row_str += "S"
                elif r == end_row and c == end_col:
                    row_str += "E"
                elif maze[r][c] == 1:
                    row_str += "1"
                else:
                    row_str += "0"
            f.write(row_str + "\n")
    
    print(f"Maze saved to {filename}")

def draw_maze_turtle(maze, cell_size=20, start_pos=None, end_pos=None):
    height = len(maze)
    width = len(maze[0]) if height > 0 else 0

    screen = turtle.Screen()
    screen.title("Maze")
    screen.setup(width=width * cell_size + 40, height=height * cell_size + 40)
    screen.tracer(0, 0)

    pen = turtle.Turtle()
    pen.hideturtle()
    pen.speed(0)
    pen.penup()

    # Top-left origin
    start_x = - (width * cell_size) / 2
    start_y = (height * cell_size) / 2

    def draw_filled_square(x, y, size):
        pen.goto(x, y)
        pen.setheading(0)
        pen.pendown()
        pen.begin_fill()
        for _ in range(4):
            pen.forward(size)
            pen.right(90)
        pen.end_fill()
        pen.penup()

    # Draw walls
    pen.color("black", "black")
    for r in range(height):
        for c in range(width):
            if maze[r][c] == 1:
                x = start_x + c * cell_size
                y = start_y - r * cell_size
                draw_filled_square(x, y, cell_size)

    # Draw start point (green)
    if start_pos is not None:
        start_row, start_col = start_pos
        if start_row is not None and start_col is not None:
            x = start_x + start_col * cell_size
            y = start_y - start_row * cell_size
            pen.color("green", "green")
            draw_filled_square(x, y, cell_size)

    # Draw end point (red)
    if end_pos is not None:
        end_row, end_col = end_pos
        if end_row is not None and end_col is not None:
            x = start_x + end_col * cell_size
            y = start_y - end_row * cell_size
            pen.color("red", "red")
            draw_filled_square(x, y, cell_size)

    screen.update()
    turtle.done()

if __name__ == "__main__":
    maze_width = 21  # Must be odd for this algorithm
    maze_height = 11 # Must be odd for this algorithm
    generated_maze = generate_maze(maze_width, maze_height)
    
    # Find start and end points
    start_pos, end_pos = find_start_end_points(generated_maze)
    
    # Save maze to file for C++ program
    save_maze_to_file(generated_maze, "maze.txt")
    
    # print_maze(generated_maze)  # Uncomment to also print in console
    draw_maze_turtle(generated_maze, cell_size=24, start_pos=start_pos, end_pos=end_pos)