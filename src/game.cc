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

#include "game.h"

#include <iostream>
#include <vector>
#include <ctime>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "stb_image.h"

#include "fractals.h"
#include "utilities.h"

static const float kWorldSize = 10;

static const int kMaxSizeDimension = -1;
static const int kMinSizeDimension = 8;
static const int kDefaultSizeDimension = 0;

static const int kMaxBlockDimension = 1;
static const int kMinBlockDimension = 16;
static const int kDefaultBlockDimension = 4;

static const int kNoValue = 0x000000;  // Should always equal to 0.
static const int kColor1 = 0xeeeeee;   // White
static const int kColor2 = 0xea4611;   // Red
static const int kColor3 = 0x25e10e;   // Green
static const int kColor4 = 0x296fff;   // Blue
static const int kColor5 = 0xf9dd07;   // Yellow
static const int kDefaultColor = kColor1;

static const double kBlockInterval = 0.25;

static const char *kVertexShaderText =
"#version 330 core\n"
"uniform mat4 uViewProjection;\n"
"uniform mat4 uModel;\n"
"uniform vec3 uColor;\n"
"layout (location = 0) in vec3 vPos;\n"
"layout (location = 1) in vec3 vNormal;\n"
"layout (location = 2) in vec3 vColor;\n"
"layout (location = 3) in vec2 vTexCoord;\n"
"out vec3 color;\n"
"out vec2 texCoord;\n"
"void main() {\n"
"  gl_Position = uViewProjection * uModel * vec4(vPos, 1.0);\n"
"  color = uColor;\n"
"  if (vNormal.x == 1 || vNormal.x == -1) {\n"
"    color *= .65;\n"
"  }\n"
"  if (vNormal.y == 1) {\n"
"    color *= 1;\n"
"  }\n"
"  if (vNormal.y == -1) {\n"
"    color *= .25;\n"
"  }\n"
"  if (vNormal.z == 1 || vNormal.z == -1) {\n"
"    color *= .5;\n"
"  }\n"
"  // TODO: color = vColor;\n"
"  texCoord = vTexCoord;\n"
"}\n";

static const char *kFragmentShaderText =
"#version 330 core\n"
"uniform sampler2D uTexture;\n"
"in vec3 color;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"  FragColor = texture(uTexture, texCoord) * vec4(color, 1.0);\n"
"}\n";

static const char *kCrosshairVertexShaderText =
"#version 330 core\n"
"uniform mat4 uModel;\n"
"layout (location = 0) in vec3 vPos;\n"
"layout (location = 1) in vec2 vTexCoord;\n"
"out vec2 texCoord;\n"
"void main() {\n"
"  gl_Position = uModel * vec4(vPos, 1.0);\n"
"  texCoord = vTexCoord;\n"
"}\n";

static const char *kCrosshairFragmentShaderText =
"#version 330 core\n"
"uniform sampler2D uTexture;\n"
"in vec2 texCoord;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"  FragColor = texture(uTexture, texCoord);\n"
"}\n";

Game::Game(Window *window, Renderer *renderer, InputSystem *input)
    : window_(window), renderer_(renderer), input_(input),
      exit_requested_(false),
      window_focused_(false),
      mouse_last_position_(0.0),
      wireframe_(false),
      camera_position_(0.0f), camera_rotation_(0.0f),
      player_position_(0.0f), player_rotation_(0.0f),
      size_dimension_(kDefaultSizeDimension),
      block_dimension_(kDefaultBlockDimension),
      speed_(0),
      color_(kDefaultColor),
      breaking_(false),
      placing_(false),
      block_interval_(kBlockInterval), last_block_time_(0),
      ray_cast_hit_(),
      world_() {}

