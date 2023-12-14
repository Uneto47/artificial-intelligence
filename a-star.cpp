#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <utility>
#include <vector>
using namespace std;
#include "utils.cpp"

// Knight's Travails problem implementation
using KnightsTravailsState = pair<int, int>;
using KnightsTravailsAction = pair<int, int>;
const KnightsTravailsAction IDLE_ACTION = {0, 0};
const vector<KnightsTravailsAction> VALID_MOVES = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};

template <typename S, typename A>
struct KnightsTravailsProblem : Problem<S, A> {
  size_t table_size;

  S result(const S &state, const A &action) override {
    int x = action.first + state.first;
    int y = action.second + state.second;
    return S(x, y);
  }

  int path_cost(const int current_cost, const S &current, const A &action, const S &next) override {
    return current_cost + 1;
  }

  int heuristics(const S &next) override {
    return abs(next.first - this->goal.first) +
           abs(next.second - this->goal.second);
  }

  bool goal_test(const S &current) override {
    return current.first == this->goal.first &&
           current.second == this->goal.second;
  }

  vector<A> actions(const S &current) override {
    vector<A> valid_actions;

    for (auto move : VALID_MOVES) {
      int x = move.first + current.first;
      int y = move.second + current.second;
      if (at_bounds(x, y))
        valid_actions.push_back(move);
    }

    return valid_actions;
  }

  bool at_bounds(int x, int y) {
    return x >= 0 && x < this->table_size &&
           y >= 0 && y < this->table_size;
  }

  KnightsTravailsProblem(S initial, S goal, size_t table_size = 8)
      : Problem<S, A>(initial, goal), table_size(table_size) {}
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

// A* implementation
template <typename S, typename A>
shared_ptr<Node<S, A>> astar(Problem<S, A> &problem) {
  auto explored = map<S, shared_ptr<Node<S, A>>>{};
  auto frontier = priority_queue<
      pair<int, shared_ptr<Node<S, A>>>,
      vector<pair<int, shared_ptr<Node<S, A>>>>,
      greater<pair<int, shared_ptr<Node<S, A>>>>>{};
  frontier.push({0, make_shared<Node<S, A>>(problem.initial, IDLE_ACTION)});

  while (!frontier.empty()) {
    auto [_, current] = frontier.top();
    frontier.pop();
    explored[current->state] = current;

    for (auto node : expands(problem, current)) {
      if (problem.goal_test(node->state))
        return node;
      else if (explored.find(node->state) == explored.end() || node->cost < explored[node->state]->cost)
        frontier.push({node->cost, node});
    }
  }

  return nullptr;
}

// Get maching char for the current room
string get_room_char(vector<pair<int, int>> states, pair<int, int> current) {
  if (states[0] == current)
    return "♞";
  else if (states[states.size() - 1] == current)
    return "G";
  else if (find(states.begin(), states.end(), current) != states.end())
    return "*";
  return " ";
}

// Print table for the problem
void print_table_with_result(shared_ptr<Node<KnightsTravailsState, KnightsTravailsAction>> node, size_t table_size) {
  vector<KnightsTravailsState> states;
  auto current = node;
  while (current != nullptr) {
    states.push_back(current->state);
    current = current->parent;
  }
  reverse(states.begin(), states.end());

  cout << "Path: ";
  for (int c = 0; c < states.size() - 1; c++)
    cout << '(' << states[c].first + 1 << ','
         << states[c].second + 1 << ')' << "->";
  cout << '(' << states[states.size() - 1].first + 1 << ','
       << states[states.size() - 1].second + 1 << ')' << endl;
  cout << "┏━";
  for (int c = table_size - 1; c > 0; c--)
    cout << "━━┳━";
  cout << "━━┓" << endl;
  for (int c = table_size - 1; c > 0; c--) {
    cout << "┃ ";
    for (int k = 0; k < table_size - 1; k++)
      cout << get_room_char(states, {k, c}) << " ┃ ";
    cout << get_room_char(states, {(int)table_size - 1, c}) << " ┃" << endl;
    cout << "┣━";
    for (int k = table_size - 1; k > 0; k--)
      cout << "━━╋━";
    cout << "━━┫" << endl;
  }
  cout << "┃ ";
  for (int k = 0; k < table_size - 1; k++)
    cout << get_room_char(states, {k, 0}) << " ┃ ";
  cout << get_room_char(states, {(int)table_size - 1, 0}) << " ┃" << endl;
  cout << "┗━";
  for (int c = table_size - 1; c > 0; c--)
    cout << "━━┻━";
  cout << "━━┛" << endl;
}

const char DELIMITER = ',';

int main(int argc, char **argv) {
  if (argc < 3 || argc > 4) {
    cerr << "Usage: ./a-star [initial] [goal] [[table size] = 8]" << endl;
    cerr << "[inital] and [goal] must be a pair of integers x,y "
         << "and the range is [1, [table size]]" << endl;
    cerr << "[table size] must be an integer" << endl;
    return 1;
  }

  pair<int, int> initial, goal;
  size_t table_size = 8;

  if (argc == 4) {
    char *p;
    table_size = strtol(argv[3], &p, 10);
    if (*p) {
      cerr << "[table size] must be an interger" << endl;
      return 1;
    }
  }

  string first, second, buffer;
  char *fp, *sp;

  buffer = argv[1];
  first = buffer.substr(0, buffer.find(DELIMITER));
  buffer.erase(0, buffer.find(DELIMITER) + 1);
  second = buffer;
  initial.first = strtol(first.c_str(), &fp, 10) - 1;
  initial.second = strtol(second.c_str(), &sp, 10) - 1;
  if (buffer.find(DELIMITER) != buffer.npos || *fp || *sp) {
    cerr << "[initial] must be a pair of intergers x,y" << endl;
    return 1;
  }
  if (initial.first < 0 || initial.second > table_size - 1) {
    cerr << "[initial] is out of range" << endl;
    return 1;
  }

  buffer = argv[2];
  first = buffer.substr(0, buffer.find(DELIMITER));
  buffer.erase(0, buffer.find(DELIMITER) + 1);
  second = buffer;
  goal.first = strtol(first.c_str(), &fp, 10) - 1;
  goal.second = strtol(second.c_str(), &sp, 10) - 1;
  if (buffer.find(DELIMITER) != buffer.npos || *fp || *sp) {
    cerr << "[goal] must be a pair of intergers x,y" << endl;
    return 1;
  }
  if (goal.first < 0 || goal.second > table_size - 1) {
    cerr << "[goal] is out of range" << endl;
    return 1;
  }

  auto problem = KnightsTravailsProblem<KnightsTravailsState, KnightsTravailsAction>(initial, goal, table_size);
  auto node = astar<KnightsTravailsState, KnightsTravailsAction>(problem);
  print_table_with_result(node, table_size);
}
