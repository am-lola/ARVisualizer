#version 330 core

/****************
  FRAGMENT shader
  Just applies the provided texture directly to the current fragment.
  No lighting, transparency, etc.
*****************/

in vec2 texCoord;
out vec3 outColor;

uniform sampler2D tex;

void main() {
  outColor = texture(tex, texCoord).rgb;
}