Game::~Game() {
  // Deallocate OpenGL objects
  glDeleteVertexArrays(1, &vertex_array_);
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &element_buffer_);

  glDeleteVertexArrays(1, &highlight_vertex_array_);
  glDeleteBuffers(1, &highlight_vertex_buffer_);
  glDeleteBuffers(1, &highlight_element_buffer_);

  glDeleteVertexArrays(1, &crosshair_vertex_array_);
  glDeleteBuffers(1, &crosshair_vertex_buffer_);
  glDeleteBuffers(1, &crosshair_element_buffer_);

  glDeleteTextures(1, &texture_);
  glDeleteTextures(1, &highlight_texture_);
  glDeleteTextures(1, &crosshair_texture_);

  glDeleteProgram(shader_program_);
  glDeleteProgram(crosshair_shader_program_);

  delete world_;
}

bool Game::Initialize() {
  input_->AddListener(this);

  window_->Maximize();
  FocusWindow();

  // Generate block geometry

  vertices_ = kCubeVertices;
  indices_ = kCubeIndices;

  glGenVertexArrays(1, &vertex_array_);
  glBindVertexArray(vertex_array_);

  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(vertices_[0]),
               &vertices_[0], GL_STATIC_DRAW);

  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(indices_[0]),
               &indices_[0], GL_STATIC_DRAW);

  // Vertex attributes
  // (Note that these can be toggled and replaced e.g. with glVertexAttrib3f)

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices_[0]),
                        reinterpret_cast<void *>(0));
  // Normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices_[0]),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  // Color
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices_[0]),
                        reinterpret_cast<void *>(6 * sizeof(float)));
  // Texture coordinate
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertices_[0]),
                        reinterpret_cast<void *>(9 * sizeof(float)));

  glBindVertexArray(0);

  // Generate highlight geometry

  highlight_vertices_ = kHighlightVertices;
  highlight_indices_ = kHighlightIndices;

  glGenVertexArrays(1, &highlight_vertex_array_);
  glBindVertexArray(highlight_vertex_array_);

  glGenBuffers(1, &highlight_vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, highlight_vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, highlight_vertices_.size() * sizeof(highlight_vertices_[0]),
               &highlight_vertices_[0], GL_STATIC_DRAW);

  glGenBuffers(1, &highlight_element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, highlight_element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, highlight_indices_.size() * sizeof(highlight_indices_[0]),
               &highlight_indices_[0], GL_STATIC_DRAW);

  // Vertex attributes
  // (Note that these can be toggled and replaced e.g. with glVertexAttrib3f)

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(highlight_vertices_[0]),
                        reinterpret_cast<void *>(0));
  // Normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(highlight_vertices_[0]),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  // Color
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                        sizeof(highlight_vertices_[0]),
                        reinterpret_cast<void *>(6 * sizeof(float)));
  // Texture coordinate
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
                        sizeof(highlight_vertices_[0]),
                        reinterpret_cast<void *>(9 * sizeof(float)));

  glBindVertexArray(0);

  // Generate crosshair geometry

  crosshair_vertices_ = kCrosshairVertices;
  crosshair_indices_ = kCrosshairIndices;

  glGenVertexArrays(1, &crosshair_vertex_array_);
  glBindVertexArray(crosshair_vertex_array_);

  glGenBuffers(1, &crosshair_vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, crosshair_vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, crosshair_vertices_.size() * sizeof(crosshair_vertices_[0]),
               &crosshair_vertices_[0], GL_STATIC_DRAW);

  glGenBuffers(1, &crosshair_element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crosshair_element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, crosshair_indices_.size() * sizeof(crosshair_indices_[0]),
               &crosshair_indices_[0], GL_STATIC_DRAW);

  // Vertex attributes
  // (Note that these can be toggled and replaced e.g. with glVertexAttrib3f)

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                        sizeof(crosshair_vertices_[0]),
                        reinterpret_cast<void *>(0));
  // Texture coordinate
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                        sizeof(crosshair_vertices_[0]),
                        reinterpret_cast<void *>(2 * sizeof(float)));

  glBindVertexArray(0);

  // Create world

  GenerateWorld();

  // Player

  player_position_ =
      glm::vec3(kWorldSize / 2.0f, kWorldSize / 2.0f, kWorldSize / 2.0f);

  return true;
}

