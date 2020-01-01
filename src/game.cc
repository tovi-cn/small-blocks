#include "game.h"

#include <iostream>

#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

static const struct {
  float x, y;
  float r, g, b;
} kVertices[3] = {
  { -0.6f, -0.4f, 1.f, 0.f, 0.f },
  {  0.6f, -0.4f, 0.f, 1.f, 0.f },
  {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char *kVertexShaderText =
"#version 330 core\n"
"uniform mat4 MVP;\n"
"layout (location = 0) in vec2 vPos;\n"
"layout (location = 1) in vec3 vCol;\n"
"out vec3 color;\n"
"void main() {\n"
"  gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"  color = vCol;\n"
"}\n";

static const char *kFragmentShaderText =
"#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"  gl_FragColor = vec4(color, 1.0);\n"
"}\n";

Game::Game() {
  // Initialize Window with OpenGL
  glfwSetErrorCallback(OnGlfwError);
  if (!glfwInit()) {
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window_ = glfwCreateWindow(800, 480, "Recurse Builder", NULL, NULL);
  if (!window_) {
    glfwTerminate();
    exit(1);
  }
  glfwSetKeyCallback(window_, OnKeyPress);
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
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                        sizeof(kVertices[0]), (void *)0);
  // Color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(kVertices[0]), (void *)(sizeof(float) * 2));

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
}

Game::~Game() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Game::Initialize() {
  return true;
}

void Game::Run() {
  while (!glfwWindowShouldClose(window_)) {
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);
    float aspect = (float)width / height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 m = glm::mat4(1.0f);
    m = glm::rotate((float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 p = glm::ortho(-aspect, aspect, -1.0f, 1.0f, 1.0f, -1.0f);
    glm::mat4 mvp = p * m;
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

void Game::OnKeyPress(GLFWwindow *window, int key, int scancode,
                      int action, int mods) {
  (void)scancode;
  (void)mods;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
