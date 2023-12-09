#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include <cstddef>
#include <memory>
#include <vector>
using namespace std;

struct Problem {
  struct State {
    struct Hash {
      size_t operator()(const State &state) const;
    };
  };
  struct Action {};

  State initial;
  State goal;

  State result(const State &state, const Action &action);
  int path_cost(const State &current, const Action &action, const State &next);
  int heuristics(const State &next);
  bool goal_test(const State &current);
  vector<Action> actions(const State &current);
};

struct Node {
  Problem::State state;
  shared_ptr<Node> parent;
  int cost;

  Node(Problem::State state, shared_ptr<Node> parent, int cost)
      : state(state), parent(parent), cost(cost) {}

  struct Comparator {
    bool operator()(const shared_ptr<Node> lhs,
                    const shared_ptr<Node> rhs) const {
      return lhs->cost > rhs->cost;
    }
  };
};

#endif
