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

#ifndef INPUT_H_
#define INPUT_H_
#include <bitset>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "key_code.h"
#include "window.h"
#include <stdio.h>

class InputListener {
 public:
  virtual void MouseDown(int button) {}
  virtual void MouseUp(int button) {}
  virtual void Scroll(float offset) {}
  virtual void KeyDown(int key) {}
  virtual void KeyUp(int key) {}
};

class InputSystem {
 public:
  InputSystem(Window *window);
  ~InputSystem();

  void Initialize();
  void PollEvents();
  void AddListener(InputListener *listener);
  void RemoveListener(InputListener *listener);

  glm::vec2 GetMousePosition() const {
    double x;
    double y;
    glfwGetCursorPos(window_->window_glfw(), &x, &y);
    return glm::vec2(x, y);
  }

  bool MouseButtonIsPressed(int button) const {
    return pressed_mouse_buttons_.test(button);
  }
  bool KeyIsPressed(int key) const {
    return pressed_keys_.test(key);
  }

  bool ExitIsRequested() const {
    return glfwWindowShouldClose(window_->window_glfw());
  }

  double GetTime() const;

  void MouseDown(int button);
  void MouseUp(int button);
  void Scroll(float offset);
  void KeyDown(int key);
  void KeyUp(int key);

 private:
  static void OnMouseButtonEvent(GLFWwindow *window, int button,
                                 int action, int mods);
  static void OnScrollEvent(GLFWwindow *window, double x_offset,
                            double y_offset);
  static void OnKeyEvent(GLFWwindow *window, int key, int scancode,
                         int action, int mods);

  Window *window_;
  std::bitset<NUM_MOUSE_BUTTONS> pressed_mouse_buttons_;
  std::bitset<NUM_KEYS> pressed_keys_;
  std::vector<InputListener *> listeners_;
};

#endif  // INPUT_H_
