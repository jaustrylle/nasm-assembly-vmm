# -*- coding: utf-8 -*-
"""astar

Find path from initial node to goal node
"""

import heapq
import math
import sys
from itertools import count

"""Define class for object, allowing only positive infinity"""

class Node:
  def __init__(self):
    self.parent_row = 0     # parent node's row dex
    self.parent_col = 0     # parent node's col dex
    self.f = float('inf')   # total node cost
    self.g = float('inf')   # cost from start_node to this one
    self.h = 0              # this node's heuristic cost

"""Helper functions"""

# Manhattan Distance heuristic
def manhattan(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

# Printing function
def print_solution(grid, path):
    rows = len(grid)
    cols = len(grid[0])

    path_set = set(path)

    for i in range(rows):
        for j in range(cols):
            if (i, j) in path_set:
                print("P", end=" ")
            else:
                print(grid[i][j], end=" ")
        print()

    print("Cost:", len(path))

# Wrapper function
def astar(*args):
    if len(args) == 3:
        # grid, start, goal
        return astar_grid(args[0], args[1], args[2])
    elif len(args) == 5:
        # rows, cols, grid, start, goal
        return astar_input(args[0], args[1], args[2], args[3], args[4])

"""Overload the function; depending on input parameters given, select appropriate function

* grid = 2D list of int
* 0 is open path, 1 is obstacle
* grid size of N*M
"""

# LIST AND TUPLES GIVEN AS INPUT
  # default case (no line param; assigned grid, start, goal)
def default_case():
    grid = [
        [0, 0, 0, 0, 0],
        [0, 1, 1, 0, 0],
        [0, 0, 0, 1, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 1, 0]
    ]
    start = (0, 0)
    goal = (4, 4)

    path = astar(grid, start, goal)
    if path:
        print("Solution:")
        print_solution(grid, path)
    else:
        print("No path found.")

# INTEGERS GIVEN AS INPUT
  # new case (new line param, 2 int for grid size in rows & cols, grid of 0s & 1s, coords start & goal as 4 int)
def new_case():
    rows, cols = map(int, input().split())
    grid = []

    for _ in range(rows):
        line = input().strip()
        grid.append([int(x) for x in line])

    r1, c1, r2, c2 = map(int, input().split())
    start = (r1, c1)
    goal = (r2, c2)

    path = astar(rows, cols, grid, start, goal)
    if path:
        print("Solution:")
        print_solution(grid, path)
    else:
        print("No path found.")

"""A* Algorithm"""

# A* search -> f(n)=g(n)+h(n)

# n= node n
# f(n)= total estimated cost of cheapest solution
# g(n)= actual cost of path from start node to n
# h(n)= estimated cost n to goal

def astar_grid(grid, start, goal):
    rows = len(grid)
    cols = len(grid[0])

    counter = count()     # tie-breaking counter for deterministic order

    # nodes popped in order they were pushed
    open_list = []
    heapq.heappush(open_list, (0, next(counter), start))

    nodes = [[Node() for _ in range(cols)] for _ in range(rows)]

    sr, sc = start
    nodes[sr][sc].f = 0
    nodes[sr][sc].g = 0
    nodes[sr][sc].h = 0
    nodes[sr][sc].parent_row = sr
    nodes[sr][sc].parent_col = sc

    closed_set = set()

    while open_list:
        f, _, (r, c) = heapq.heappop(open_list)   # node f-score, no counter, coordinates
        closed_set.add((r, c))

        if (r, c) == goal:
            path = []
            while not (nodes[r][c].parent_row == r and nodes[r][c].parent_col == c):
                path.append((r, c))
                temp_r = nodes[r][c].parent_row
                temp_c = nodes[r][c].parent_col
                r, c = temp_r, temp_c
            path.append(start)
            path.reverse()
            return path

        # Move Down, Right, Up, Left
        directions = [(1,0),(0,1),(-1,0),(0,-1)]

        for dr, dc in directions:
            nr = r + dr
            nc = c + dc

            if 0 <= nr < rows and 0 <= nc < cols:
                if grid[nr][nc] == 0 and (nr, nc) not in closed_set:

                    g_new = nodes[r][c].g + 1
                    h_new = manhattan((nr, nc), goal)
                    f_new = g_new + h_new

                    if nodes[nr][nc].f == float('inf') or nodes[nr][nc].f > f_new:
                        heapq.heappush(open_list, (f_new, next(counter), (nr, nc)))

                        nodes[nr][nc].f = f_new
                        nodes[nr][nc].g = g_new
                        nodes[nr][nc].h = h_new
                        nodes[nr][nc].parent_row = r
                        nodes[nr][nc].parent_col = c

    return None

def astar_input(rows, cols, grid, start, goal):
    return astar_grid(grid, start, goal)

"""Task execution"""

if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1].lower() == "new":
        new_case()
    else:
        default_case()

new_case()