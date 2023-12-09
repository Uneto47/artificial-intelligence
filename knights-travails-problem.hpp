#ifndef KNIGHTS_TRAVAILS_PROBLEM_HPP
#define KNIGHTS_TRAVAILS_PROBLEM_HPP

#include "problem.hpp"
#include <cstddef>
#include <cstdlib>
#include <utility>

const vector<pair<int, int>> VALID_MOVES = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                            {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};

struct KnightsTravailsProblem : Problem {
  struct State : Problem::State {
    pair<int, int> position;

    State(int x, int y) { this->position = {x, y}; }

    struct Hash : Problem::State::Hash {
      size_t operator()(const State &state) const {
        return (state.position.first * 0x1f1f1f1f) ^ state.position.second;
      }
    };
  };

  struct Action : Problem::Action {
    pair<int, int> move;
    Action(pair<int, int> move) : move(move) {}
  };

  State initial;
  State goal;
  size_t table_size;

  State result(const State &state, const Action &action) {
    int x = action.move.first + state.position.first;
    int y = action.move.second + state.position.second;

    return State(x, y);
  }

  int path_cost(const int current_cost, const State &current,
                const Action &action, const State &next) {
    return current_cost + 1;
  }

  int heuristics(const State &next) {
    return abs(next.position.first - goal.position.first) +
           abs(next.position.second - goal.position.second) - 1;
  }

  bool goal_test(const State &current) {
    return current.position.first == this->goal.position.first &&
           current.position.second == this->goal.position.second;
  }

  vector<Action> actions(const State &current) {
    vector<Action> valid_actions;
    auto position = current.position;

    for (auto move : VALID_MOVES) {
      int x = move.first + position.first;
      int y = move.second + position.second;
      if (at_bounds(x, y))
        valid_actions.push_back(Action({x, y}));
    }

    return valid_actions;
  }

  bool at_bounds(int x, int y) {
    return x >= 0 && x < this->table_size && y >= 0 && y < this->table_size;
  }

  KnightsTravailsProblem(State initial, State goal, size_t table_size = 8)
      : initial(initial), goal(goal), table_size(table_size) {}
};

#endif
