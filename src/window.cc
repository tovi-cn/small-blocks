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

#include "window.h"

#include <iostream>

Window::Window() : window_(nullptr) {}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Window::Initialize(const std::string &title, glm::ivec2 size) {
  glfwSetErrorCallback(OnGlfwError);
  if (!glfwInit()) {
    std::cerr << "glfwInit() failed\n";
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = glfwCreateWindow(size.x, size.y,
                             title.c_str(),
                             nullptr, nullptr);
  if (!window_) {
    std::cerr << "glfwCreateWindow() failed\n";
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window_);

  glfwSwapInterval(0);

  return true;
}

void Window::Maximize() {
  glfwMaximizeWindow(window_);
}

void Window::SetCursorEnabled(bool enabled) {
  glfwSetInputMode(window_, GLFW_CURSOR,
                   enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION,
                     enabled ? GLFW_TRUE : GLFW_FALSE);
  }
}

void Window::OnGlfwError(int error, const char *description) {
  (void)error;
  std::cerr << "Error: " << description << "\n";
}
