#include "input_system.h"

#include <algorithm>

InputSystem::InputSystem(Window *window)
  : window_(window),
    pressed_mouse_buttons_(), pressed_keys_(),
    listeners_() {}

InputSystem::~InputSystem() {
  GLFWwindow *window_glfw = window_->window_glfw();

  glfwSetWindowUserPointer(window_glfw, nullptr);

  glfwSetMouseButtonCallback(window_glfw, nullptr);
  glfwSetScrollCallback(window_glfw, nullptr);
  glfwSetKeyCallback(window_glfw, nullptr);
}

void InputSystem::Initialize() {
  GLFWwindow *window_glfw = window_->window_glfw();

  glfwSetWindowUserPointer(window_glfw, this);

  glfwSetMouseButtonCallback(window_glfw, OnMouseButtonEvent);
  glfwSetScrollCallback(window_glfw, OnScrollEvent);
  glfwSetKeyCallback(window_glfw, OnKeyEvent);
}

void InputSystem::PollEvents() {
  glfwPollEvents();
}

void InputSystem::AddListener(InputListener *listener) {
  listeners_.push_back(listener);
}

void InputSystem::RemoveListener(InputListener *listener) {
  auto position = std::find(listeners_.begin(), listeners_.end(), listener);
  if (position != listeners_.end()) {
    listeners_.erase(position);
  }
}

double InputSystem::GetTime() const {
  return glfwGetTime();
}

void InputSystem::MouseDown(int button) {
  pressed_mouse_buttons_.set(button);

  for (auto listener : listeners_) {
    listener->MouseDown(button);
  }
}

void InputSystem::MouseUp(int button) {
  pressed_mouse_buttons_.reset(button);

  for (auto listener : listeners_) {
    listener->MouseUp(button);
  }
}

void InputSystem::Scroll(float offset) {
  for (auto listener : listeners_) {
    listener->Scroll(offset);
  }
}

void InputSystem::KeyDown(int key) {
  if (key < 0) {
    return;
  }
  pressed_keys_.set(key);

  for (auto listener : listeners_) {
    listener->KeyDown(key);
  }
}

void InputSystem::KeyUp(int key) {
  if (key < 0) {
    return;
  }
  pressed_keys_.reset(key);

  for (auto listener : listeners_) {
    listener->KeyUp(key);
  }
}

void InputSystem::OnMouseButtonEvent(GLFWwindow *window, int button,
                                     int action, int mods) {
  (void)mods;

  InputSystem *input =
      static_cast<InputSystem *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    input->MouseDown(button);
  } else if (action == GLFW_RELEASE) {
    input->MouseUp(button);
  }
}

void InputSystem::OnScrollEvent(GLFWwindow *window, double x_offset,
                                double y_offset) {
  InputSystem *input =
      static_cast<InputSystem *>(glfwGetWindowUserPointer(window));
  input->Scroll(y_offset);
}

void InputSystem::OnKeyEvent(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
  (void)scancode;
  (void)mods;

  InputSystem *input =
      static_cast<InputSystem *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    input->KeyDown(key);
  } else if (action == GLFW_RELEASE) {
    input->KeyUp(key);
  }
}
