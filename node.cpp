#include "node.hpp"

Node::Node(State state, shared_ptr<Node> parent, int cost)
    : state(state), parent(parent), cost(cost){};

bool NodeComparator::operator()(const shared_ptr<Node> lhs,
                                const shared_ptr<Node> rhs) const {
  return lhs->cost > rhs->cost;
};