void Game::LoadAssets() {
  // Load block texture

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int image_width;
  int image_height;

  stbi_set_flip_vertically_on_load(true);
  unsigned char *image_data =
      stbi_load("assets/block.png", &image_width, &image_height, nullptr, 3);
  stbi_set_flip_vertically_on_load(false);
  if (image_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
  } else {
    std::cerr << "Failed to load texture.\n";
  }

  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  // Load highlight texture

  glGenTextures(1, &highlight_texture_);
  glBindTexture(GL_TEXTURE_2D, highlight_texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(true);
  image_data =
      stbi_load("assets/highlight.png", &image_width, &image_height, nullptr, 4);
  stbi_set_flip_vertically_on_load(false);
  if (image_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
  } else {
    std::cerr << "Failed to load texture.\n";
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  // Load crosshair texture

  glGenTextures(1, &crosshair_texture_);
  glBindTexture(GL_TEXTURE_2D, crosshair_texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(true);
  image_data =
      stbi_load("assets/crosshair.png", &image_width, &image_height, nullptr, 4);
  stbi_set_flip_vertically_on_load(false);
  if (image_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
  } else {
    std::cerr << "Failed to load texture.\n";
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  // Load shaders

  shader_program_ =
      renderer_->CreateShaderProgram(kVertexShaderText, kFragmentShaderText);
  crosshair_shader_program_ =
      renderer_->CreateShaderProgram(kCrosshairVertexShaderText,
                                     kCrosshairFragmentShaderText);
}

void Game::Run() {
  mouse_last_position_ = input_->mouse_position();
  double last_time = input_->GetTime();

  while (!exit_requested_) {
    double current_time = input_->GetTime();
    float delta_time = static_cast<float>(current_time - last_time);
    last_time = current_time;

    input_->PollEvents();

    Update(delta_time);
    Render();
  }
}

void Game::Update(float delta_time) {
  if (input_->is_exit_requested()) {
    exit_requested_ = true;
    return;
  }

  double mouse_x;
  double mouse_y;
  glm::vec2 mouse_position = input_->mouse_position();
  glm::vec2 mouse_delta = mouse_position - mouse_last_position_;
  mouse_last_position_ = mouse_position;

  speed_ = glm::pow(2.0f, -size_dimension_);

  if (window_focused_) {
    float mouse_sensitivity = 0.005f;
    player_rotation_.x += -mouse_delta.y * mouse_sensitivity;
    player_rotation_.y += -mouse_delta.x * mouse_sensitivity;
    player_rotation_.x = glm::clamp(player_rotation_.x, glm::radians(-89.99f),
                                    glm::radians(89.99f));

    glm::vec3 forward = GetCameraForward();
    forward.y = 0.0f;
    forward = glm::normalize(forward);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    glm::vec3 direction(0.0f);
    if (input_->is_key_pressed(KEY_W)) {
      direction += forward;
    }
    if (input_->is_key_pressed(KEY_S)) {
      direction -= forward;
    }
    if (input_->is_key_pressed(KEY_A)) {
      direction -= right;
    }
    if (input_->is_key_pressed(KEY_D)) {
      direction += right;
    }
    if (direction != glm::vec3(0.0f)) {
      player_position_ += glm::normalize(direction) * speed_ * delta_time;
    }

    if (input_->is_key_pressed(KEY_SPACE)) {
      player_position_ += up * speed_ * delta_time;
    }
    if (input_->is_key_pressed(KEY_LEFT_SHIFT)) {
      player_position_ -= up * speed_ * delta_time;
    }
  }

  player_position_.x =
      glm::clamp(player_position_.x, -kWorldSize, 2 * kWorldSize);
  player_position_.y =
      glm::clamp(player_position_.y, -kWorldSize, 2 * kWorldSize);
  player_position_.z =
      glm::clamp(player_position_.z, -kWorldSize, 2 * kWorldSize);

  camera_position_ = player_position_;
  // TODO: float player_height = glm::pow(2.0f, -size_dimension_);
  float player_height = 1.0f;
  camera_position_.y += player_height;
  camera_rotation_ = player_rotation_;

  double time = input_->GetTime();
  if (placing_ && time - last_block_time_ >= block_interval_) {
    PlaceBlock();
    last_block_time_ = time;
  }
  if (breaking_ && time - last_block_time_ >= block_interval_) {
    BreakBlock();
    last_block_time_ = time;
  }

  ray_cast_hit_ = RayCastBlock();
}

void Game::GenerateWorld() {
  delete world_;
  world_ = new Block();
  world_->set_child(4, new Block(kColor3));
  world_->set_child(5, new Block(kColor2));
  world_->set_child(6, new Block(kColor4));
  world_->set_child(7, new Block(kColor5));
}

void Game::PlaceBlock() {
  RayCastHit hit = RayCastBlock();
  if (hit.block && hit.dimension <= block_dimension_) {
    SetBlock(hit.previous_position.x, hit.previous_position.y,
             hit.previous_position.z, block_dimension_, color_);
  }
}

void Game::BreakBlock() {
  RayCastHit hit = RayCastBlock();
  if (hit.block) {
    SetBlock(hit.position.x, hit.position.y, hit.position.z,
             block_dimension_, kNoValue);
  }
}

void Game::CopyBlock() {
  RayCastHit hit = RayCastBlock();
  if (hit.block) {
    color_ = hit.block->value();
  }
}

Game::RayCastHit Game::RayCastBlock() {
  RayCastHit hit;
  hit.block = nullptr;
  hit.dimension = 0;
  hit.position = camera_position_;
  hit.previous_position = hit.position;
  glm::vec3 direction = GetCameraForward();

  int num_steps = 10000;
  for (int i = 0; i < num_steps; ++i) {
    int dimension;
    Block *block = GetBlock(hit.position.x, hit.position.y, hit.position.z,
                            &dimension);
    if (block && (!block->is_leaf() || block->value() != kNoValue)) {
      hit.block = block;
      hit.dimension = dimension;

      float block_size = kWorldSize * glm::pow(2.0f, -block_dimension_);

      hit.position.x = FloorNearestMultiple(hit.position.x, block_size);
      hit.position.y = FloorNearestMultiple(hit.position.y, block_size);
      hit.position.z = FloorNearestMultiple(hit.position.z, block_size);

      hit.previous_position.x =
          FloorNearestMultiple(hit.previous_position.x, block_size);
      hit.previous_position.y =
          FloorNearestMultiple(hit.previous_position.y, block_size);
      hit.previous_position.z =
          FloorNearestMultiple(hit.previous_position.z, block_size);

      return hit;
    }

    hit.previous_position = hit.position;
    float step_size =
        0.05f * glm::pow(2.0f, static_cast<float>(-block_dimension_));
    hit.position += direction * step_size;
  }

  return hit;
}

Block *Game::GetBlock(float x, float y, float z, int *dimension) {
  if (x < 0.0f || y < 0.0f || z < 0.0f ||
      x >= kWorldSize || y >= kWorldSize || z >= kWorldSize) {
    return nullptr;
  }

  Block *block = world_;
  *dimension = 0;
  float size = kWorldSize;
  float dx = 0.0f;
  float dy = 0.0f;
  float dz = 0.0f;

  for (int i = 0; ; ++i) {
    if (block->is_leaf()) {
      return block;
    }

    ++(*dimension);
    size /= 2.0f;
    float center_x = size + dx;
    float center_y = size + dy;
    float center_z = size + dz;

    int index = 0;
    if        (x  < center_x && y >= center_y && z >= center_z) {
      index = 0;
      dy += size;
      dz += size;
    } else if (x >= center_x && y >= center_y && z >= center_z) {
      index = 1;
      dx += size;
      dy += size;
      dz += size;
    } else if (x  < center_x && y >= center_y && z  < center_z) {
      index = 2;
      dy += size;
    } else if (x >= center_x && y >= center_y && z  < center_z) {
      index = 3;
      dx += size;
      dy += size;
    } else if (x  < center_x && y  < center_y && z >= center_z) {
      index = 4;
      dz += size;
    } else if (x >= center_x && y  < center_y && z >= center_z) {
      index = 5;
      dx += size;
      dz += size;
    } else if (x  < center_x && y  < center_y && z  < center_z) {
      index = 6;
    } else if (x >= center_x && y  < center_y && z  < center_z) {
      index = 7;
      dx += size;
    }

    Block *child = block->child(index);
    if (!child) {
      return nullptr;
    }
    block = child;
  }
}

void Game::SetBlock(float x, float y, float z, int dimension, int value) {
  if (x < 0.0f || y < 0.0f || z < 0.0f ||
      x >= kWorldSize || y >= kWorldSize || z >= kWorldSize) {
    return;
  }

  Block *block = world_;
  float size = kWorldSize;
  float dx = 0.0f;
  float dy = 0.0f;
  float dz = 0.0f;

  for (int i = 0; i < dimension; ++i) {
    size /= 2.0f;
    float center_x = size + dx;
    float center_y = size + dy;
    float center_z = size + dz;

    int index = 0;
    if        (x  < center_x && y >= center_y && z >= center_z) {
      index = 0;
      dy += size;
      dz += size;
    } else if (x >= center_x && y >= center_y && z >= center_z) {
      index = 1;
      dx += size;
      dy += size;
      dz += size;
    } else if (x  < center_x && y >= center_y && z  < center_z) {
      index = 2;
      dy += size;
    } else if (x >= center_x && y >= center_y && z  < center_z) {
      index = 3;
      dx += size;
      dy += size;
    } else if (x  < center_x && y  < center_y && z >= center_z) {
      index = 4;
      dz += size;
    } else if (x >= center_x && y  < center_y && z >= center_z) {
      index = 5;
      dx += size;
      dz += size;
    } else if (x  < center_x && y  < center_y && z  < center_z) {
      index = 6;
    } else if (x >= center_x && y  < center_y && z  < center_z) {
      index = 7;
      dx += size;
    }

    Block *child = block->child(index);
    if (!child) {
      child = new Block(block->value());
      block->set_child(index, child);
    }
    block = child;
  }

  block->set_value(value);
  world_->Simplify();
}

void Game::ShrinkSize() {
  ++size_dimension_;
  size_dimension_ = glm::min(size_dimension_, kMinSizeDimension);
}

void Game::GrowSize() {
  --size_dimension_;
  size_dimension_ = glm::max(size_dimension_, kMaxSizeDimension);
}

void Game::ShrinkBlock() {
  ++block_dimension_;
  block_dimension_ = glm::min(block_dimension_, kMinBlockDimension);
}

void Game::GrowBlock() {
  --block_dimension_;
  block_dimension_ = glm::max(block_dimension_, kMaxBlockDimension);
}

void Game::SetColor(int color) {
  color_ = color;
}

void Game::FocusWindow() {
  window_focused_ = true;
  window_->SetCursorEnabled(false);
}

void Game::UnfocusWindow() {
  window_focused_ = false;
  window_->SetCursorEnabled(true);
}

void Game::Render() {
  glm::ivec2 window_size = window_->size();
  glViewport(0, 0, window_size.x, window_size.y);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float fov = 90.0f;
  float aspect = static_cast<float>(window_size.x) / window_size.y;
  float near = 0.01f;
  float far = 500.0f;
  glm::mat4 projection_matrix =
      glm::perspective(glm::radians(fov), aspect, near, far);

  glm::mat4 view_matrix = GetCameraViewMatrix();

  glm::mat4 view_projection_matrix = projection_matrix * view_matrix;

  glUseProgram(shader_program_);
  glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uViewProjection"), 1, GL_FALSE,
                     static_cast<const GLfloat *>(&view_projection_matrix[0][0]));
  glUseProgram(0);

  if (wireframe_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  DrawBlock(world_, 0.0f, 0.0f, 0.0f, kWorldSize);
  if (wireframe_) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  DrawHighlight();

  DrawCrosshair();

  renderer_->SwapBuffers();
}

void Game::DrawBlock(Block *block, float x, float y, float z, float size) {
  if (!block) {
    return;
  }

  if (block->value() != kNoValue) {
    glm::mat4 model_matrix(1.0f);
    model_matrix = glm::scale(glm::vec3(size)) * model_matrix;
    model_matrix = glm::translate(glm::vec3(x, y, z)) * model_matrix;

    glUseProgram(shader_program_);
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uModel"), 1, GL_FALSE,
                       static_cast<const GLfloat *>(&model_matrix[0][0]));
    glUseProgram(0);

    glUseProgram(shader_program_);
    float r = static_cast<float>((block->value() >> 16) & 0xff) / 0xff;
    float g = static_cast<float>((block->value() >> 8) & 0xff) / 0xff;
    float b = static_cast<float>(block->value() & 0xff) / 0xff;
    glUniform3f(glGetUniformLocation(shader_program_, "uColor"), r, g, b);
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glUseProgram(shader_program_);
    glUniform1i(glGetUniformLocation(shader_program_, "uTexture"), 0);
    glUseProgram(0);

    glUseProgram(shader_program_);
    glBindVertexArray(vertex_array_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()),
                   GL_UNSIGNED_INT, reinterpret_cast<void *>(0));
    glBindVertexArray(0);
    glUseProgram(0);
  }

  if (!block->is_leaf()) {
    size /= 2;
    DrawBlock(block->child(0), x       , y + size   , z + size, size);
    DrawBlock(block->child(1), x + size, y + size   , z + size, size);
    DrawBlock(block->child(2), x       , y + size   , z       , size);
    DrawBlock(block->child(3), x + size, y + size   , z       , size);

    DrawBlock(block->child(4), x       , y          , z + size, size);
    DrawBlock(block->child(5), x + size, y          , z + size, size);
    DrawBlock(block->child(6), x       , y          , z       , size);
    DrawBlock(block->child(7), x + size, y          , z       , size);
  }
}

void Game::DrawHighlight() {
  if (!ray_cast_hit_.block) {
    return;
  }

  glm::mat4 model_matrix(1.0f);
  float size = kWorldSize
      * glm::pow(2.0f, static_cast<float>(-block_dimension_));
  float extra = size * 0.015f;
  model_matrix = glm::scale(glm::vec3(size + extra)) * model_matrix;
  glm::vec3 position = ray_cast_hit_.position;
  position.x -= extra / 2.0f;
  position.y -= extra / 2.0f;
  position.z -= extra / 2.0f;
  model_matrix = glm::translate(position) * model_matrix;

  glUseProgram(shader_program_);
  glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uModel"), 1, GL_FALSE,
                     static_cast<const GLfloat *>(&model_matrix[0][0]));
  glUseProgram(0);

  glUseProgram(shader_program_);
  glUniform3f(glGetUniformLocation(shader_program_, "uColor"), 0.07f, 0.07f, 0.07f);  // Dark gray
  glUseProgram(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, highlight_texture_);
  glUseProgram(shader_program_);
  glUniform1i(glGetUniformLocation(shader_program_, "uTexture"), 0);
  glUseProgram(0);

  glUseProgram(shader_program_);
  glBindVertexArray(highlight_vertex_array_);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(highlight_indices_.size()),
                 GL_UNSIGNED_INT, reinterpret_cast<void *>(0));
  glBindVertexArray(0);
  glUseProgram(0);
}

