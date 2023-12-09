#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include <cstddef>
#include <vector>
using namespace std;

struct State {
  size_t operator==(const State &rhs) const;
};

struct StateHash {
  size_t operator()(const State &state) const;
};

struct Action {};

struct Problem {
  State initial;
  State goal;

  State result(const State &state, const Action &action);
  int path_cost(const State &current, const Action &action, const State &next);
  int heuristics(const State &next);
  bool goal_test(const State &current);
  vector<Action> actions(const State &current);
};

#endif
