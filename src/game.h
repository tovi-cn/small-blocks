#ifndef GAME_H_
#define GAME_H_

#include <bitset>
#include <vector>

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"

#include "block.h"
#include "geometry.h"

class Game {
 public:
  static constexpr float kWorldSize = 10;
  static constexpr float kDefaultSpeed = 0.5f;

  Game();
  ~Game();

  bool Initialize();
  void Run();

  void SetBlock(float x, float y, float z, int dimension, int value);
  void Shrink();
  void Grow();

  void MouseDown(int button);
  void MouseUp(int button);
  void KeyDown(int key);
  void KeyUp(int key);

 private:
  void Update(float delta_time);
  void Render();
  void DrawBlock(Block *block, float x, float y, float z, float size);

  void FocusWindow();
  void UnfocusWindow();

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
  bool window_focused_;
  std::bitset<32> pressed_mouse_buttons_;
  std::bitset<1024> pressed_keys_;
  double mouse_last_x_;
  double mouse_last_y_;

  float wireframe_;
  glm::vec3 camera_position_;
  glm::vec3 camera_rotation_;

  float speed_;

  Block *world_;

  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  GLuint vertex_array_;
  GLuint vertex_buffer_;
  GLuint element_buffer_;

  GLuint program_;
  GLuint view_projection_location_;
  GLuint model_location_;
};

#endif  // GAME_H_
