#version 330 core

/****************
  FRAGMENT shader
  Just applies the provided RGB color directly to the current fragment.
  No lighting, transparency, etc.
*****************/

in vec4 frag_color;
out vec4 outColor;

void main()
{
  outColor = frag_color;
}
