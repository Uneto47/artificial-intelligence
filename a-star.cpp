#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

// Problem interface implementation
struct State {
  friend ostream &operator<<(ostream &os, const State &state) {
    os << "blank";
    return os;
  }
  size_t operator==(const State &rhs) const { return 0; }
};

struct StateHash {
  size_t operator()(const State &state) const { return 0; }
};

struct Action {};

struct Problem {
  State initial;
  State goal;

  State result(const State &state, const Action &action) { return {}; }
  int path_cost(const int current_cost, const State &current,
                const Action &action, const State &next) {
    return 0;
  }
  int heuristics(const State &next) { return 0; }
  bool goal_test(const State &current) { return false; }
  vector<Action> actions(const State &current) { return {}; }
};

// Knight's Travails problem implementation
const vector<pair<int, int>> VALID_MOVES = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                            {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};

struct KnightsTravailsState : State {
  pair<int, int> position;

  friend ostream &operator<<(ostream &os, const KnightsTravailsState &state) {
    os << state.position.first << ',' << state.position.second;
    return os;
  }

  size_t operator==(const KnightsTravailsState &rhs) const {
    return this->position.first == rhs.position.first &&
           this->position.second == rhs.position.second;
  }

  KnightsTravailsState(int x, int y) : position({x, y}) {}
  KnightsTravailsState(pair<int, int> position) : position(position) {}
};

struct KnightsTravailsStateHash : StateHash {
  size_t operator()(const KnightsTravailsState &state) const {
    return (state.position.first * 0x1f1f1f1f) ^ state.position.second;
  }
};

struct KnightsTravailsAction : Action {
  pair<int, int> move;
  KnightsTravailsAction(int x, int y) : move({x, y}) {}
  KnightsTravailsAction(pair<int, int> move) : move(move) {}
};

struct KnightsTravailsProblem : Problem {
  KnightsTravailsState initial;
  KnightsTravailsState goal;
  size_t table_size;

  KnightsTravailsState result(const KnightsTravailsState &state,
                              const KnightsTravailsAction &action) {
    int x = action.move.first + state.position.first;
    int y = action.move.second + state.position.second;
    return KnightsTravailsState(x, y);
  }

  int path_cost(const int current_cost, const KnightsTravailsState &current,
                const KnightsTravailsAction &action,
                const KnightsTravailsState &next) {
    return current_cost + 1;
  }

  int heuristics(const KnightsTravailsState &next) {
    return abs(next.position.first - goal.position.first) +
           abs(next.position.second - goal.position.second) - 1;
  }

  bool goal_test(const KnightsTravailsState &current) {
    return current.position.first == this->goal.position.first &&
           current.position.second == this->goal.position.second;
  }

  vector<KnightsTravailsAction> actions(const KnightsTravailsState &current) {
    vector<KnightsTravailsAction> valid_actions;
    auto position = current.position;

    for (auto move : VALID_MOVES) {
      int x = move.first + position.first;
      int y = move.second + position.second;
      if (at_bounds(x, y))
        valid_actions.push_back(KnightsTravailsAction(x, y));
    }

    return valid_actions;
  }
  bool at_bounds(int x, int y) {
    return x >= 0 && x < this->table_size && y >= 0 && y < this->table_size;
  }

  KnightsTravailsProblem(KnightsTravailsState initial,
                         KnightsTravailsState goal, size_t table_size = 8)
      : initial(initial), goal(goal), table_size(table_size) {}
};

// Search tree node for A*
template <typename StateType = State> struct Node {
  StateType state;
  shared_ptr<Node<StateType>> parent;
  int cost;

  Node(StateType state, shared_ptr<Node<StateType>> parent, int cost)
      : state(state), parent(parent), cost(cost){};
};

template <typename StateType = State> struct NodeComparator {
  bool operator()(const shared_ptr<Node<StateType>> lhs,
                  const shared_ptr<Node<StateType>> rhs) const {
    return lhs->cost > rhs->cost;
  };
};

// A* implementation
template <typename StateType = State, typename StateHashType = StateHash,
          typename ActionType = Action, typename ProblemType = Problem>
vector<StateType> astar(ProblemType problem) {
  priority_queue<shared_ptr<Node<StateType>>,
                 vector<shared_ptr<Node<StateType>>>, NodeComparator<StateType>>
      frontier;
  frontier.push(make_shared<Node<StateType>>(problem.initial, nullptr, 0));

  unordered_map<StateType, shared_ptr<Node<StateType>>, StateHash> explored;

  while (!frontier.empty()) {
    shared_ptr<Node<StateType>> current = frontier.top();
    frontier.pop();
    cout << frontier.top()->state << endl;

    if (problem.goal_test(current->state)) {
      vector<StateType> path;
      auto node = current;
      while (node != nullptr) {
        path.push_back(node->state);
        node = node->parent;
      }
      reverse(path.begin(), path.end());
      return path;
    }

    explored[current->state] = current;
    for (auto action : problem.actions(current->state)) {
      auto next = problem.result(current->state, action);
      auto cost =
          problem.path_cost(current->cost, current->state, action, next);
      if (explored.find(next) == explored.end() || cost < explored[next]->cost)
        frontier.push(make_shared<Node<StateType>>(
            next, current, cost + problem.heuristics(next)));
    }
  }

  return {};
}

int main(void) {
  auto states = astar<KnightsTravailsState, KnightsTravailsStateHash,
                      KnightsTravailsAction, KnightsTravailsProblem>(
      KnightsTravailsProblem({0, 0}, {7, 7}));
  cout << states.size() << endl;
}
