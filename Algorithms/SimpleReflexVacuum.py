# -*- coding: utf-8 -*-
"""SimpleReflexVacuum

# Performance-measuring environment simulator
"""

import random

class VacuumEnvironment:
    def __init__(self, length=3, width=3, dirt_probability=0.66, obstacles=None, start=(0,0)):
        self.length = length
        self.width = width
        self.dirt_probability = dirt_probability
        self.obstacles = obstacles if obstacles else set()
        self.start = start

        # Create grid with dirt randomly placed
        self.room = []
        for i in range(width):
            row = []
            for j in range(length):
                if (i, j) in self.obstacles:
                    row.append('X')  # obstacle
                else:
                    # 66% chance of dirt
                    if random.random() < self.dirt_probability:
                        row.append(1)  # dirt
                    else:
                        row.append(0)  # clean
            self.room.append(row)

  # each move costs 1, cleaning room increases performance val by 10
    def print_room(self):
        for row in self.room:
            print("[", ", ".join(str(cell) for cell in row), "]", sep="")
        print()

  # stop when room is clean
    def is_clean(self):
      for row in self.room:
          for cell in row:
              if cell == 1:
                  return False
      return True

class SimpleReflexVacuum:
    def __init__(self, environment):
      # obstacles avoidable by agent, num and loc given as parameter in grid (0,1)
        self.env = environment
        self.position = environment.start
        self.performance = 0
        self.steps = 0
        self.visited = set()
        self.last_position = None

    def move(self):
        x, y = self.position
        self.visited.add(self.position)

        # Right first, then down, then left, then last up
        moves = [(x, y+1), (x+1, y), (x, y-1), (x-1, y)]

        # Move to adjacent dirty square
        for nx, ny in moves:
            if 0 <= nx < self.env.width and 0 <= ny < self.env.length:
                if (nx, ny) not in self.env.obstacles:
                    if self.env.room[nx][ny] == 1:
                        self.last_position = self.position
                        self.position = (nx, ny)
                        self.clean()
                        self.performance -= 1
                        self.env.print_room()
                        return

        # Move to unvisited square
        for nx, ny in moves:
            if 0 <= nx < self.env.width and 0 <= ny < self.env.length:
                if (nx, ny) not in self.env.obstacles and (nx, ny) not in self.visited:
                    self.last_position = self.position
                    self.position = (nx, ny)
                    self.clean()
                    self.performance -= 1
                    self.env.print_room()
                    return

        # Finally allow revisiting
        for nx, ny in moves:
            if 0 <= nx < self.env.width and 0 <= ny < self.env.length:
                if (nx, ny) not in self.env.obstacles:
                    self.last_position = self.position
                    self.position = (nx, ny)
                    self.clean()
                    self.performance -= 1
                    self.env.print_room()
                    return

    def clean(self):
      # 1 = dirty, 0 = clean, X = obstacle
        x, y = self.position
        if self.env.room[x][y] == 1:
            self.env.room[x][y] = 0
            self.performance += 10

    def run(self, steps=100):
        for _ in range(steps):
            self.steps += 1
            self.move()
            self.clean()
            if self.env.is_clean():
              break

        print("Final performance:", self.performance, "\n")
        print("Final position:", self.position, "\n")

# initial loc of agent given as parameter BEFORE execution (1,0)
env = VacuumEnvironment(
    length=3,
    width=3,
    dirt_probability=0.66,
    obstacles={(0,0), (1,1)},
    start=(1,0)
)

agent = SimpleReflexVacuum(env)
agent.run(steps=100)