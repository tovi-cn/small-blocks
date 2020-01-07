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
