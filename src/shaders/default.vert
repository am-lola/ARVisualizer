#version 330 core

/****************
  VERTEX shader
  Applies the given Model-View-Projection matrix to each vertex.
  Passes an interpolated vertex color to the fragment shader.
*****************/

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

uniform mat4 MVP;

out vec3 frag_color;

void main()
{
  gl_Position = MVP * vec4(vertexPosition, 1.0); // make homogenous vector;

    // pass color through to fragment shader
    frag_color = vertexColor;
}
