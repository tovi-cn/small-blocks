#version 330 core

uniform sampler2D uTexture;
in vec3 color;
in vec2 texCoord;
out vec4 FragColor;

void main() {
  FragColor = texture(uTexture, texCoord) * vec4(color, 1.0);
}
