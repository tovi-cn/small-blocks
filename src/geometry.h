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

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>

class Geometry {
 public:
  struct Position {
    float x;
    float y;
    float z;
  };

  struct Normal {
    float x;
    float y;
    float z;
  };

  struct Uv {
    float x;
    float y;
  };

  struct Color {
    float x;
    float y;
    float z;
  };

  Geometry();
  ~Geometry();

  std::vector<Position> &positions() { return positions_; }
  std::vector<Normal> &normals() { return normals_; }
  std::vector<Uv> &uvs() { return uvs_; }
  std::vector<Color> &colors() { return colors_; }
  std::vector<unsigned int> &indices() { return indices_; }

 private:
  std::vector<Position> positions_;
  std::vector<Normal> normals_;
  std::vector<Uv> uvs_;
  std::vector<Color> colors_;

  std::vector<unsigned int> indices_;
};

#endif  // GEOMETRY_H_
