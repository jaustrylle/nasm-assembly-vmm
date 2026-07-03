# -*- coding: utf-8 -*-

# Traveling Salesperson Problem (TSP) using Simulated Annealing

* salesperson needs to visit N cities, start from random city, return to start
* minimize total travel distance
* use simulated annealing algorithm to optimize route

Globals
"""

import random
import math
import matplotlib.pyplot as plt
import numpy as np
import sys

"""Generate cities

* N cities randomly generated as (x,y) coordinates in 2D plane
* No 2 cities can have the same coordinates
"""

def get_num_cities(default=10):
    N = default

    try:
        # Try reading sys.argv (command-line)
        if len(sys.argv) > 1:
            arg = sys.argv[1]

            # Only accept valid positive integers
            if arg.isdigit() and int(arg) > 0:
                N = int(arg)
            else:
                print(f"Invalid argument '{arg}', using default N={default}")

    except Exception:
        # If sys.argv fails (e.g., Jupyter), fallback to user input
        try:
            user_input = input(f"Enter number of cities [default {default}]: ")
            if user_input.strip() != "":
                N = int(user_input)
        except Exception:
            # If input also fails, keep default
            N = default

    return N

def generate_cities(N):
    if N < 2:
        raise ValueError("Need at least 2 cities for the grid corners.")

    cities = []

    # middle cities are 2 float vals in range (0,1)
    coords_seen = set()
    while len(coords_seen) < N:
        x, y = random.random(), random.random()
        coords_seen.add((x, y))

    cities.extend(coords_seen)

    return cities

"""Distance function"""

# calculate Euclidean distance between 2 cities

def euclidean_distance(city1, city2):
    x1, y1 = city1
    x2, y2 = city2
    return math.dist((x1, y1), (x2, y2))

"""Tour cost"""

# compute total tour distance
# ASSUMPTION: at least 1 direct road between every city pair

def total_tour(tour):
    total = 0

    for i in range(len(tour) - 1):
        total += euclidean_distance(tour[i], tour[i+1])

    total += euclidean_distance(tour[-1], tour[0])  # return to start
    return total

"""Neighbor function"""

# swap 2 random cities in curr path to gen new candidate solution

def swap_cities(path):
    if len(path) < 2:
      raise ValueError("Path must contain at least two cities.")

    new_path = path.copy()

    i, j = random.sample(range(len(path)), 2)

    new_path[i], new_path[j] = new_path[j], new_path[i]

    return new_path

"""Simulated Annealing algorithm"""

def tour_cities(cities, T=100, cooling_rate=0.995, interval=50):     # initial value defined for T
  # start with initial rand tour
  current_tour = cities.copy()
  random.shuffle(current_tour)

  current_cost = total_tour(current_tour)

  best_tour = current_tour.copy()
  best_cost = current_cost

  history = []
  iteration = 0     # interval is how many iterations between plots

  # use simulated annealing algorithm
  while T > 1e-3:
    # STEP 1 - gen new tour by swapping 2 cities
    candidate = swap_cities(current_tour)
    candidate_cost = total_tour(candidate)

    delta = candidate_cost - current_cost

    # STEP 2 - accept new tour with prob based on T
    if delta < 0 or random.random() < math.exp(-delta / T):
      current_tour = candidate
      current_cost = candidate_cost

      # STEP 3 - repeat for each step, track best sol
      if current_cost < best_cost:
        best_tour = current_tour.copy()
        best_cost = current_cost

      history.append(best_cost)     # plots
      iteration += 1

      # Plot every `interval` iterations
      if iteration % interval == 0:
        plot_progress(current_tour, iteration, best_cost)

    # STEP 4 - reduce T gradually, stop when T = 0
    T *= cooling_rate

  return best_tour, best_cost, history

"""Display and visualization"""

def print_cities_grid(cities):
    N = len(cities)
    # Determine grid size (approximate square)
    grid_size = math.ceil(N**0.5)

    for i in range(0, N, grid_size):
        row = cities[i:i+grid_size]
        print(" | ".join(f"({c[0]:.2f},{c[1]:.2f})" for c in row))

def plot_tour(tour):

    x = [city[0] for city in tour]
    y = [city[1] for city in tour]

    x.append(tour[0][0])
    y.append(tour[0][1])

    # Separate start city
    start_city = tour[0]
    other_cities = tour[1:]

    # x and y for other cities
    x = [c[0] for c in other_cities]
    y = [c[1] for c in other_cities]

    # x and y for start city
    x_start = start_city[0]
    y_start = start_city[1]

    # Plot
    plt.figure(figsize=(6,6))
    plt.plot([c[0] for c in tour] + [tour[0][0]],   # tour line
             [c[1] for c in tour] + [tour[0][1]],
             'o-', color='blue', label='Tour')

    # Highlight start city in red
    plt.scatter(x_start, y_start, color='red', s=150, label='Start City (0,0)')

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title("TSP Tour with Start City Highlighted")
    plt.legend()
    plt.grid(True)
    plt.show()

def plot_optimization(tour, history):

    x = [city[0] for city in tour]
    y = [city[1] for city in tour]

    x.append(tour[0][0])
    y.append(tour[0][1])

    plt.figure(figsize=(6,6))
    plt.plot(history)
    plt.title("Simulated Annealing Optimization")
    plt.xlabel("Iteration")
    plt.ylabel("Tour Distance")
    plt.show()

def plot_progress(tour, iteration, cost):
    x = [city[0] for city in tour]
    y = [city[1] for city in tour]
    x.append(tour[0][0])
    y.append(tour[0][1])

    plt.figure(figsize=(5,5))
    plt.plot(x, y, marker='o')
    plt.title(f"Iteration {iteration} - Distance: {cost:.3f}")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.show()

def plot_cities_grid(cities):
    x = [c[0] for c in cities]
    y = [c[1] for c in cities]

    plt.figure(figsize=(6,6))
    plt.scatter(x, y, c='blue', s=100)

    for i, (xi, yi) in enumerate(cities):
        plt.text(xi + 0.01, yi + 0.01, str(i), fontsize=9)  # label cities

    plt.xlim(0, 1)
    plt.ylim(0, 1)
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title("Cities on 2D Grid")
    plt.grid(True)
    plt.show()

"""Main"""

# Check if user passed number of cities
N = get_num_cities()

cities = generate_cities(N)
print(f"Generated {N} cities:")

best_tour, best_cost, history = tour_cities(cities, T=100, cooling_rate=0.995, interval=50)

plot_optimization(best_tour, history)

print("Best tour distance:", best_cost)

print("Best tour of cities:")
print_cities_grid(best_tour)

plot_cities_grid(best_tour)

plot_tour(best_tour)