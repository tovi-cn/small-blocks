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

#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

class Window {
 public:
  Window();
  ~Window();

  bool Initialize(const std::string &title, glm::ivec2 size);

  glm::ivec2 size() const {
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);
    return glm::ivec2(width, height);
  }

  void Maximize();
  void SetCursorEnabled(bool enabled);

  GLFWwindow *window_glfw() const { return window_; }

 private:
  static void OnGlfwError(int error, const char *description);

  GLFWwindow *window_;
};

#endif  // WINDOW_H_
