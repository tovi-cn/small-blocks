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

#include "glm/gtx/transform.hpp"

#include "utilities.h"

static const float kWorldSize = 10.0f;

static const int kMaxSizeDimension = -1;
// TODO
// static const int kMinSizeDimension = 8;
static const int kMinSizeDimension = 5;
static const int kDefaultSizeDimension = 0;

static const int kDefaultBlockDimension = 4;
// TODO
// static const int kMaxBlockDimension = 1;
static const int kMaxBlockDimension = kMaxSizeDimension + kDefaultBlockDimension;
// TODO
// static const int kMinBlockDimension = 16;
static const int kMinBlockDimension = kMinSizeDimension + kDefaultBlockDimension;

static const int kNoValue = 0x000000;  // Should always equal to 0.
static const int kColor1 = 0xeeeeee;   // White
static const int kColor2 = 0xea4611;   // Red
static const int kColor3 = 0x25e10e;   // Green
static const int kColor4 = 0x296fff;   // Blue
static const int kColor5 = 0xf9dd07;   // Yellow
static const int kDefaultColor = kColor1;

static const double kBlockInterval = 0.25;

Game::Game(Window *window, Renderer *renderer, InputSystem *input)
    : window_(window), renderer_(renderer), input_(input),
      window_focused_(false),
      mouse_last_position_(0.0f),
      mouse_delta_(0.0f),
      wireframe_(false),
      player_position_(0.0f), player_rotation_(0.0f), player_body_(nullptr),
      size_dimension_(kDefaultSizeDimension),
      block_dimension_(kDefaultBlockDimension),
      speed_(0.0f),
      color_(kDefaultColor),
      breaking_(false),
      placing_(false),
      block_interval_(kBlockInterval), last_block_time_(0.0),
      ray_cast_hit_(),
      world_(),
      bodies_(),
      block_geometry_(),
      block_material_(),
      block_mesh_(nullptr),
      highlight_geometry_(),
      highlight_material_(),
      highlight_mesh_(nullptr),
      crosshair_geometry_(),
      crosshair_material_(),
      crosshair_mesh_(nullptr)
      {}

Game::~Game() {
  glDeleteProgram(block_shader_program_);
  glDeleteProgram(highlight_shader_program_);
  glDeleteProgram(crosshair_shader_program_);

  glDeleteTextures(1, &block_texture_);
  glDeleteTextures(1, &highlight_texture_);
  glDeleteTextures(1, &crosshair_texture_);

  delete block_mesh_;
  delete highlight_mesh_;
  delete crosshair_mesh_;

  delete world_;

  for (Body *body : bodies_) {
    delete body;
  }
}

bool Game::Initialize() {
  input_->AddListener(this);

  window_->Maximize();
  FocusWindow();

  // Assets

  LoadAssets();

  // Block

  block_geometry_.positions() = kCubeVertexPositions;
  block_geometry_.normals() = kCubeVertexNormals;
  block_geometry_.uvs() = kCubeVertexUvs;
  block_geometry_.indices() = kCubeIndices;

  block_material_.set_shader_program(block_shader_program_);
  block_material_.set_texture(block_texture_);

  block_mesh_ = new Mesh(&block_geometry_, &block_material_);

  // Highlight

  highlight_geometry_.positions() = kCubeVertexPositions;
  highlight_geometry_.uvs() = kCubeVertexUvs;
  highlight_geometry_.indices() = kCubeIndices;

  highlight_material_.set_shader_program(highlight_shader_program_);
  highlight_material_.set_texture(highlight_texture_);

  highlight_mesh_ = new Mesh(&highlight_geometry_, &highlight_material_);

  // Crosshair

  crosshair_geometry_.positions() = kSquareVertexPositions;
  crosshair_geometry_.uvs() = kSquareVertexUvs;
  crosshair_geometry_.indices() = kSquareIndices;

  crosshair_material_.set_shader_program(crosshair_shader_program_);
  crosshair_material_.set_texture(crosshair_texture_);

  crosshair_mesh_ = new Mesh(&crosshair_geometry_, &crosshair_material_);

  glm::ivec2 window_size = window_->GetSize();

  glm::mat4 crosshair_model_matrix(1.0f);

  float crosshair_size = 0.025f * window_size.y;
  crosshair_model_matrix =
      glm::scale(glm::vec3(crosshair_size)) * crosshair_model_matrix;

  glm::vec2 crosshair_center(window_size.x / 2.0f - crosshair_size / 2.0f,
                             window_size.y / 2.0f - crosshair_size / 2.0f);
  crosshair_model_matrix =
      glm::translate(glm::vec3(crosshair_center, 0.0f))
      * crosshair_model_matrix;

  crosshair_model_matrix =
      glm::ortho(0.0f, static_cast<float>(window_size.x),
                 0.0f, static_cast<float>(window_size.y), 0.0f, 0.01f)
      * crosshair_model_matrix;
  crosshair_mesh_->set_model_matrix(crosshair_model_matrix);

  // Create world

  GenerateWorld();

  BoxBody *world_body =
      new BoxBody(glm::vec3(kWorldSize, kWorldSize / 2.0f, kWorldSize));
  // TODO: Add collision bodies for the world dynamically based on which
  // blocks have been placed or broken.
  bodies_.push_back(world_body);
  world_body->set_fixed(true);
  world_body->position() = glm::vec3(kWorldSize / 2.0f,
                                     (kWorldSize / 2.0f) / 2.0f,
                                     kWorldSize / 2.0f);

  // Player

  player_rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
  player_body_ = new BoxBody(glm::vec3(0.5f, 1.0f, 0.5f));
  bodies_.push_back(player_body_);
  player_body_->position() =
      glm::vec3(kWorldSize / 2.0f,
                kWorldSize / 2.0f + world_body->position().y
                    + world_body->size().y / 2.0f,
                kWorldSize / 2.0f);
  player_body_->position().y += player_body_->size().y / 2.0f;
  player_position_ = player_body_->position();
  player_position_.y -= player_body_->size().y / 2.0f;

  return true;
}

