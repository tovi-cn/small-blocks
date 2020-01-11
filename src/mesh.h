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

#ifndef MESH_H_
#define MESH_H_

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "geometry.h"
#include "material.h"

class Mesh {
 public:
  Mesh(Geometry *geometry, Material *material);
  ~Mesh();

  Geometry *geometry() const { return geometry_; }
  Material *material() const { return material_; }

  const glm::mat4 &model_matrix() const {
    return model_matrix_;
  }
  void set_model_matrix(const glm::mat4 &matrix) {
    model_matrix_ = matrix;
  }

  bool hidden() const { return hidden_; }
  void set_hidden(bool hidden) { hidden_ = hidden; }

  bool wireframe() const { return wireframe_; }
  void set_wireframe(bool wireframe) { wireframe_ = wireframe; }

  GLuint vertex_array() const { return vertex_array_; }
  const std::vector<GLuint> &vertex_buffers() const {
    return vertex_buffers_;
  }
  GLuint element_buffer() const { return element_buffer_; }

 private:
  static int GetNumVertexAttributes(Geometry *geometry);

  Geometry *geometry_;
  Material *material_;

  glm::mat4 model_matrix_;
  bool hidden_;
  bool wireframe_;

  GLuint vertex_array_;
  std::vector<GLuint> vertex_buffers_;
  GLuint element_buffer_;
};

#endif  // MESH_H_
