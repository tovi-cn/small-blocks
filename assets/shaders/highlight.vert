#version 330 core

uniform mat4 uViewProjection;
uniform mat4 uModel;
uniform vec3 uColor;
layout (location = 0) in vec3 vPos;
layout (location = 2) in vec2 vTexCoord;
out vec3 color;
out vec2 texCoord;

void main() {
  gl_Position = uViewProjection * uModel * vec4(vPos, 1.0);
  color = uColor;
  texCoord = vTexCoord;
}
