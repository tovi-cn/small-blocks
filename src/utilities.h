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

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>

#include "glm/glm.hpp"

struct BoundingBox {
  BoundingBox() : BoundingBox(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {}

  BoundingBox(float left, float right, float top, float bottom,
              float front, float back)
      : left(left), right(right),
        top(top), bottom(bottom),
        front(front), back(back) {}

  float left;
  float right;
  float top;
  float bottom;
  float front;
  float back;
};

static bool Intersects(BoundingBox b1, BoundingBox b2) {
  // Assuming coordinate system used in OpenGL.
  return !(b1.left >= b2.right ||
           b1.right <= b2.left ||
           b1.top <= b2.bottom ||
           b1.bottom >= b2.top ||
           b1.front <= b2.back ||
           b1.back >= b2.front);
}

void SeedRandom();

static float RandomFloat() {
  return static_cast<float>(rand()) / RAND_MAX;
}

static float FloorNearestMultiple(float num, float multiple) {
  return glm::floor(num / multiple) * multiple;
}

bool LoadFile(const std::string &path, std::string *data);

#endif  // UTILITIES_H_
