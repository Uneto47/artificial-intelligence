#include "a-star.hpp"
#include <algorithm>
#include <memory>
#include <queue>
#include <unordered_map>

vector<Problem::State> astar(Problem problem) {
  priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, Node::Comparator>
      frontier;
  frontier.push(make_shared<Node>(problem.initial, nullptr, 0));

  unordered_map<Problem::State, shared_ptr<Node>, Problem::State::Hash>
      explored;

  while (!frontier.empty()) {
    shared_ptr<Node> current = frontier.top();
    frontier.pop();

    if (problem.goal_test(current->state)) {
      vector<Problem::State> path;
      auto node = current;
      while (node != nullptr) {
        path.push_back(node->state);
        node = node->parent;
      }
      reverse(path.begin(), path.end());
      return path;
    }

    explored[current->state] = current;
    for (auto action : problem.actions(current->state)) {
      auto next = problem.result(current->state, action);
      auto cost = problem.path_cost(current->state, action, next);
      if (explored.find(next) == explored.end() || cost < explored[next]->cost)
        frontier.push(
            make_shared<Node>(next, current, cost + problem.heuristics(next)));
    }
  }

  return {};
}
