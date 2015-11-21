#version 330 core

in vec2 texCoord;
out vec3 outColor;

uniform sampler2D videotex;

void main() {
  outColor = texture(videotex, texCoord).rgb;
}
