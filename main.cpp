#include <iostream>
#include "a-star.hpp"
#include "knights-travails-problem.hpp"

int main() {
  KnightsTravailsProblem::State initial(0, 0);
  KnightsTravailsProblem::State goal(7, 7);
  KnightsTravailsProblem problem(initial, goal);

  std::vector<Problem::State> solution = AStar::astar(problem);

  return 0;
}
