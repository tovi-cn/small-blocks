#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "glad/glad.h"
#include "glm/glm.hpp"

class Material {
 public:
  Material();
  ~Material();

  GLuint shader_program() const { return shader_program_; }
  void set_shader_program(GLuint program) { shader_program_ = program; }

  GLuint texture() const { return texture_; }
  void set_texture(GLuint texture) { texture_ = texture; }

 private:
  GLuint shader_program_;
  GLuint texture_;
};

#endif  // MATERIAL_H_
