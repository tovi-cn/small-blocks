#ifndef GAME_H_
#define GAME_H_

#include <bitset>

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"

class Game {
 public:
  Game();
  ~Game();

  bool Initialize();
  void Run();

  void MouseDown(int button);
  void MouseUp(int button);
  void KeyDown(int key);
  void KeyUp(int key);

 private:
  static void OnMouseButtonEvent(GLFWwindow *window, int button,
                                 int action, int mods);
  static void OnKeyEvent(GLFWwindow *window, int key, int scancode,
                         int action, int mods);
  static void OnGlfwError(int error, const char* description);
  static void GLAPIENTRY OnGlError(GLenum source,
                                   GLenum type,
                                   GLuint id,
                                   GLenum severity,
                                   GLsizei length,
                                   const GLchar *message,
                                   const void *user_param);

  GLFWwindow *window_;
  std::bitset<32> pressed_mouse_buttons_;
  std::bitset<1024> pressed_keys_;

  GLuint vertex_array_;
  GLuint vertex_buffer_;
  GLuint program_;
  GLuint mvp_location_;

  glm::vec3 camera_position_;
  glm::vec3 camera_rotation_;
  double mouse_last_x_;
  double mouse_last_y_;
};

#endif  // GAME_H_
