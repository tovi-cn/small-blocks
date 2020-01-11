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
