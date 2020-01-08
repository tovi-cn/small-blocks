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

#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "block.h"
#include "geometry.h"
#include "input_system.h"
#include "material.h"
#include "renderer.h"
#include "window.h"

class Game : public InputListener {
 public:
  struct RayCastHit {
    Block *block;
    int dimension;
    glm::vec3 position;
    glm::vec3 previous_position;
  };

  Game(Window *window, Renderer *renderer, InputSystem *input);
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

  void FocusWindow();
  void UnfocusWindow();

  virtual void MouseDown(int button);
  virtual void MouseUp(int button);
  virtual void Scroll(float offset);
  virtual void KeyDown(int key);
  virtual void KeyUp(int key);

 private:
  void LoadAssets();

  void Update(float delta_time);
  void Render();
  void DrawBlock(Block *block, float x, float y, float z, float size);
  void DrawHighlight();
  void DrawCrosshair();

  Window *window_;
  Renderer *renderer_;
  InputSystem *input_;

  bool window_focused_;
  glm::vec2 mouse_last_position_;

  float wireframe_;

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

  GLuint block_texture_;
  GLuint highlight_texture_;
  GLuint crosshair_texture_;

  GLuint block_shader_program_;
  GLuint highlight_shader_program_;
  GLuint crosshair_shader_program_;

  Geometry block_geometry_;
  Material block_material_;
  Mesh *block_mesh_;

  Geometry highlight_geometry_;
  Material highlight_material_;
  Mesh *highlight_mesh_;

  Geometry crosshair_geometry_;
  Material crosshair_material_;
  Mesh *crosshair_mesh_;
};

#endif  // GAME_H_
