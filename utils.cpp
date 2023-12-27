#include <cstddef>
#include <memory>
#include <vector>
using namespace std;

// Problem interface implementation
template <typename S, typename A>
struct Problem {
  S initial, goal;

  virtual S result(const S &state, const A &action) = 0;
  virtual int path_cost(const int current_cost, const S &current, const A &action, const S &next) = 0;
  virtual int heuristics(const S &next) = 0;
  virtual bool goal_test(const S &current) = 0;
  virtual vector<A> actions(const S &current) = 0;

  Problem(S initial, S goal)
      : initial(initial), goal(goal) {}
};

template <typename S, typename A>
struct Game {
  S initial, goal;

  virtual int utility(const S &current) = 0;
  virtual int to_move(const S &current) = 0;
  virtual bool terminal_test(const S &current) = 0;
  virtual S result(const S &state, const A &action) = 0;
  virtual vector<A> actions(const S &current) = 0;

  Game(S initial, S goal)
      : initial(initial), goal(goal) {}
};

// Tree node implementation
template <typename S, typename A>
struct Node {
  S state;
  A action;
  shared_ptr<Node<S, A>> parent;
  int cost;

  Node(S state, A action, shared_ptr<Node<S, A>> parent = nullptr, int cost = 0)
      : state(state), action(action), parent(parent), cost(cost){};
};

template <typename S, typename A>
vector<shared_ptr<Node<S, A>>> expands(Problem<S, A> &problem, shared_ptr<Node<S, A>> node) {
  vector<shared_ptr<Node<S, A>>> results;
  for (auto action : problem.actions(node->state)) {
    auto next = problem.result(node->state, action);
    auto cost = problem.path_cost(node->cost, node->state, action, next);
    auto child = make_shared<Node<S, A>>(next, action, node, cost + problem.heuristics(next));
    results.push_back(child);
  }
  return results;
}

// Check if position is at bounds of the table
bool at_bounds(size_t x, size_t y, size_t table_size) {
  return x >= 0 && x < table_size &&
         y >= 0 && y < table_size;
}

bool at_bounds(pair<size_t, size_t> position, size_t table_size) {
  return position.first >= 0 && position.first < table_size &&
         position.second >= 0 && position.second < table_size;
}