void Game::DrawCrosshair() {
  glm::ivec2 window_size = window_->size();

  glm::mat4 model_matrix(1.0f);

  float size = 0.025f * window_size.y;
  model_matrix = glm::scale(glm::vec3(size)) * model_matrix;

  glm::vec2 center(window_size.x / 2.0f - size / 2.0f,
                   window_size.y / 2.0f - size / 2.0f);
  model_matrix = glm::translate(glm::vec3(center, 0.0f)) * model_matrix;

  model_matrix =
      glm::ortho(0.0f, static_cast<float>(window_size.x),
                 0.0f, static_cast<float>(window_size.y), 0.0f, 0.01f)
      * model_matrix;

  glUseProgram(crosshair_shader_program_);
  glUniformMatrix4fv(glGetUniformLocation(crosshair_shader_program_, "uModel"), 1, GL_FALSE,
                     static_cast<const GLfloat *>(&model_matrix[0][0]));
  glUseProgram(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, crosshair_texture_);
  glUseProgram(crosshair_shader_program_);
  glUniform1i(glGetUniformLocation(crosshair_shader_program_, "uTexture"), 0);
  glUseProgram(0);

  glUseProgram(crosshair_shader_program_);
  glBindVertexArray(crosshair_vertex_array_);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(crosshair_indices_.size()),
                 GL_UNSIGNED_INT, reinterpret_cast<void *>(0));
  glBindVertexArray(0);
  glUseProgram(0);
}

