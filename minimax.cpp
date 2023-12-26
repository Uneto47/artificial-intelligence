#include "utils.cpp"
#include <algorithm>
#include <limits>

using TicTacToeState = vector<short>;
using TicTacToeAction = size_t;
enum TicTacToeTurnValue { O = -1,
                          EMPTY,
                          X };

template <typename S, typename A>
struct TicTacToeGame : Game<S, A> {
  size_t table_size = 3;
  size_t table_area = 9;

  size_t ununsed_rooms(const S &current) {
    return count(current.begin(), current.end(), EMPTY);
  }

  short winner(const S &current) {
    for (int i = 0; i < table_area; i += table_size)
      if (current[i] != EMPTY &&
          current[i] == current[i + 1] &&
          current[i] == current[i + 2])
        return current[i];

    for (int i = 0; i < table_size; i++)
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
    return to_move(current);
  }

  int to_move(const S &current) override {
    return ununsed_rooms(current) % 2 == 0 ? O : X;
  }

  bool terminal_test(const S &current) override {
    return winner(current) != EMPTY || ununsed_rooms(current) == 0;
  }

  S result(const S &state, const A &action) override {
    auto next = S(state);
    auto [i, j] = action;
    next[i][j] = to_move(state);
    return next;
  }

  vector<A> actions(const S &current) override {
    vector<A> moves;
    for (int i = 0; i < table_size; i++)
      if (current[i] != EMPTY)
        moves.push_back(i);
    return moves;
  }
};

template <typename S, typename A>
pair<int, shared_ptr<A>> min_search(Game<S, A> game, S state, int alpha = numeric_limits<int>::min(), int beta = numeric_limits<int>::max()) {
  if (game.terminal_test(state))
    return make_pair(game.utility(state), nullptr);

  auto value = numeric_limits<int>::max();
  shared_ptr<A> action = nullptr;
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

template <typename S, typename A>
pair<int, shared_ptr<A>> max_search(Game<S, A> game, S state, int alpha = numeric_limits<int>::min(), int beta = numeric_limits<int>::max()) {
  if (game.terminal_test(state))
    return make_pair(game.utility(state), nullptr);

  auto value = numeric_limits<int>::min();
  shared_ptr<A> action = nullptr;
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

template <typename S, typename A>
A minimax(Game<S, A> game, S state) {
  auto turn = game.to_move(state);
  auto [_, action] = turn == 1 ? max_search(game, state) : min_search(game, state);
  return *action;
}

int main(int argc, char **argv) {
  return 0;
}
