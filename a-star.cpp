#include "a-star.hpp"

std::vector<Problem::State> AStar::astar(Problem problem) {
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Node::Comparator>
      frontier;
  frontier.push(std::make_shared<Node>(problem.initial, nullptr, 0));

  std::unordered_map<Problem::State, std::shared_ptr<Node>, Problem::State::Hash>
      explored;

  while (!frontier.empty()) {
    std::shared_ptr<Node> current = frontier.top();
    frontier.pop();

    if (problem.goal_test(current->state)) {
      std::vector<Problem::State> path;
      auto node = current;
      while (node != nullptr) {
        path.push_back(node->state);
        node = node->parent;
      }
      std::reverse(path.begin(), path.end());
      return path;
    }

    explored[current->state] = current;
    for (auto action : problem.actions(current->state)) {
      auto next = problem.result(current->state, action);
      auto cost = problem.path_cost(current->state, action, next);
      if (explored.find(next) == explored.end() || cost < explored[next]->cost)
        frontier.push(
            std::make_shared<Node>(next, current, cost + problem.heuristics(next)));
    }
  }

  return {};
}
