import tkinter as tk
from tkinter import ttk, messagebox


def load_maze(path="maze.txt"):
    try:
        with open(path, "r") as f:
            h, w = map(int, f.readline().split())
            sr, sc = map(int, f.readline().split())
            er, ec = map(int, f.readline().split())
            grid = [f.readline().rstrip("\n") for _ in range(h)]
        return (h, w, sr, sc, er, ec, grid)
    except Exception as e:
        messagebox.showerror("Error", f"Failed to load maze: {e}")
        return None


def load_results(path="results.txt"):
    try:
        with open(path, "r") as f:
            return f.read()
    except Exception as e:
        return f"(results.txt not found or unreadable)\n{e}"


def load_fastest_path(path="fastest.txt"):
    try:
        with open(path, "r") as f:
            lines = [line.rstrip("\n") for line in f]
        if not lines:
            return ("", [])
        name = lines[0]
        coords = []
        for line in lines[1:]:
            parts = line.split()
            if len(parts) == 2:
                r, c = int(parts[0]), int(parts[1])
                coords.append((r, c))
        return (name, coords)
    except Exception as e:
        return ("", [])


class MazeCanvas(ttk.Frame):
    def __init__(self, master, maze_data, path_coords, cell_size=24, **kwargs):
        super().__init__(master, **kwargs)
        self.h, self.w, self.sr, self.sc, self.er, self.ec, self.grid = maze_data
        self.path = set(path_coords)
        self.cell = cell_size
        width = self.w * self.cell
        height = self.h * self.cell
        self.canvas = tk.Canvas(self, width=width, height=height, bg="#000000")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.draw()

    def draw(self):
        for r in range(self.h):
            for c in range(self.w):
                ch = self.grid[r][c]
                x0, y0 = c * self.cell, r * self.cell
                x1, y1 = x0 + self.cell, y0 + self.cell
                fill = "#202020"  # empty
                if ch == '1':
                    fill = "#444444"  # wall
                if (r, c) in self.path and ch not in ('S', 'E'):
                    fill = "#00bcd4"  # path cyan
                if r == self.sr and c == self.sc:
                    fill = "#2ecc71"  # start green
                if r == self.er and c == self.ec:
                    fill = "#e74c3c"  # end red
                self.canvas.create_rectangle(x0, y0, x1, y1, outline="#1c1c1c", fill=fill)


def main():
    maze = load_maze("maze.txt")
    if maze is None:
        return
    res_text = load_results("results.txt")
    fastest_name, fastest_path = load_fastest_path("fastest.txt")

    root = tk.Tk()
    root.title("Maze Solver Results")

    # Left: text results
    left = ttk.Frame(root)
    left.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=8, pady=8)
    ttk.Label(left, text="Results", font=("Segoe UI", 12, "bold")).pack(anchor=tk.W)
    txt = tk.Text(left, wrap=tk.WORD, height=30, width=60)
    txt.pack(fill=tk.BOTH, expand=True)
    txt.insert("1.0", res_text)
    txt.configure(state=tk.DISABLED)

    # Right: maze canvas with fastest path
    right = ttk.Frame(root)
    right.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=8, pady=8)
    title = f"Fastest: {fastest_name}" if fastest_name else "Fastest: (none)"
    ttk.Label(right, text=title, font=("Segoe UI", 12, "bold")).pack(anchor=tk.W)
    MazeCanvas(right, maze, fastest_path, cell_size=24).pack(fill=tk.BOTH, expand=True)

    root.mainloop()


if __name__ == "__main__":
    main()


