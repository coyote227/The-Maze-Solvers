import csv
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


def load_benchmark(path="benchmark.csv"):
    try:
        with open(path, newline="") as f:
            return list(csv.DictReader(f))
    except Exception as e:
        return [{"Error": f"benchmark.csv not found or unreadable: {e}"}]


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
                fill = "#202020"
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
    benchmark = load_benchmark("benchmark.csv")
    fastest_name, fastest_path = load_fastest_path("fastest.txt")

    root = tk.Tk()
    root.title("Maze Solver Results")
    root.geometry("1200x700")

    # Left: text results
    left = ttk.Frame(root)
    left.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=8, pady=8)
    tabs = ttk.Notebook(left)
    tabs.pack(fill=tk.BOTH, expand=True)

    results_tab = ttk.Frame(tabs)
    tabs.add(results_tab, text="Current Maze")
    txt = tk.Text(results_tab, wrap=tk.WORD, height=30, width=60)
    txt.pack(fill=tk.BOTH, expand=True)
    txt.insert("1.0", res_text)
    txt.configure(state=tk.DISABLED)

    benchmark_tab = ttk.Frame(tabs)
    tabs.add(benchmark_tab, text="Benchmarks")
    columns = (
        "algorithm", "size", "runs", "average", "minimum", "maximum",
        "memory", "path"
    )
    table = ttk.Treeview(benchmark_tab, columns=columns, show="headings")
    headings = {
        "algorithm": "Algorithm", "size": "Maze Size", "runs": "Runs",
        "average": "Average ms", "minimum": "Minimum ms",
        "maximum": "Maximum ms", "memory": "Peak KB", "path": "Path moves"
    }
    for column in columns:
        table.heading(column, text=headings[column])
        table.column(column, width=90, anchor=tk.CENTER)
    for row in benchmark:
        if "Error" in row:
            table.insert("", tk.END, values=(row["Error"],))
            continue
        table.insert("", tk.END, values=(
            row.get("algorithm", ""),
            f"{row.get('height', '')}x{row.get('width', '')}",
            row.get("runs", ""),
            row.get("average_ms", ""),
            row.get("minimum_ms", ""),
            row.get("maximum_ms", ""),
            row.get("peak_working_set_kb", ""),
            row.get("path_moves", "")
        ))
    table.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
    scrollbar = ttk.Scrollbar(benchmark_tab, orient=tk.VERTICAL, command=table.yview)
    scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
    table.configure(yscrollcommand=scrollbar.set)

    right = ttk.Frame(root)
    right.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=8, pady=8)
    title = f"Fastest: {fastest_name}" if fastest_name else "Fastest: (none)"
    ttk.Label(right, text=title, font=("Segoe UI", 12, "bold")).pack(anchor=tk.W)
    MazeCanvas(right, maze, fastest_path, cell_size=24).pack(fill=tk.BOTH, expand=True)

    root.mainloop()


if __name__ == "__main__":
    main()


