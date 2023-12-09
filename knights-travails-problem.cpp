#include "knights-travails-problem.hpp"

const vector<pair<int, int>> VALID_MOVES = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                            {1, 2}, {-1, 2}, {1, -2}, {-1, -2}};

size_t KnightsTravailsState::operator==(const KnightsTravailsState &rhs) const {
  return this->position.first == rhs.position.first &&
         this->position.second == rhs.position.second;
}

KnightsTravailsState::KnightsTravailsState(int x, int y) : position({x, y}) {}
KnightsTravailsState::KnightsTravailsState(pair<int, int> position)
    : position(position) {}

size_t
KnightsTravailsStateHash::operator()(const KnightsTravailsState &state) const {
  return (state.position.first * 0x1f1f1f1f) ^ state.position.second;
}

KnightsTravailsAction::KnightsTravailsAction(pair<int, int> move)
    : move(move) {}

KnightsTravailsState
KnightsTravailsProblem::result(const KnightsTravailsState &state,
                               const KnightsTravailsAction &action) {
  int x = action.move.first + state.position.first;
  int y = action.move.second + state.position.second;
  return KnightsTravailsState(x, y);
}
int KnightsTravailsProblem::path_cost(const int current_cost,
                                      const KnightsTravailsState &current,
                                      const KnightsTravailsAction &action,
                                      const KnightsTravailsState &next) {
  return current_cost + 1;
}

int KnightsTravailsProblem::heuristics(const KnightsTravailsState &next) {
  return abs(next.position.first - goal.position.first) +
         abs(next.position.second - goal.position.second) - 1;
}

bool KnightsTravailsProblem::goal_test(const KnightsTravailsState &current) {
  return current.position.first == this->goal.position.first &&
         current.position.second == this->goal.position.second;
}

vector<KnightsTravailsAction>
KnightsTravailsProblem::actions(const KnightsTravailsState &current) {
  vector<KnightsTravailsAction> valid_actions;
  auto position = current.position;

  for (auto move : VALID_MOVES) {
    int x = move.first + position.first;
    int y = move.second + position.second;
    if (at_bounds(x, y))
      valid_actions.push_back(KnightsTravailsAction({x, y}));
  }

  return valid_actions;
}

bool KnightsTravailsProblem::at_bounds(int x, int y) {
  return x >= 0 && x < this->table_size && y >= 0 && y < this->table_size;
}

KnightsTravailsProblem::KnightsTravailsProblem(KnightsTravailsState initial,
                                               KnightsTravailsState goal,
                                               size_t table_size)
    : initial(initial), goal(goal), table_size(table_size) {}
