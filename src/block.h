// Copyright (C) 2020 Carl Enlund
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef BLOCK_H_
#define BLOCK_H_

#include <cassert>

// An octree block, consisting of eight child blocks
//
//  .-------.
//  | 0 | 1 | \  Top (as seen from above)
//  |---+---|  \
//  | 2 | 3 |   \
//  '-------'    \
//   \    .-------.
//    \   | 4 | 5 |  Bottom (as seen from above)
//     \  |---+---|
//      \ | 6 | 7 |
//        '-------'
//
class Block {
 public:
  static const int kNumChildren = 8;

  Block(int value = 0)
      : value_(value), children_() {}

  ~Block() {
    for (int i = 0; i < kNumChildren; ++i) {
      delete children_[i];
    }
  }

  void set_child(int index, Block *child) {
    assert(index >= 0 && index < kNumChildren);
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
    for (int i = 0; i < kNumChildren; ++i) {
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
    for (int i = 0; i < kNumChildren; ++i) {
      delete children_[i];
      children_[i] = nullptr;
    }
  }

  void Subdivide();
  void Simplify();

 private:
  int value_;
  Block *children_[kNumChildren];
};

#endif  // BLOCK_H_