glm::vec3 Game::GetCameraForward() const {
  glm::mat4 rotation(1.0f);
  rotation = glm::rotate(camera_rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f)) * rotation;
  rotation = glm::rotate(camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f)) * rotation;
  glm::vec3 forward = glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
  return forward;
}

glm::mat4 Game::GetCameraViewMatrix() const {
  glm::mat4 matrix(1.0f);
  matrix = glm::translate(-camera_position_) * matrix;
  matrix = glm::rotate(-camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f)) * matrix;
  matrix = glm::rotate(-camera_rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f)) * matrix;
  matrix = glm::scale(glm::vec3(1.0f / speed_)) * matrix;
  return matrix;
}

void Game::MouseDown(int button) {
  FocusWindow();

  if (button == MOUSE_BUTTON_LEFT) {
    BreakBlock();
    breaking_ = true;
    placing_ = false;
    last_block_time_ = input_->GetTime();
  }
  if (button == MOUSE_BUTTON_RIGHT) {
    PlaceBlock();
    placing_ = true;
    breaking_ = false;
    last_block_time_ = input_->GetTime();
  }
  if (button == MOUSE_BUTTON_MIDDLE) {
    CopyBlock();
  }
}

void Game::MouseUp(int button) {
  if (button == MOUSE_BUTTON_LEFT) {
    breaking_ = false;
  }
  if (button == MOUSE_BUTTON_RIGHT) {
    placing_ = false;
  }
}

void Game::Scroll(float offset) {
  if (offset < 0) {
    ShrinkBlock();
  } else if (offset > 0) {
    GrowBlock();
  }
}

void Game::KeyDown(int key) {
  if (key == KEY_Q) {
    ShrinkSize();
  }
  if (key == KEY_E) {
    GrowSize();
  }
  if (key == KEY_Z) {
    ShrinkBlock();
  }
  if (key == KEY_C) {
    GrowBlock();
  }

  if (key == KEY_1) {
    SetColor(kColor1);
  }
  if (key == KEY_2) {
    SetColor(kColor2);
  }
  if (key == KEY_3) {
    SetColor(kColor3);
  }
  if (key == KEY_4) {
    SetColor(kColor4);
  }
  if (key == KEY_5) {
    SetColor(kColor5);
  }

  if (key == KEY_R) {
    GenerateWorld();
  }

  if (key == KEY_G) {
    wireframe_ = !wireframe_;
  }
  if (key == KEY_ESCAPE) {
    UnfocusWindow();
  }
}

void Game::KeyUp(int key) {
}
