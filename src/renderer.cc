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

#include "renderer.h"

#include <algorithm>
#include <iostream>

#include "stb_image.h"

#include "utilities.h"

static const float kDefaultFov = glm::radians(90.0f);
static const float kDefaultAspect = 1.0f;
static const float kDefaultNear = 0.01f;
static const float kDefaultFar = 500.0f;

static const std::string kVertexShaderFileExtension = ".vert";
static const std::string kFragmentShaderFileExtension = ".frag";

static const int kNumTextureImageComponents = 4;

Renderer::Renderer(Window *window)
    : window_(window),
      fov_(kDefaultFov),
      aspect_(kDefaultAspect),
      near_(kDefaultNear),
      far_(kDefaultFar),
      camera_position_(0.0f),
      camera_rotation_(0.0f),
      render_list_() {}

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

void Renderer::AddMesh(Mesh *mesh) {
  if (std::find(render_list_.begin(), render_list_.end(), mesh)
          != render_list_.end()) {
    return;
  }
  render_list_.push_back(mesh);
}

void Renderer::Render() {
  glm::ivec2 window_size = window_->GetSize();
  glViewport(0, 0, window_size.x, window_size.y);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto mesh : render_list_) {
    RenderMesh(mesh);
  }
}

void Renderer::RenderMesh(Mesh *mesh) {
  if (mesh->hidden()) {
    return;
  }

  glPolygonMode(GL_FRONT_AND_BACK, mesh->wireframe() ? GL_LINE : GL_FILL);

  GLuint shader_program = mesh->material()->shader_program();
  glUseProgram(shader_program);

  glm::mat4 view_projection_matrix = GetViewProjectionMatrix();

  glUniformMatrix4fv(glGetUniformLocation(shader_program, "uViewProjection"),
                     1, GL_FALSE,
                     static_cast<const GLfloat *>(&view_projection_matrix[0][0]));

  glUniformMatrix4fv(glGetUniformLocation(shader_program, "uModel"),
                     1, GL_FALSE,
                     static_cast<const GLfloat *>(&mesh->model_matrix()[0][0]));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mesh->material()->texture());
  glUniform1i(glGetUniformLocation(shader_program, "uTexture"), 0);

  glBindVertexArray(mesh->vertex_array());
  glDrawElements(GL_TRIANGLES,
                 static_cast<GLsizei>(mesh->geometry()->indices().size()),
                 GL_UNSIGNED_INT,
                 reinterpret_cast<void *>(0));
  glBindVertexArray(0);

  glUseProgram(0);
}

void Renderer::SwapBuffers() {
  glfwSwapBuffers(window_->window_glfw());
}

GLuint Renderer::LoadShaderProgram(const std::string &shader_path) {
  std::string vertex_shader_path =
      shader_path + kVertexShaderFileExtension;
  GLuint vertex_shader =
      LoadShader(vertex_shader_path, GL_VERTEX_SHADER);

  std::string fragment_shader_path =
      shader_path + kFragmentShaderFileExtension;
  GLuint fragment_shader =
      LoadShader(fragment_shader_path, GL_FRAGMENT_SHADER);

  return CreateShaderProgram(vertex_shader, fragment_shader);
}

GLuint Renderer::CreateShaderProgram(const std::string &vertex_shader_text,
                                     const std::string &fragment_shader_text) {
  GLuint vertex_shader =
      CreateShader(vertex_shader_text, GL_VERTEX_SHADER);
  GLuint fragment_shader =
      CreateShader(fragment_shader_text, GL_FRAGMENT_SHADER);
  return CreateShaderProgram(vertex_shader, fragment_shader);
}

GLuint Renderer::CreateShaderProgram(GLuint vertex_shader,
                                     GLuint fragment_shader) {
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

GLuint Renderer::LoadShader(const std::string &path, GLenum type) {
  std::cout << "Loading shader " << path << "\n";
  std::string text;
  if (!LoadFile(path, &text)) {
    std::cerr << "Failed to load " << path << "\n";
    return 0;
  }
  return CreateShader(text, type);
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

GLuint Renderer::LoadTexture(const std::string &image_path) {
  std::cout << "Loading texture " << image_path << "\n";

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int num_image_components = kNumTextureImageComponents;
  int image_width;
  int image_height;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *image_data =
      stbi_load(image_path.c_str(), &image_width, &image_height, nullptr,
                num_image_components);
  stbi_set_flip_vertically_on_load(false);

  if (image_data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
  } else {
    std::cerr << "Failed to load texture " << image_path << "\n";
  }

  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
}

void GLAPIENTRY Renderer::OnGlError(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
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
