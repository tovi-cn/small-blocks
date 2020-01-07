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

#ifndef RENDERER_H_
#define RENDERER_H_

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "window.h"

class Renderer {
 public:
  Renderer(Window *window);
  ~Renderer();

  bool Initialize();

  GLuint CreateShaderProgram(const std::string &vertex_shader_text,
                             const std::string &fragment_shader_text);
  GLuint CreateShader(const std::string &text, GLenum type);

  void SwapBuffers();

 private:
  static void GLAPIENTRY OnGlError(
      GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
      const GLchar *message, const void *user_param);

  Window *window_;
};

#endif  // RENDERER_H_
