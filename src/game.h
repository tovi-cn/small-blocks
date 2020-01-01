#ifndef GAME_H_
#define GAME_H_

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class Game {
 public:
  Game();
  ~Game();

  bool Initialize();
  void Run();

 private:
  static void OnGlfwError(int error, const char* description);
  static void GLAPIENTRY OnGlError(GLenum source,
                                   GLenum type,
                                   GLuint id,
                                   GLenum severity,
                                   GLsizei length,
                                   const GLchar *message,
                                   const void *user_param);
  static void OnKeyPress(GLFWwindow *window, int key, int scancode,
                         int action, int mods);

  GLFWwindow *window_;

  GLuint vertex_array_;
  GLuint vertex_buffer_;
  GLuint program_;
  GLuint mvp_location_;
};

#endif  // GAME_H_
