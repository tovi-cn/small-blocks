#ifndef INPUT_SYSTEM_H_
#define INPUT_SYSTEM_H_

#include <bitset>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "key_code.h"
#include "window.h"

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

  glm::vec2 mouse_position() const {
    double x;
    double y;
    glfwGetCursorPos(window_->window_glfw(), &x, &y);
    return glm::vec2(x, y);
  }

  bool is_mouse_button_pressed(int button) const {
    return pressed_mouse_buttons_.test(button);
  }
  bool is_key_pressed(int key) const {
    return pressed_keys_.test(key);
  }

  bool is_exit_requested() const {
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

#endif  // INPUT_SYSTEM_H_
