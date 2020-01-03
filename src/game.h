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
  struct RaycastHit {
    Block *block;
    Block *previous_block;
    glm::vec3 position;
    glm::vec3 previous_position;
  };

  static constexpr float kWorldSize = 10;

  static constexpr int kMaxSizeDimension = -1;
  static constexpr int kMinSizeDimension = 8;
  static constexpr int kDefaultSizeDimension = 0;

  static constexpr int kMaxBlockDimension = 1;
  static constexpr int kMinBlockDimension = 16;
  static constexpr int kDefaultBlockDimension = 4;

  static constexpr int kNoValue = 0x000000;  // Should always equal to 0.
  static constexpr int kColor1 = 0xeeeeee;   // White
  static constexpr int kColor2 = 0xea4611;   // Red
  static constexpr int kColor3 = 0x25e10e;   // Green
  static constexpr int kColor4 = 0x296fff;   // Blue
  static constexpr int kColor5 = 0xf9dd07;   // Yellow
  static constexpr int kDefaultColor = kColor1;

  static constexpr double kBlockInterval = 0.25;

  Game();
  ~Game();

  bool Initialize();
  void Run();

  void GenerateWorld();

  void PlaceBlock();
  void BreakBlock();
  RaycastHit RaycastBlock();
  Block *GetBlockNode(float x, float y, float z, int dimension);
  int GetBlock(float x, float y, float z);
  void SetBlock(float x, float y, float z, int dimension, int value);

  void ShrinkSize();
  void GrowSize();
  void ShrinkBlock();
  void GrowBlock();
  void SetColor(int color);

  void MouseDown(int button);
  void MouseUp(int button);
  void KeyDown(int key);
  void KeyUp(int key);

 private:
  void Update(float delta_time);
  void Render();
  void DrawBlock(Block *block, float x, float y, float z, float size);

  glm::vec3 GetCameraForward() const;
  glm::mat4 GetCameraViewMatrix() const;

  void FocusWindow();
  void UnfocusWindow();

  static void OnMouseButtonEvent(GLFWwindow *window, int button,
                                 int action, int mods);
  static void OnKeyEvent(GLFWwindow *window, int key, int scancode,
                         int action, int mods);
  static void OnGlfwError(int error, const char *description);
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

  glm::vec3 player_position_;
  glm::vec3 player_rotation_;
  int size_dimension_;
  int block_dimension_;
  float speed_;
  int color_;
  bool placing_;
  bool breaking_;
  double last_block_time_;

  Block *world_;

  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  GLuint vertex_array_;
  GLuint vertex_buffer_;
  GLuint element_buffer_;

  GLuint program_;
  GLuint view_projection_location_;
  GLuint model_location_;
  GLuint color_location_;
};

#endif  // GAME_H_
