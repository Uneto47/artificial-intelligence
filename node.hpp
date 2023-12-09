#ifndef NODE_H
#define NODE_H

#include "problem.hpp"
#include <memory>

struct Node {
  State state;
  shared_ptr<Node> parent;
  int cost;

  Node(State state, shared_ptr<Node> parent, int cost);
};

struct NodeComparator {
  bool operator()(const shared_ptr<Node> lhs, const shared_ptr<Node> rhs) const;
};

#endif
