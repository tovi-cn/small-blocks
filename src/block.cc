#include "block.h"

void Block::Simplify() {
  for (int i = 0; i < 8; ++i) {
    if (children_[i]) {
      children_[i]->Simplify();
    }
  }
  bool equal_children = true;
  for (int i = 0; i < 8; ++i) {
    if (children_[i] && children_[i]->value_ != children_[0]->value_) {
      equal_children = false;
      break;
    }
  }
  if (equal_children) {
    value_ = children_[0]->value_;
    for (int i = 0; i < 8; ++i) {
      delete children_[i];
      children_[i] = nullptr;
    }
  }
}

void Block::Subdivide() {
  assert(is_leaf());
  for (int i = 0; i < 8; ++i) {
    children_[i] = new Block(value_);
  }
  value_ = 0;
}
