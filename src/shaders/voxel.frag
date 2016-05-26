#version 330 core

/****************
  FRAGMENT shader
  Very simple lit shading. Just adjusts color intensity based on angle to light.
*****************/

uniform vec3 light_dir;

in vec4 frag_color;
in vec3 frag_normal;
out vec4 outColor;

void main()
{
  float d = dot( normalize(frag_normal), normalize(light_dir) );

  outColor = frag_color * mix(0.8, 1.0, d);
  outColor.a = 1.0;
}
