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

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "glm/glm.hpp"

#include "utilities.h"

class Body {
 public:
  Body();
  ~Body();

  glm::vec3 &position() { return position_;}
  glm::vec3 &velocity() { return velocity_;}
  glm::vec3 &acceleration() { return acceleration_;}

  bool is_fixed() const { return fixed_; }
  void set_fixed(bool fixed) { fixed_ = fixed; }

  virtual bool CollidesWith(Body *body) const = 0;
  virtual BoundingBox GetBoundingBox() const = 0;

 protected:
  glm::vec3 position_;
  glm::vec3 velocity_;
  glm::vec3 acceleration_;

  bool fixed_;
};

class BoxBody : public Body {
 public:
  explicit BoxBody(glm::vec3 size);
  ~BoxBody();

  glm::vec3 &size() { return size_; }

  virtual bool CollidesWith(Body *body) const {
    if (dynamic_cast<BoxBody *>(body)) {
      BoundingBox box1 = GetBoundingBox();
      BoundingBox box2 = body->GetBoundingBox();
      return Intersects(box1, box2);
    }
    return false;
  }

  virtual BoundingBox GetBoundingBox() const {
    BoundingBox box;
    box.left = position_.x - size_.x / 2.0f;
    box.right = position_.x + size_.x / 2.0f;
    box.top = position_.y + size_.y / 2.0f;
    box.bottom = position_.y - size_.y / 2.0f;
    box.front = position_.z + size_.z / 2.0f;
    box.back = position_.z - size_.z / 2.0f;
    return box;
  }

 private:
  glm::vec3 size_;
};

#endif  // PHYSICS_H_
