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
           abs(next.position.second - goal.position.second);
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
        valid_actions.push_back(KnightsTravailsAction(move));
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
    explored[current->state] = current;

    for (auto action : problem.actions(current->state)) {
      auto next = problem.result(current->state, action);
      auto cost =
          problem.path_cost(current->cost, current->state, action, next);
      auto node = make_shared<Node<StateType>>(next, current,
                                               cost + problem.heuristics(next));

      if (problem.goal_test(node->state)) {
        vector<StateType> path;
        while (node != nullptr) {
          path.push_back(node->state);
          node = node->parent;
        }
        reverse(path.begin(), path.end());
        return path;
      }

      if (explored.find(next) == explored.end() || cost < explored[next]->cost)
        frontier.push(node);
    }
  }

  return {};
}

// Get maching char for the current room
string get_room_char(vector<KnightsTravailsState> states,
                     KnightsTravailsState current) {
  if (states[0] == current)
    return "♞";
  else if (states[states.size() - 1] == current)
    return "G";
  else if (find(states.begin(), states.end(), current) != states.end())
    return "*";
  return " ";
}

// Print table for the problem
void print_table_with_result(vector<KnightsTravailsState> states,
                             size_t table_size) {
  cout << "Path: ";
  for (int c = 0; c < states.size() - 1; c++)
    cout << '(' << states[c].position.first + 1 << ','
         << states[c].position.second + 1 << ')' << "->";
  cout << '(' << states[states.size() - 1].position.first + 1 << ','
       << states[states.size() - 1].position.second + 1 << ')' << endl;
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

  auto problem = KnightsTravailsProblem(initial, goal, table_size);
  auto states = astar<KnightsTravailsState, KnightsTravailsStateHash,
                      KnightsTravailsAction, KnightsTravailsProblem>(problem);
  print_table_with_result(states, table_size);
}
