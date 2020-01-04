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

#include "fractals.h"

void SimpleFractal(Block *block, int depth) {
  if (depth <= 0) {
    return;
  }
  block->set_child(depth % 2 == 0 ? 0 : 7, new Block(100));
  block->set_child(depth % 2 == 0 ? 7 : 0, new Block());
  SimpleFractal(block->child(depth % 2 == 0 ? 7 : 0), depth - 1);
}
