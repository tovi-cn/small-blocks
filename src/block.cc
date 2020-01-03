#include "block.h"

void Block::Simplify() {
  if (is_leaf()) {
    return;
  }
  for (int i = 0; i < 8; ++i) {
    if (children_[i]) {
      children_[i]->Simplify();
    }
  }
  for (int i = 0; i < 8; ++i) {
    if (!children_[i] || !children_[i]->is_leaf()) {
      return;
    }
  }
  for (int i = 0; i < 8; ++i) {
    if (children_[i]->value_ != children_[0]->value_) {
      return;
    }
  }
  value_ = children_[0]->value_;
  for (int i = 0; i < 8; ++i) {
    delete children_[i];
    children_[i] = nullptr;
  }
}

void Block::Subdivide() {
  assert(is_leaf());
  for (int i = 0; i < 8; ++i) {
    children_[i] = new Block(value_);
  }
  value_ = 0;
}
