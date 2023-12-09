#ifndef A_STAR_HPP
#define A_STAR_HPP

#include "problem.hpp"
#include <algorithm>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class AStar {
public:
  static std::vector<Problem::State> astar(Problem problem);
};

#endif