void Game::LoadAssets() {
  block_texture_ =
      renderer_->LoadTexture("assets/textures/block.png");
  highlight_texture_ =
      renderer_->LoadTexture("assets/textures/highlight.png");
  crosshair_texture_ =
      renderer_->LoadTexture("assets/textures/crosshair.png");

  block_shader_program_ =
      renderer_->LoadShaderProgram("assets/shaders/block");
  highlight_shader_program_ =
      renderer_->LoadShaderProgram("assets/shaders/highlight");
  crosshair_shader_program_ =
      renderer_->LoadShaderProgram("assets/shaders/crosshair");
}

void Game::Run() {
  mouse_last_position_ = input_->GetMousePosition();
  double last_time = input_->GetTime();

  while (!input_->ExitIsRequested()) {
    double current_time = input_->GetTime();
    float delta_time = static_cast<float>(current_time - last_time);
    last_time = current_time;

    input_->PollEvents();

    Update(delta_time);
    Render();
  }
}

void Game::Update(float delta_time) {
  glm::vec2 mouse_position = input_->GetMousePosition();
  mouse_delta_ = mouse_position - mouse_last_position_;
  mouse_last_position_ = mouse_position;
  if (!window_focused_) {
    mouse_delta_ = glm::vec2(0.0f);
  }

  speed_ = glm::pow(2.0f, -size_dimension_);

  UpdatePlayer(delta_time);

  ray_cast_hit_ = RayCastBlock();

  for (Body *body : bodies_) {
    body->position() += body->velocity() * delta_time;
    body->velocity() += body->acceleration() * delta_time;
  }

  HandleCollisions();

  double time = input_->GetTime();
  if (placing_ && time - last_block_time_ >= block_interval_) {
    PlaceBlock();
    last_block_time_ = time;
  }
  if (breaking_ && time - last_block_time_ >= block_interval_) {
    BreakBlock();
    last_block_time_ = time;
  }

  if (ray_cast_hit_.block) {
    highlight_mesh_->set_hidden(false);

    glm::mat4 highlight_model_matrix(1.0f);
    float highlight_size =
        kWorldSize * glm::pow(2.0f, static_cast<float>(-block_dimension_));
    float highlight_extra = highlight_size * 0.015f;
    highlight_model_matrix =
        glm::scale(glm::vec3(highlight_size + highlight_extra))
        * highlight_model_matrix;
    glm::vec3 highlight_position = ray_cast_hit_.position;
    highlight_position.x -= highlight_extra / 2.0f;
    highlight_position.y -= highlight_extra / 2.0f;
    highlight_position.z -= highlight_extra / 2.0f;
    highlight_model_matrix =
        glm::translate(highlight_position) * highlight_model_matrix;
    highlight_mesh_->set_model_matrix(highlight_model_matrix);
  } else {
    highlight_mesh_->set_hidden(true);
  }
}

