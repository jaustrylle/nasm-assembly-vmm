# -*- coding: utf-8 -*-
"""MazeNavigation.ipynb

# This robot navigates a maze by moving in four directions: up, down, left and right. It tracks state spaces, and avoids both obstacles and walls.
"""

from collections import deque

"""State representation"""

def nav_maze(maze, start, goal):
  # State space
  rows = len(maze)
  cols = len(maze[0])

  # State represented as (x,y) tuple
  # Start from given start position
  start = start

  # Navigate robot to goal position
  goal = goal

  # Handle actions; robot can move up, down, left or right
  actions = [(-1,0), (1,0), (0,-1), (0,1)]

  # Uses BFS for queue of stores
  # Stores are (current_pos, path_taken)
  queue = deque([(start, [start])])
  visited = set([start])      # Memory

  # Each state represents robot's current position in maze
  while queue:
    (curr_x, curr_y), path = queue.popleft()

    # Check if goal reached
    if(curr_x, curr_y) == goal:
      return path

    # Search in actions list:
    # For each valid action, robot's position/state changes
    # If move blocked by wall, state does not change
    for dx, dy in actions:
      next_x, next_y = curr_x + dx, curr_y + dy

      # Check obstacles and nearby cell, from start to goal; x=row, y=col in grid
      # Robot explores all possible states, avoiding walls along its way to its goal
      if 0 <= next_x < rows and 0 <= next_y < cols:
        if maze[next_x][next_y] == 0 and (next_x, next_y) not in visited:
          visited.add((next_x, next_y))
          queue.append(((next_x, next_y), path + [(next_x, next_y)]))

  return None     # If goal is unreachable

"""Actions and task execution"""

# Maze = 2D grid, empty spaces = 0, walls = 1
# Input = 2D maze is a list of lists (0=empty space, 1=wall)
maze = [
    [0, 1, 0, 0, 0],
    [0, 1, 0, 1, 0],
    [0, 0, 0, 1, 0],
    [1, 1, 0, 0, 0],
    [0, 0, 0, 1, 0]
]

# Execution
start = (0,0)
goal = (4,4)

path = nav_maze(maze, start, goal)

# Output = steps taken to reach the goal position
if path:
    print(f"Path from {start} to {goal}: {path}")

    # Copy the maze for printing with '*' marking path
    maze_copy = [row[:] for row in maze]

    # Replace path cells with "*"
    for r, c in path:
        maze_copy[r][c] = "*"

    # Print newly formatted maze
    for row in maze_copy:
        print("     " + " ".join(str(cell) for cell in row))
else:
    print("Goal is unreachable.")