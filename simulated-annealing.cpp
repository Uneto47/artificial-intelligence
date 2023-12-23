#include "utils.cpp"
#include <cmath>
#include <cstdlib>

template <typename S, typename A>
S simulated_annealing(Problem<S, A> problem) {
  size_t time = 0;
  auto current = problem.initial;

  while (problem.goal_test(current)) {
    auto temperature = problem.heuristics(time);
    if (temperature == 0)
      return current;

    auto moves = problem.actions(current);
    auto action = moves[rand() % moves.size()];
    auto next = problem.result(current, action);

    auto cost = problem.path_cost(0, current, action, next);
    if (cost > 0 || rand() / static_cast<double>(RAND_MAX) < exp(cost / (double)temperature))
      current = next;
  }

  return current;
}
