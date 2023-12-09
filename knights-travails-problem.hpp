#ifndef KNIGHTS_TRAVAILS_PROBLEM_HPP
#define KNIGHTS_TRAVAILS_PROBLEM_HPP

#include "problem.hpp"
#include <cstddef>
#include <cstdlib>
#include <utility>

struct KnightsTravailsState : State {
  pair<int, int> position;

  size_t operator==(const KnightsTravailsState &rhs) const;

  KnightsTravailsState(int x, int y);
  KnightsTravailsState(pair<int, int> position);
};

struct KnightsTravailsStateHash : StateHash {
  size_t operator()(const KnightsTravailsState &state) const;
};

struct KnightsTravailsAction : Action {
  pair<int, int> move;
  KnightsTravailsAction(pair<int, int> move);
};

struct KnightsTravailsProblem : Problem {
  KnightsTravailsState initial;
  KnightsTravailsState goal;
  size_t table_size;

  KnightsTravailsState result(const KnightsTravailsState &state,
                              const KnightsTravailsAction &action);

  int path_cost(const int current_cost, const KnightsTravailsState &current,
                const KnightsTravailsAction &action,
                const KnightsTravailsState &next);

  int heuristics(const KnightsTravailsState &next);

  bool goal_test(const KnightsTravailsState &current);

  vector<KnightsTravailsAction> actions(const KnightsTravailsState &current);
  bool at_bounds(int x, int y);

  KnightsTravailsProblem(KnightsTravailsState initial,
                         KnightsTravailsState goal, size_t table_size = 8);
};

#endif
