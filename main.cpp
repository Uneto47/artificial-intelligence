#include "a-star.hpp"
#include "knights-travails-problem.hpp"

int main() {
  auto problem = KnightsTravailsProblem({0, 0}, {7, 7});
  auto solution = astar(problem);

  return 0;
}
