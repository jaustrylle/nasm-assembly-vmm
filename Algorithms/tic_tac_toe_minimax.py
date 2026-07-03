# -*- coding: utf-8 -*-
"""tic-tac-toe-minimax.ipynb

# Tic-Tac-Toe with Minimax Algorithm

Implement the minimax algorithm for a Tic-Tac-Toe game to allow the AI to play optimally.
"""

import math

"""## Create a Tic-Tac-Toe board"""

board = [" " for _ in range(9)]

def print_board():
    for i in range(0, 9, 3):
        # Labels the board using numbers
        row_display = [str(i+j) if board[i+j] == " " else board[i+j] for j in range(3)]

        print(f" {row_display[0]} | {row_display[1]} | {row_display[2]} ")

        if i < 6:
            print("---+---+---")

"""## Check if the board is full"""

def is_full():
    return " " not in board

"""## Choose the best move for the AI

1. Minimax algorithm
2. Alpha-beta pruning

### Minimax search
"""

def minimax_search():
    best_val = -math.inf
    move = -1
    for i in range(9):
        if board[i] == " ":
            board[i] = "O"
            # AI move is maximizing
            move_val = minimax(0, False, -math.inf, math.inf)
            board[i] = " "
            if move_val > best_val:
                move = i
                best_val = move_val
    return move

"""### Determine max value"""

def minimax(depth, is_maximizing, alpha, beta):
    # Check terminal states
    result = is_terminal(board)
    if result == "O": return 10 - depth  # AI wins
    if result == "X": return depth - 10  # Player wins
    if is_full(): return 0               # Draw

    if is_maximizing:
        best_score = -math.inf
        for i in range(9):
            if board[i] == " ":
                board[i] = "O"
                score = minimax(depth + 1, False, alpha, beta)
                board[i] = " "
                best_score = max(score, best_score)
                alpha = max(alpha, score)
                if beta <= alpha:
                    break
        return best_score
    else:
        best_score = math.inf
        for i in range(9):
            if board[i] == " ":
                board[i] = "X"
                score = minimax(depth + 1, True, alpha, beta)
                board[i] = " "
                best_score = min(score, best_score)
                beta = min(beta, score)
                if beta <= alpha:
                    break
        return best_score

"""Make a move"""

def play():
    print("You are X and start first, the AI is O.")
    print_board()

    while True:
        # Player move
        try:
            move = int(input("\nEnter position (0-8): "))
            if move < 0 or move > 8 or board[move] != " ":
                print("Invalid move! Try again.")
                continue
        except ValueError:
            print("Please enter a number between 0 and 8.")
            continue

        board[move] = "X"
        print_board()

        if is_terminal(board) == "X":
            print("\nCongratulations! You win!")
            break
        if is_full():
            print("\nIt's a draw!")
            break

        # AI move
        print("\nAI is thinking...")
        ai_move = minimax_search()
        if ai_move != -1:
            board[ai_move] = "O"

        print_board()

        if is_terminal(board) == "O":
            print("\nAI wins! Better luck next time.")
            break
        if is_full():
            print("\nIt's a draw!")
            break

"""## Game ends when either player wins OR board is full (draw)"""

def is_terminal(state):
    win_states = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],
        [0, 3, 6], [1, 4, 7], [2, 5, 8],
        [0, 4, 8], [2, 4, 6]
    ]
    for combo in win_states:
        if state[combo[0]] == state[combo[1]] == state[combo[2]] != " ":
            return state[combo[0]]
    return None

"""## Main"""

if __name__ == "__main__":
  play()