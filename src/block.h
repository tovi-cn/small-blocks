#ifndef BLOCK_H_
#define BLOCK_H_

#include <cassert>

// An octree block, consisting of eight child blocks
//
//   .-------.
//   | 0 | 1 | \      Top (seen from above)
//   |---+---|  \
//   | 2 | 3 |   \
//   '-------'    \
//    \    .-------.
//     \   | 4 | 5 |  Bottom (seen from above)
//      \  |---+---|
//       \ | 6 | 7 |
//         '-------'
//
class Block {
 public:
  Block(int value = 0)
      : value_(value), children_() {}

  ~Block() {
    for (int i = 0; i < 8; ++i) {
      delete children_[i];
    }
  }

  void set_child(int index, Block *child) {
    assert(index >= 0 && index < 8);
    if (value_) {
      Subdivide();
    }
    if (children_[index]) {
      delete children_[index];
    }
    children_[index] = child;
  }
  Block *child(int index) const {
    return children_[index];
  }

  bool is_leaf() const {
    for (int i = 0; i < 8; ++i) {
      if (children_[i]) {
        return false;
      }
    }
    return true;
  }

  int value() const {
    return value_;
  }
  void set_value(int value) {
    value_ = value;
    for (int i = 0; i < 8; ++i) {
      delete children_[i];
      children_[i] = nullptr;
    }
  }

  void Subdivide();
  void Simplify();

 private:
  int value_;
  Block *children_[8];
};

#endif  // BLOCK_H_
