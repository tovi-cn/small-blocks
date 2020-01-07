#ifndef RENDERER_H_
#define RENDERER_H_

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "window.h"

class Renderer {
 public:
  Renderer(Window *window);
  ~Renderer();

  bool Initialize();

  GLuint CreateShaderProgram(const std::string &vertex_shader_text,
                             const std::string &fragment_shader_text);
  GLuint CreateShader(const std::string &text, GLenum type);

  void SwapBuffers();

 private:
  static void GLAPIENTRY OnGlError(
      GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
      const GLchar *message, const void *user_param);

  Window *window_;
};

#endif  // RENDERER_H_
