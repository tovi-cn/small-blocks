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

#include "mesh.h"

Mesh::Mesh(Geometry *geometry, Material *material)
    : geometry_(geometry),
      material_(material),
      model_matrix_(1.0f),
      hidden_(false),
      wireframe_(false),
      vertex_array_(0),
      vertex_buffers_(),
      element_buffer_(0) {
  glGenVertexArrays(1, &vertex_array_);
  glBindVertexArray(vertex_array_);

  vertex_buffers_.resize(GetNumVertexAttributes(geometry_));
  glGenBuffers(static_cast<GLsizei>(vertex_buffers_.size()),
               &vertex_buffers_[0]);

  int attribute_index = 0;

  if (!geometry->positions().empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_[attribute_index]);
    glBufferData(GL_ARRAY_BUFFER,
                 geometry_->positions().size()
                     * sizeof(geometry_->positions()[0]),
                 &geometry_->positions()[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(attribute_index);
    glVertexAttribPointer(attribute_index, 3, GL_FLOAT, GL_FALSE,
                          sizeof(geometry_->positions()[0]),
                          reinterpret_cast<void *>(0));
    ++attribute_index;
  }

  if (!geometry->normals().empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_[attribute_index]);
    glBufferData(GL_ARRAY_BUFFER,
                 geometry_->normals().size()
                     * sizeof(geometry_->normals()[0]),
                 &geometry_->normals()[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(attribute_index);
    glVertexAttribPointer(attribute_index, 3, GL_FLOAT, GL_FALSE,
                          sizeof(geometry_->normals()[0]),
                          reinterpret_cast<void *>(0));
    ++attribute_index;
  }

  if (!geometry->uvs().empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_[attribute_index]);
    glBufferData(GL_ARRAY_BUFFER,
                 geometry_->uvs().size()
                     * sizeof(geometry_->uvs()[0]),
                 &geometry_->uvs()[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(attribute_index);
    glVertexAttribPointer(attribute_index, 2, GL_FLOAT, GL_FALSE,
                          sizeof(geometry_->uvs()[0]),
                          reinterpret_cast<void *>(0));
    ++attribute_index;
  }

  if (!geometry->colors().empty()) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_[attribute_index]);
    glBufferData(GL_ARRAY_BUFFER,
                 geometry_->colors().size()
                     * sizeof(geometry_->colors()[0]),
                 &geometry_->colors()[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(attribute_index);
    glVertexAttribPointer(attribute_index, 3, GL_FLOAT, GL_FALSE,
                          sizeof(geometry_->colors()[0]),
                          reinterpret_cast<void *>(0));
    ++attribute_index;
  }

  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               geometry_->indices().size()
                   * sizeof(geometry_->indices()[0]),
               &geometry_->indices()[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vertex_array_);
  glDeleteBuffers(static_cast<GLsizei>(vertex_buffers_.size()),
                  &vertex_buffers_[0]);
  glDeleteBuffers(1, &element_buffer_);
}

int Mesh::GetNumVertexAttributes(Geometry *geometry) {
  return (geometry->positions().empty() ? 0 : 1)
         + (geometry->normals().empty() ? 0 : 1)
         + (geometry->uvs().empty() ? 0 : 1)
         + (geometry->colors().empty() ? 0 : 1);
}
