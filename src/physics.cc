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

#include "physics.h"

Body::Body()
   : position_(0.0f), velocity_(0.0f), acceleration_(0.0f),
     fixed_(false) {}

Body::~Body() {
}

BoxBody::BoxBody(glm::vec3 size) : size_(size) {}

BoxBody::~BoxBody() {
}
