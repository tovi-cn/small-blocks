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

#include "block.h"

void Block::Simplify() {
  if (is_leaf()) {
    return;
  }

  for (int i = 0; i < kNumChildren; ++i) {
    if (children_[i]) {
      children_[i]->Simplify();
    }
  }

  // Check whether the children can be merged into a single block.
  for (int i = 0; i < kNumChildren; ++i) {
    if (!children_[i] || !children_[i]->is_leaf()) {
      return;
    }
  }
  for (int i = 0; i < kNumChildren; ++i) {
    if (children_[i]->value_ != children_[0]->value_) {
      return;
    }
  }

  value_ = children_[0]->value_;
  for (int i = 0; i < kNumChildren; ++i) {
    delete children_[i];
    children_[i] = nullptr;
  }
}

void Block::Subdivide() {
  assert(is_leaf());
  for (int i = 0; i < kNumChildren; ++i) {
    children_[i] = new Block(value_);
  }
  value_ = 0;
}
