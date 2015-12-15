#version 330 core

/****************
  FRAGMENT shader
  Renders a surface normal as color (for debugging)
*****************/

in vec3 frag_normal;
out vec4 outColor;

void main() {
  // convert normal from (-1..1) to (0..1) range for visualization
  vec3 n = (1. + normalize(frag_normal)) / 2.;

  outColor = vec4(n, 1.0);
}
