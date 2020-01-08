#version 330 core

uniform mat4 uModel;
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;
out vec2 texCoord;

void main() {
  gl_Position = uModel * vec4(vPos, 1.0);
  texCoord = vTexCoord;
}
