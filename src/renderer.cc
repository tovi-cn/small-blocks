#include "renderer.h"

#include <iostream>

Renderer::Renderer(Window *window) : window_(window) {}

Renderer::~Renderer() {}

bool Renderer::Initialize() {
  if (!gladLoadGL()) {
    std::cerr << "gladLoadGL() failed\n";
    return false;
  }

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(OnGlError, 0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

GLuint Renderer::CreateShaderProgram(const std::string &vertex_shader_text,
                                     const std::string &fragment_shader_text) {
  GLuint vertex_shader =
      CreateShader(vertex_shader_text, GL_VERTEX_SHADER);
  GLuint fragment_shader =
      CreateShader(fragment_shader_text, GL_FRAGMENT_SHADER);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  GLint program_linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
  if (!program_linked) {
    GLsizei log_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    GLchar *error_log = new GLchar[log_length];
    glGetProgramInfoLog(program, log_length, nullptr, error_log);
    glDeleteProgram(program);
    std::cerr << "Link error: " << error_log << "\n";
    delete error_log;
  }

  return program;
}

GLuint Renderer::CreateShader(const std::string &text, GLenum type) {
  GLuint shader = glCreateShader(type);
  const GLchar *texts[] = {text.c_str()};
  glShaderSource(shader, 1, texts, nullptr);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLsizei log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    std::string error_log;
    error_log.resize(log_length);
    glGetShaderInfoLog(shader, log_length, nullptr, &error_log[0]);
    glDeleteShader(shader);
    std::cerr << "Error: " << error_log << "\n";
  }

  return shader;
}

void Renderer::SwapBuffers() {
  glfwSwapBuffers(window_->window_glfw());
}

void GLAPIENTRY Renderer::OnGlError(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *message, const void *user_param) {
  (void)source;
  (void)id;
  (void)length;
  (void)user_param;
  std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
            << ", type = 0x" << type
            << ", severity = 0x" << severity
            << ", message = " << message << "\n";
}
