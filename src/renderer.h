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

#ifndef RENDERER_H_
#define RENDERER_H_

#include <list>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "mesh.h"
#include "window.h"

class Renderer {
 public:
  Renderer(Window *window);
  ~Renderer();

  bool Initialize();

  void AddMesh(Mesh *mesh);

  void Render();
  void RenderMesh(Mesh *mesh);
  void SwapBuffers();

  glm::mat4 GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
  }
  glm::mat4 GetProjectionMatrix() const {
    return glm::perspective(fov_, aspect_, near_, far_);
  }

  glm::mat4 GetViewMatrix() const {
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(-camera_position_) * matrix;
    matrix = glm::rotate(-camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f))
                 * matrix;
    matrix = glm::rotate(-camera_rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f))
                 * matrix;
    // TODO: Add a scale factor for rendering extremely small geometry.
    // matrix = glm::scale(glm::vec3(1.0f / speed_)) * matrix;
    return matrix;
  }

  glm::vec3 GetCameraForward() const {
    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(camera_rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f))
                   * rotation;
    rotation = glm::rotate(camera_rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f))
                   * rotation;
    glm::vec3 forward =
        glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
    return forward;
  }

  float fov() const { return fov_; };
  void set_fov(float fov) { fov_ = fov; };

  float aspect() const { return aspect_; };
  void set_aspect(float aspect) { aspect_ = aspect; };

  float near() const { return near_; };
  void set_near(float near) { near_ = near; };

  float far() const { return far_; };
  void set_far(float far) { far_ = far; };

  glm::vec3 camera_position() const { return camera_position_; }
  void set_camera_position(glm::vec3 camera_position) {
    camera_position_ = camera_position;
  }

  glm::vec3 camera_rotation() const { return camera_rotation_; }
  void set_camera_rotation(glm::vec3 camera_rotation) {
    camera_rotation_ = camera_rotation;
  }

  GLuint CreateShaderProgram(const std::string &vertex_shader_text,
                             const std::string &fragment_shader_text);
  GLuint CreateShader(const std::string &text, GLenum type);

 private:
  static void GLAPIENTRY OnGlError(GLenum source, GLenum type, GLuint id,
                                   GLenum severity, GLsizei length,
                                   const GLchar *message,
                                   const void *user_param);

  Window *window_;

  float fov_;
  float aspect_;
  float near_;
  float far_;
  glm::vec3 camera_position_;
  glm::vec3 camera_rotation_;

  std::list<Mesh *> render_list_;
};

#endif  // RENDERER_H_
