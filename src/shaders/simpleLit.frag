#version 330 core

/****************
  FRAGMENT shader
  Very simple lit shading. Just adjusts color intensity based on angle to light.
*****************/

uniform vec3 light_dir;
uniform vec4 color;

in vec3 frag_normal;
out vec4 outColor;

void main() {
  float d = dot( normalize(frag_normal), normalize(light_dir) );

  outColor = color * mix(0.5, 1.0, d);
}
