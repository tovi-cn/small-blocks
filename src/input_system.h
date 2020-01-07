#ifndef INPUT_SYSTEM_H_
#define INPUT_SYSTEM_H_

#include <bitset>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

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
  void AddListener(InputListener *listener);
  void RemoveListener(InputListener *listener);

  bool mouse_button_is_pressed(int button) {
    return pressed_mouse_buttons_.test(button);
  }
  bool key_is_pressed(int key) {
    return pressed_keys_.test(key);
  }

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
  std::bitset<32> pressed_mouse_buttons_;
  std::bitset<1024> pressed_keys_;
  std::vector<InputListener *> listeners_;
};

#endif  // INPUT_SYSTEM_H_
