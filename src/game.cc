#include "game.h"

#include <iostream>
#include <vector>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

static const char *kVertexShaderText =
"#version 330 core\n"
"uniform mat4 uViewProjection;\n"
"uniform mat4 uModel;\n"
"layout (location = 0) in vec3 vPos;\n"
"layout (location = 1) in vec3 vCol;\n"
"out vec3 color;\n"
"void main() {\n"
"  gl_Position = uViewProjection * uModel * vec4(vPos, 1.0);\n"
"  color = vCol;\n"
"}\n";

static const char *kFragmentShaderText =
"#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"  gl_FragColor = vec4(color, 1.0);\n"
"}\n";

Game::Game() 
    : window_(nullptr), window_focused_(false),
      pressed_keys_(),
      camera_position_(0.0f), camera_rotation_(0.0f),
      mouse_last_x_(0.0), mouse_last_y_(0.0),
      world_() {}

Game::~Game() {
  // Deallocate OpenGL objects
  glDeleteVertexArrays(1, &vertex_array_);
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteProgram(program_);

  glfwDestroyWindow(window_);
  glfwTerminate();

  delete world_;
}

bool Game::Initialize() {
  // Initialize Window

  glfwSetErrorCallback(OnGlfwError);
  if (!glfwInit()) {
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = glfwCreateWindow(800, 600, "Recurse Builder", NULL, NULL);
  if (!window_) {
    glfwTerminate();
    return false;
  }

  glfwSetMouseButtonCallback(window_, OnMouseButtonEvent);
  glfwSetKeyCallback(window_, OnKeyEvent);

  glfwSetWindowUserPointer(window_, this);

  glfwMaximizeWindow(window_);
  FocusWindow();

  // Initialize OpenGL

  glfwMakeContextCurrent(window_);
  gladLoadGL();
  glfwSwapInterval(0);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(OnGlError, 0);

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);  
  // glCullFace(GL_BACK);

  // Camera position

  camera_position_ = glm::vec3(kWorldSize / 2.0f, 1.5f, -2.5f);
  camera_rotation_.y = glm::radians(-180.0f);

  // Generate geometry

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
                        sizeof(vertices_[0]), reinterpret_cast<void *>(0));
  // Color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertices_[0]),
                        reinterpret_cast<void *>(3 * sizeof(float)));

  glBindVertexArray(0);

  // Load shaders

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &kVertexShaderText, NULL);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &kFragmentShaderText, NULL);
  glCompileShader(fragment_shader);

  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader);
  glAttachShader(program_, fragment_shader);
  glLinkProgram(program_);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  view_projection_location_ = glGetUniformLocation(program_, "uViewProjection");
  model_location_ = glGetUniformLocation(program_, "uModel");

  // Create world

  world_ = new Block();
  world_->set_value(1);
  SetBlock(0.0f, 0.0f, 0.0f, 4, 0);

  return true;
}

void Game::Run() {
  glfwGetCursorPos(window_, &mouse_last_x_, &mouse_last_y_);
  double last_time = glfwGetTime();

  while (!glfwWindowShouldClose(window_)) {
    double current_time = glfwGetTime();
    float delta_time = static_cast<float>(current_time - last_time);
    last_time = current_time;

    Update(delta_time);
    Render();

    glfwPollEvents();
  }
}

void Game::SetBlock(float x, float y, float z, int dimension, int value) {
  Block *block = world_;
  float size = kWorldSize;
  float dx = 0.0f;
  float dy = 0.0f;
  float dz = 0.0f;

  for (int i = 0; i < dimension; ++i) {
    size /= 2.0f;
    int index = 0;
    if (x < size + dx && y < size + dy && z < size + dz) {
      index = 0;
    } else if (x >= size + dx && y < size + dy && z < size + dz) {
      index = 1;
      dx += size;
    } else if (x < size + dx && y >= size + dy && z < size + dz) {
      index = 2;
      dy += size;
    } else if (x >= size + dx && y >= size + dy && z < size + dz) {
      index = 3;
      dx += size;
      dy += size;
    } else if (x < size + dx && y < size + dy && z >= size + dz) {
      index = 4;
      dz += size;
    } else if (x >= size + dx && y < size + dy && z >= size + dz) {
      index = 5;
      dx += size;
      dz += size;
    } else if (x < size + dx && y >= size + dy && z >= size + dz) {
      index = 6;
      dy += size;
      dz += size;
    } else if (x >= size + dx && y >= size + dy && z >= size + dz) {
      index = 7;
      dx += size;
      dy += size;
      dz += size;
    }

    Block *child = block->child(index);
    if (!child) {
      child = new Block(block->value());
      block->set_child(index, child);
    }
    block = child;
  }

  block->set_value(value);
}

