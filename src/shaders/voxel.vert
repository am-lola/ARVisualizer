#version 330 core

/****************
  VERTEX shader
  Applies the given Model-View-Projection matrix to each vertex.
  Passes an interpolated vertex color to the fragment shader.
*****************/

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

// instance data
layout(location = 2) in vec3 pos;
layout(location = 3) in vec4 color;
layout(location = 4) in float scale;

uniform mat4 MV;
uniform mat4 MVP;

out vec4 frag_color;
out vec3 frag_normal;

void main()
{
  gl_Position = MVP * vec4(vertex.xyz*scale + pos, 1.0);

  // pass color through to fragment shader
  frag_color = color;
  frag_normal = (MV * vec4(normal, 0)).xyz;
}
