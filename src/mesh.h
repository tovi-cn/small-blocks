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
