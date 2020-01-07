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

  bool Initialize(const std::string &title);

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
