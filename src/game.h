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

#ifndef GAME_H_
#define GAME_H_

#include <bitset>
#include <string>
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
  struct RayCastHit {
    Block *block;
    int dimension;
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
  void CopyBlock();
  RayCastHit RayCastBlock();
  Block *GetBlock(float x, float y, float z, int *dimension);
  void SetBlock(float x, float y, float z, int dimension, int value);

  void ShrinkSize();
  void GrowSize();
  void ShrinkBlock();
  void GrowBlock();
  void SetColor(int color);

  void MouseDown(int button);
  void MouseUp(int button);
  void Scroll(float offset);
  void KeyDown(int key);
  void KeyUp(int key);

 private:
  GLuint CreateShader(const char *text, GLenum type);
  GLuint CreateShaderProgram(const char *vertex_shader_text,
                             const char *fragment_shader_text);

  void Update(float delta_time);
  void Render();
  void DrawBlock(Block *block, float x, float y, float z, float size);
  void DrawHighlight();
  void DrawCrosshair();

  glm::vec3 GetCameraForward() const;
  glm::mat4 GetCameraViewMatrix() const;

  void FocusWindow();
  void UnfocusWindow();

  static void OnMouseButtonEvent(GLFWwindow *window, int button,
                                 int action, int mods);
  static void OnScrollEvent(GLFWwindow *window, double x_offset,
                            double y_offset);
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
  double block_interval_;
  double last_block_time_;
  RayCastHit ray_cast_hit_;

  Block *world_;

  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  GLuint vertex_array_;
  GLuint vertex_buffer_;
  GLuint element_buffer_;

  std::vector<Vertex> highlight_vertices_;
  std::vector<unsigned int> highlight_indices_;
  GLuint highlight_vertex_array_;
  GLuint highlight_vertex_buffer_;
  GLuint highlight_element_buffer_;

  std::vector<CrosshairVertex> crosshair_vertices_;
  std::vector<unsigned int> crosshair_indices_;
  GLuint crosshair_vertex_array_;
  GLuint crosshair_vertex_buffer_;
  GLuint crosshair_element_buffer_;

  GLuint texture_;
  GLuint highlight_texture_;
  GLuint crosshair_texture_;

  GLuint shader_program_;
  GLuint crosshair_shader_program_;
};

#endif  // GAME_H_
