#include "game.h"

#include <iostream>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

static const struct {
  float x, y, z;
  float r, g, b;
} kVertices[3] = {
  { -0.6f, -0.4f, 0.0f, 1.f, 0.f, 0.f },
  {  0.6f, -0.4f, 0.0f, 0.f, 1.f, 0.f },
  {   0.f,  0.6f, 0.0f, 0.f, 0.f, 1.f }
};

static const char *kVertexShaderText =
"#version 330 core\n"
"uniform mat4 MVP;\n"
"layout (location = 0) in vec3 vPos;\n"
"layout (location = 1) in vec3 vCol;\n"
"out vec3 color;\n"
"void main() {\n"
"  gl_Position = MVP * vec4(vPos, 1.0);\n"
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
    : window_(nullptr),
      pressed_keys_(),
      camera_position_(0.0f), camera_rotation_(0.0f),
      mouse_last_x_(0.0), mouse_last_y_(0.0) {
  camera_rotation_.y = glm::radians(180.0f);
}

Game::~Game() {
  glfwDestroyWindow(window_);
  glfwTerminate();
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
  glfwSetWindowUserPointer(window_, this);

  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }

  glfwSetMouseButtonCallback(window_, OnMouseButtonEvent);
  glfwSetKeyCallback(window_, OnKeyEvent);

  // Initialize OpenGL

  glfwMakeContextCurrent(window_);
  gladLoadGL();
  glfwSwapInterval(1);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(OnGlError, 0);

  // Generate geometry

  glGenVertexArrays(1, &vertex_array_);
  glBindVertexArray(vertex_array_);

  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(kVertices[0]), (void *)0);
  // Color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(kVertices[0]), (void *)(3 * sizeof(float)));

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

  mvp_location_ = glGetUniformLocation(program_, "MVP");

  return true;
}

void Game::Run() {
  glfwGetCursorPos(window_, &mouse_last_x_, &mouse_last_y_);
  double last_time = glfwGetTime();

  while (!glfwWindowShouldClose(window_)) {
    double current_time = glfwGetTime();
    float delta_time = current_time - last_time;
    last_time = current_time;

    // Handle mouse and keyboard input

    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(window_, &mouse_x, &mouse_y);
    float mouse_delta_x = mouse_x - mouse_last_x_;
    float mouse_delta_y = mouse_y - mouse_last_y_;
    mouse_last_x_ = mouse_x;
    mouse_last_y_ = mouse_y;

    float mouse_sensitivity = 0.007f;
    camera_rotation_.x += -mouse_delta_y * mouse_sensitivity;
    camera_rotation_.y += -mouse_delta_x * mouse_sensitivity;
    camera_rotation_.x = glm::clamp(camera_rotation_.x, glm::radians(-90.0f),
                                    glm::radians(90.0f));

    float speed = 1.0f;
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
    if (pressed_keys_.test(GLFW_KEY_LEFT_CONTROL)) {
      camera_position_ -= up * speed * delta_time;
    }

    // Render scene

    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);
    float aspect = (float)width / height;
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glm::mat4 m(1.0f);
    m = glm::rotate((float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)) * m;
    m = glm::translate(glm::vec3(0.0f, 0.0f, 1.0f)) * m;

    glm::mat4 v(1.0f);
    v = glm::translate(-camera_position_) * v;
    v = glm::rotate(-camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f)) * v;
    v = glm::rotate(-camera_rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f)) * v;

    float fov = 80.0f;
    float near = 0.1f;;
    float far = 100.0f;
    glm::mat4 p = glm::perspective(glm::radians(fov), aspect, near, far);

    glm::mat4 mvp = p * v * m;

    glUseProgram(program_);
    glUniformMatrix4fv(mvp_location_, 1, GL_FALSE, (const GLfloat *)&mvp[0][0]);


    glBindVertexArray(vertex_array_);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glUseProgram(0);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void Game::MouseDown(int button) {
  pressed_mouse_buttons_.set(button);
}

void Game::MouseUp(int button) {
  pressed_mouse_buttons_.reset(button);
}

void Game::KeyDown(int key) {
  if (key == GLFW_KEY_W) {
  }
  pressed_keys_.set(key);

  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
  }
}

void Game::KeyUp(int key) {
  pressed_keys_.reset(key);
}

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
