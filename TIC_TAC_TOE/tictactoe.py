

import numpy as np
from random import choice


maximizing_player_symbol = 'x' # hero
minimizing_player_symbol = 'o' # villain



def remaining_moves_exist(board):
	return np.any(board == '_')


def evaluate_board(board):
	# check rows:
	for i in range(3):
		if (board[i][0] == board[i][1] == board[i][2]):
			if board[i][0] == maximizing_player_symbol:
				return 100
			elif board[i][0] == minimizing_player_symbol:
				return -100

	# check columns:
	for j in range(3):
		if (board[0][j] == board[1][j] == board[2][j]):
			if board[0][j] == maximizing_player_symbol:
				return 100
			elif board[0][j] == minimizing_player_symbol:
				return -100

	# check diagonals:
	if (board[0][0] == board[1][1] == board[2][2]):
		if board[1][1] == maximizing_player_symbol:
			return 100
		elif board[1][1] == minimizing_player_symbol:
			return -100

	if (board[0][2] == board[1][1] == board[2][0]):
		if board[1][1] == maximizing_player_symbol:
			return 100
		elif board[1][1] == minimizing_player_symbol:
			return -100

	return 0



def minimax(board, depth, is_maximizing_player):
	
	current_value = evaluate_board(board)

	# if maximizer has won
	if current_value == 100:
		return current_value - depth

	# if minimizer has won
	if current_value == -100:
		return current_value + depth

	# if no remaining moves, we have a tie
	if not remaining_moves_exist(board):
		return 0

	# otherwise, search the game tree

	best_value = -np.inf if is_maximizing_player else np.inf

	possible_moves = [(i,j) for i in range(3) for j in range(3) if board[i][j] == '_']

	for (i,j) in possible_moves:

		# try the move
		board[i][j] = maximizing_player_symbol if is_maximizing_player else minimizing_player_symbol

		# determine value of the move
		value = minimax(board, depth+1, not is_maximizing_player)

		# if current move is "better" than best move, update
		best_value = max(value, best_value) if is_maximizing_player else min(value, best_value)

		# undo the move
		board[i][j] = '_'

	return best_value





def find_best_move(board, is_maximizing_player):

	best_move = (np.nan, np.nan)
	best_value = -np.inf if is_maximizing_player else np.inf
	
	possible_moves = [(i,j) for i in range(3) for j in range(3) if board[i][j] == '_']

	for (i,j) in possible_moves:

		# try the move
		board[i][j] = maximizing_player_symbol if is_maximizing_player else minimizing_player_symbol

		# determine value of the move
		value = minimax(board, 0, not is_maximizing_player)

		# if current move is "better" than best move, update
		if is_maximizing_player:
			if value > best_value:
				best_value = value
				best_move = (i, j)

		else:
			if value < best_value:
				best_value = value
				best_move = (i, j)

		# undo the move
		board[i][j] = '_'

	return best_move




def play_tictactoe():

	board = np.array([['_','_','_'], ['_','_','_'], ['_','_','_']])
	turn = choice(['x', 'o'])

	while (True):
		print(board)

		if turn == maximizing_player_symbol:
			print("turn: hero = ", turn, '\n', sep='')
			best_move = find_best_move(board, True)
			board[best_move] = maximizing_player_symbol
			print("hero moves to ", best_move, '\n')

		else:
			print("turn: villain = ", turn, '\n', sep='')
			best_move = find_best_move(board, False)
			board[best_move] = minimizing_player_symbol
			print("villain moves to ", best_move, '\n')

		# either Hero or Villain has won
		if evaluate_board(board) != 0:
			print(board, '\n')
			print("game result:", evaluate_board(board))
			break

		# neither player has won and no moves exist --> tie
		elif not remaining_moves_exist(board):
			print(board, '\n')
			print("game result:", 0)
			break

		turn = 'x' if turn == 'o' else 'o'


play_tictactoe()










