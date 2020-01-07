#include "window.h"

#include <iostream>

Window::Window() : window_(nullptr) {}

Window::~Window() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Window::Initialize(const std::string &title) {
  glfwSetErrorCallback(OnGlfwError);
  if (!glfwInit()) {
    std::cerr << "glfwInit() failed\n";
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = glfwCreateWindow(800, 600,
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

