#include <vector>
using namespace std;

class Action {};

class State {
public:
  bool operator==(const State &rhs) { return false; }
};

class Node {
public:
  State state;
  int action;
  int depth;
  int cost;
  Node *parent;

  Node(State state, int action = 0, int depth = 0, int cost = 0)
      : state(state), action(action), depth(depth), cost(cost) {}
};

class Problem {
private:
  State initial;
  State goal;

public:
  Node get_initial_node() { return Node(this->initial); }
  bool goal_test(State current) { return current == goal; }
  int heuristics(const State &state) { return 0; };
  vector<State> expand() {
    vector<State> childs;
    return childs;
  }
};
