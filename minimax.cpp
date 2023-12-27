#include "utils.cpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

using TicTacToeState = vector<short>;
using TicTacToeAction = size_t;
enum TicTacToeTurnValue { O = -1, EMPTY, X };

template <typename S, typename A>
struct TicTacToeGame : Game<S, A> {
  size_t table_size = 3;
  size_t table_area = 9;

  size_t ununsed_rooms(const S &current) {
    return count(current.begin(), current.end(), EMPTY);
  }

  short winner(const S &current) {
    for (size_t i = 0; i < table_area; i += table_size)
      if (current[i] != EMPTY &&
          current[i] == current[i + 1] &&
          current[i] == current[i + 2])
        return current[i];

    for (size_t i = 0; i < table_size; i++)
      if (current[i] != EMPTY &&
          current[i] == current[i + table_size] &&
          current[i] == current[i + table_size * 2])
        return current[i];

    if (current[4] != EMPTY)
      for (int i = 0; i < 3; i += 2)
        if (current[i] == current[4] && current[4] == current[2 * table_size + 2 - i])
          return current[4];

    return EMPTY;
  }

  int utility(const S &current) override {
    return winner(current);
  }

  int to_move(const S &current) override {
    return ununsed_rooms(current) % 2 == 0 ? O : X;
  }

  bool terminal_test(const S &current) override {
    return winner(current) != EMPTY || ununsed_rooms(current) == 0;
  }

  S result(const S &state, const A &action) override {
    auto next = S(table_area);
    for (size_t i = 0; i < table_area; i++)
      next[i] = state[i];
    next[action] = to_move(state);
    return next;
  }

  vector<A> actions(const S &current) override {
    vector<A> moves;
    for (size_t i = 0; i < table_area; i++)
      if (current[i] == EMPTY)
        moves.push_back(i);
    return moves;
  }
  
  TicTacToeGame(S initial = {0, 0, 0, 0, 0, 0, 0, 0, 0})
      : Game<S, A>(initial, {}) {}
};

// Return the pair result value and optimal move for the minimum player
template <typename S, typename A>
pair<int, shared_ptr<A>> min_search(Game<S, A> &game, S state, int alpha = numeric_limits<int>::min(), int beta = numeric_limits<int>::max()) {
  if (game.terminal_test(state))
    return make_pair(game.utility(state), nullptr);

  auto value = numeric_limits<int>::max();
  auto action = make_shared<A>();
  for (auto a : game.actions(state)) {
    auto [v, _] = max_search(game, game.result(state, a), alpha, beta);
    if (v < value) {
      value = v;
      (*action) = a;
      beta = min(v, beta);
    }

    if (value <= alpha)
      return make_pair(value, action);
  }

  return make_pair(value, action);
}

// Return the pair result value and optimal move for the maximum player
template <typename S, typename A>
pair<int, shared_ptr<A>> max_search(Game<S, A> &game, S state, int alpha = numeric_limits<int>::min(), int beta = numeric_limits<int>::max()) {
  if (game.terminal_test(state))
    return make_pair(game.utility(state), nullptr);

  auto value = numeric_limits<int>::min();
  auto action = make_shared<A>();
  for (auto a : game.actions(state)) {
    auto [v, _] = min_search(game, game.result(state, a), alpha, beta);
    if (v > value) {
      value = v;
      (*action) = a;
      alpha = max(v, alpha);
    }

    if (value >= beta)
      return make_pair(value, action);
  }

  return make_pair(value, action);
}

// Return the pair optimal move for the current player
template <typename S, typename A>
A minimax(Game<S, A> &game, S state) {
  auto turn = game.to_move(state);
  auto [_, action] = turn == 1 ? max_search(game, state) : min_search(game, state);
  return *action;
}

string mark(TicTacToeState board, TicTacToeAction position) {
  switch (board[position]) {
    case X:
      return "X";
    case O:
      return "O";
    default:
      return to_string(position + 1);
  }
}

void print_board(TicTacToeState board) {
  cout << "┏━━━┳━━━┳━━━┓" << endl;
  cout << "┃ " << mark(board, 0) << " ┃ " << mark(board, 1) << " ┃ " << mark(board, 2) << " ┃" << endl;
  cout << "┣━━━╋━━━╋━━━┫" << endl;
  cout << "┃ " << mark(board, 3) << " ┃ " << mark(board, 4) << " ┃ " << mark(board, 5) << " ┃" << endl;
  cout << "┣━━━╋━━━╋━━━┫" << endl;
  cout << "┃ " << mark(board, 6) << " ┃ " << mark(board, 7) << " ┃ " << mark(board, 8) << " ┃" << endl;
  cout << "┗━━━┻━━━┻━━━┛" << endl;
}

int main(void) {
  srand(time(NULL));
  cout << "╔═════════════╗" << endl;
  cout << "║ Tic Tac Toe ║" << endl;
  cout << "╚═════════════╝" << endl;

  cout << "Choose: X or O" << endl;
  string mark = "";
  while (mark.compare("X") != 0 && mark.compare("O") != 0) {
    cout << ">>> ";
    cin >> mark;
    transform(mark.begin(), mark.end(), mark.begin(), ::toupper);
  }

  auto game = TicTacToeGame<TicTacToeState, TicTacToeAction>();
  auto board = game.initial;
  TicTacToeTurnValue player = mark.compare("X") == 0 ? X : O;
  
  if (player == O) {
    vector<int> corners = {0, 2, 6, 8};
    auto random_move = corners[rand() % 4];
    board[random_move] = X;
    cout << "Computer marked at position " << (random_move + 1) << endl;
  }

  char *p = NULL;
  TicTacToeAction position = 0;
  while (!game.terminal_test(board)) {
    print_board(board);

    cout << "Choose a position: between 1 and 9" << endl;
    do {
      cout << ">>> ";
      cin >> mark;
      position = strtol(mark.c_str(), &p, 10);
    } while(*p || position < 1 || position > 9 || board[--position] != EMPTY);
    board[position] = game.to_move(board);

    if (game.terminal_test(board))
      break;

    position = minimax<TicTacToeState, TicTacToeAction>(game, board);
    board[position] = game.to_move(board);
    cout << "Computer marked at position " << (position + 1) << endl;
  }

  print_board(board);
  auto result = game.winner(board);
  if (result == EMPTY)
    cout << "It's a Tie" << endl;
  else if (player == result)
    cout << "How do you win?" << endl;
  else
    cout << "You lose, obviously!" << endl;

  return 0;
}