void Game::UpdatePlayer(float delta_time) {
  float mouse_sensitivity = 0.005f;
  player_rotation_.x += -mouse_delta_.y * mouse_sensitivity;
  player_rotation_.y += -mouse_delta_.x * mouse_sensitivity;
  player_rotation_.x = glm::clamp(player_rotation_.x, glm::radians(-89.99f),
                                  glm::radians(89.99f));

  glm::vec3 forward = renderer_->GetCameraForward();
  forward.y = 0.0f;
  forward = glm::normalize(forward);
  glm::vec3 up(0.0f, 1.0f, 0.0f);
  glm::vec3 right = glm::normalize(glm::cross(forward, up));

  glm::vec3 direction(0.0f);
  if (input_->KeyIsPressed(KEY_W)) {
    direction += forward;
  }
  if (input_->KeyIsPressed(KEY_S)) {
    direction -= forward;
  }
  if (input_->KeyIsPressed(KEY_A)) {
    direction -= right;
  }
  if (input_->KeyIsPressed(KEY_D)) {
    direction += right;
  }
  if (direction == glm::vec3(0.0f)) {
    player_body_->velocity().x = 0.0f;
    player_body_->velocity().z = 0.0f;
  } else {
    glm::vec3 velocity = glm::normalize(direction) * speed_;
    player_body_->velocity().x = velocity.x;
    player_body_->velocity().z = velocity.z;
  }

  if (input_->KeyIsPressed(KEY_SPACE)) {
    player_body_->velocity().y = 3.0f * glm::pow(2.0f, -size_dimension_);
  }

  player_body_->acceleration().y = -9.0f * glm::pow(2.0f, -size_dimension_);

  // if (input_->KeyIsPressed(KEY_SPACE)) {
  //   player_position_ += up * speed_ * delta_time;
  // }
  // if (input_->KeyIsPressed(KEY_LEFT_SHIFT)) {
  //   player_position_ -= up * speed_ * delta_time;
  // }

  // player_position_ = player_body_->position();
  // player_position_.y -= player_body_->size().y / 2.0f;

  if (player_body_->position().y < -12 * kWorldSize) {
    player_body_->position().x = kWorldSize / 2.0f;
    player_body_->position().z = kWorldSize / 2.0f;
    player_body_->position().y = 12 * kWorldSize;
    player_body_->velocity().y = player_body_->acceleration().y;
  }

  player_position_ = player_body_->position();
  player_position_.y -= player_body_->size().y / 2.0f;

  float player_height = glm::pow(2.1f, -size_dimension_);
  player_body_->size().y = player_height;

  glm::vec3 camera_position = player_body_->position();
  camera_position.y += player_body_->size().y / 2.0f;
  renderer_->set_camera_position(camera_position);
  renderer_->set_camera_rotation(player_rotation_);
}

void ResolveCollision(Body *body1, Body *body2) {
  BoundingBox box1 = body1->GetBoundingBox();
  BoundingBox box2 = body2->GetBoundingBox();

  // Swap the bodies if the second body has a velocity.
  // Note: this doesn't currently handle collisions with two moving bodies.
  if (body2->velocity() != glm::vec3(0.0f)) {
    Body *tmp = body1;
    body1 = body2;
    body2 = tmp;
  }

  float overlap_x = glm::min(box1.right - box2.left, box2.right - box1.left);
  float overlap_y = glm::min(box1.top - box2.bottom, box2.top - box1.bottom);
  float overlap_z = glm::min(box1.front - box2.back, box2.front - box1.back);

  float bounce = 0.0f;

  if (overlap_x <= overlap_y && overlap_x <= overlap_z) {
    if (body1->position().x < body2->position().x) {
      body1->position().x -= overlap_x;
      if (body1->velocity().x > 0.0f) {
        body1->velocity().x *= -bounce;
      }
    } else {
      body1->position().x += overlap_x;
      if (body1->velocity().x < 0.0f) {
        body1->velocity().x *= -bounce;
      }
    }
  } else if (overlap_y <= overlap_x && overlap_y <= overlap_z) {
    if (body1->position().y < body2->position().y) {
      body1->position().y -= overlap_y;
      if (body1->velocity().y > 0.0f) {
        body1->velocity().y *= -bounce;
      }
    } else {
      body1->position().y += overlap_y;
      if (body1->velocity().y < 0.0f) {
        body1->velocity().y *= -bounce;
      }
    }
  } else {
    if (body1->position().z < body2->position().z) {
      body1->position().z -= overlap_z;
      if (body1->velocity().z > 0.0f) {
        body1->velocity().z *= -bounce;
      }
    } else {
      body1->position().z += overlap_z;
      if (body1->velocity().z < 0.0f) {
        body1->velocity().z *= -bounce;
      }
    }
  }
}

