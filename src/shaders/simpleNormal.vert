#version 330 core

/****************
  VERTEX shader
  Applies the given Model-View-Projection matrix to each vertex.
  Passes an interpolated surface normal to the fragment shader.
*****************/

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec3 frag_normal;

void main()
{
  gl_Position = MVP * vec4(vertexPosition, 1.0); // make homogenous vector;

  // transform surface normal & pass to fragment shader
  frag_normal = ( V * M * vec4(normal, 0)).xyz;
}