void Game::Update(float delta_time) {
  double mouse_x;
  double mouse_y;
  glfwGetCursorPos(window_, &mouse_x, &mouse_y);
  float mouse_delta_x = static_cast<float>(mouse_x - mouse_last_x_);
  float mouse_delta_y = static_cast<float>(mouse_y - mouse_last_y_);
  mouse_last_x_ = mouse_x;
  mouse_last_y_ = mouse_y;

  if (window_focused_) {
    float mouse_sensitivity = 0.005f;
    camera_rotation_.x += -mouse_delta_y * mouse_sensitivity;
    camera_rotation_.y += -mouse_delta_x * mouse_sensitivity;
    camera_rotation_.x = glm::clamp(camera_rotation_.x, glm::radians(-90.0f),
                                    glm::radians(90.0f));

    float speed = 2.0f;
    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f)) * rotation;
    glm::vec3 forward = glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    glm::vec3 direction(0.0f);
    if (pressed_keys_.test(GLFW_KEY_W)) {
      direction += forward;
    }
    if (pressed_keys_.test(GLFW_KEY_S)) {
      direction -= forward;
    }
    if (pressed_keys_.test(GLFW_KEY_A)) {
      direction -= right;
    }
    if (pressed_keys_.test(GLFW_KEY_D)) {
      direction += right;
    }
    if (direction != glm::vec3(0.0f)) {
      camera_position_ += glm::normalize(direction) * speed * delta_time;
    }

    if (pressed_keys_.test(GLFW_KEY_SPACE)) {
      camera_position_ += up * speed * delta_time;
    }
    if (pressed_keys_.test(GLFW_KEY_LEFT_SHIFT)) {
      camera_position_ -= up * speed * delta_time;
    }
  }
}

void Game::Render() {
  int width;
  int height;
  glfwGetFramebufferSize(window_, &width, &height);
  float aspect = static_cast<float>(width) / height;
  glViewport(0, 0, width, height);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float fov = 90.0f;
  float near = 0.01f;;
  float far = 100.0f;
  glm::mat4 projection_matrix =
      glm::perspective(glm::radians(fov), aspect, near, far);

  glm::mat4 view_matrix(1.0f);
  view_matrix = glm::translate(-camera_position_) * view_matrix;
  view_matrix = glm::rotate(-camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f)) * view_matrix;
  view_matrix = glm::rotate(-camera_rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f)) * view_matrix;

  glm::mat4 view_projection_matrix = projection_matrix * view_matrix;

  glUseProgram(program_);
  glUniformMatrix4fv(view_projection_location_, 1, GL_FALSE,
                     static_cast<const GLfloat *>(&view_projection_matrix[0][0]));
  glUseProgram(0);

  DrawBlock(world_, 0.0f, 0.0f, 0.0f, kWorldSize);

  glfwSwapBuffers(window_);
}

void Game::DrawBlock(Block *block, float x, float y, float z, float size) {
  if (!block) {
    return;
  }
  if (block->value()) {
    glm::mat4 model_matrix(1.0f);
    model_matrix = glm::scale(glm::vec3(size)) * model_matrix;
    model_matrix = glm::translate(glm::vec3(x, y, z)) * model_matrix;

    glUseProgram(program_);
    glUniformMatrix4fv(model_location_, 1, GL_FALSE,
                       static_cast<const GLfloat *>(&model_matrix[0][0]));
    glUseProgram(0);

    glUseProgram(program_);
    glBindVertexArray(vertex_array_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT,
                   reinterpret_cast<void *>(0));
    glBindVertexArray(0);
    glUseProgram(0);
  }

  if (!block->is_leaf()) {
    size /= 2;
    DrawBlock(block->child(0), x, y, z, size);
    DrawBlock(block->child(1), x + size, y, z, size);
    DrawBlock(block->child(2), x, y + size, z, size);
    DrawBlock(block->child(3), x + size, y + size, z, size);

    DrawBlock(block->child(4), x, y, z + size, size);
    DrawBlock(block->child(5), x + size, y, z + size, size);
    DrawBlock(block->child(6), x, y + size, z + size, size);
    DrawBlock(block->child(7), x + size, y + size, z + size, size);
  }
}

void Game::MouseDown(int button) {
  pressed_mouse_buttons_.set(button);

  FocusWindow();
}

void Game::MouseUp(int button) {
  pressed_mouse_buttons_.reset(button);
}

void Game::KeyDown(int key) {
  pressed_keys_.set(key);

  if (key == GLFW_KEY_ESCAPE) {
    UnfocusWindow();
  }
}

void Game::KeyUp(int key) {
  pressed_keys_.reset(key);
}

void Game::FocusWindow() {
  window_focused_ = true;
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
}

void Game::UnfocusWindow() {
  window_focused_ = false;
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
  }
}

//
// External event handling
//

void Game::OnMouseButtonEvent(GLFWwindow *window, int button,
                              int action, int mods) {
  (void)mods;

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    game->MouseDown(button);
  } else if (action == GLFW_RELEASE) {
    game->MouseUp(button);
  }
}

void Game::OnKeyEvent(GLFWwindow *window, int key, int scancode,
                      int action, int mods) {
  (void)scancode;
  (void)mods;

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    game->KeyDown(key);
  } else if (action == GLFW_RELEASE) {
    game->KeyUp(key);
  }
}

void Game::OnGlfwError(int error, const char *description) {
  (void)error;
  std::cerr << "Error: " << description << "\n";
}

void GLAPIENTRY Game::OnGlError(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *user_param) {
  (void)source;
  (void)id;
  (void)length;
  (void)user_param;
  std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
            << ", type = 0x" << type
            << ", severity = 0x" << severity
            << ", message = " << message << "\n";
}