void Game::HandleCollisions() {
  // Ignore float precision for now.

  for (size_t i = 0; i < bodies_.size(); ++i) {
    Body *body1 = bodies_[i];
    for (size_t j = i + 1; j < bodies_.size(); ++j) {
      Body *body2 = bodies_[j];
      // float x1 = body1->position().x;
      // float y1 = body1->position().y;
      // float z1 = body1->position().z;
      // float x2 = body2->position().x;
      // float y2 = body2->position().y;
      // float z2 = body2->position().z;
      // std::cout << "body1: " << x1 << ", " << y1 << ", " << z1 << "\n";
      // std::cout << "body2: " << x2 << ", " << y2 << ", " << z2 << "\n";
      if (body1->CollidesWith(body2) &&
          !(body1->is_fixed() && body2->is_fixed())) {
        // std::cout << "COLLIDES\n";
        ResolveCollision(body1, body2);
      }
    }
  }
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
  // TODO: Make sure there are no smaller blocks contained at some dimension
  // at the previous position.
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
  hit.position = renderer_->camera_position();
  hit.previous_position = hit.position;
  glm::vec3 direction = renderer_->GetCameraForward();

  int num_steps = 10000;
  float block_size = kWorldSize * glm::pow(2.0f, -block_dimension_);
  float step_size =
      0.05f * glm::pow(2.0f, static_cast<float>(-block_dimension_));

  for (int i = 0; i < num_steps; ++i) {
    int dimension;
    Block *block = GetBlock(hit.position.x, hit.position.y, hit.position.z,
                            &dimension);
    if (block && (!block->is_leaf() || block->value() != kNoValue)) {
      hit.block = block;
      hit.dimension = dimension;

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
      break;
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

  return block;
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
  if (size_dimension_ >= kMinSizeDimension) {
    return;
  }
  // TODO: Is it more logical if the dimension decreases when shrinking?
  ++size_dimension_;
  // TODO: Redundant from if statement above
  size_dimension_ = glm::min(size_dimension_, kMinSizeDimension);

  player_body_->position().y -= (player_body_->size().y / 2.0f) / 2.0f;

  ShrinkBlock();
}

void Game::GrowSize() {
  --size_dimension_;
  size_dimension_ = glm::max(size_dimension_, kMaxSizeDimension);

  GrowBlock();
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
  glm::ivec2 window_size = window_->GetSize();
  renderer_->set_aspect(static_cast<float>(window_size.x) / window_size.y);

  renderer_->ClearScreen();

  DrawBlock(world_, 0.0f, 0.0f, 0.0f, kWorldSize);

  // Note: Draw transparent geometry and UI last.

  glUseProgram(highlight_mesh_->material()->shader_program());
  glUniform3f(
      glGetUniformLocation(highlight_mesh_->material()->shader_program(),
                           "uColor"),
      0.07f, 0.07f, 0.07f);
  glUseProgram(0);
  renderer_->RenderMesh(highlight_mesh_);

  renderer_->RenderMesh(crosshair_mesh_);

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
    block_mesh_->set_model_matrix(model_matrix);

    glUseProgram(block_shader_program_);
    float r = static_cast<float>((block->value() >> 16) & 0xff) / 0xff;
    float g = static_cast<float>((block->value() >> 8) & 0xff) / 0xff;
    float b = static_cast<float>(block->value() & 0xff) / 0xff;
    glUniform3f(glGetUniformLocation(block_shader_program_, "uColor"), r, g, b);
    glUseProgram(0);

    block_mesh_->set_wireframe(wireframe_);

    renderer_->RenderMesh(block_mesh_);
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

void Game::MouseDown(int button) {
  if (!window_focused_) {
    FocusWindow();
    return;
  }

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
