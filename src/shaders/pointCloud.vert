#version 330 core

/****************
  VERTEX shader
  Applies the given Model-View-Projection matrix to each vertex.
  Passes an interpolated vertex color to the fragment shader.
*****************/

layout(location = 0) in vec4 vertexPosition;
#ifdef WITH_COLOR
layout(location = 1) in vec4 vertexColor;
#endif

uniform mat4 MVP;
uniform mat4 M;

uniform float fadeDepth = 5.0f;

out vec4 frag_color;

void main()
{
  gl_Position = MVP * vec4(vertexPosition.xyz, 1.0);

  vec4 posWorldSpace = M * vec4(vertexPosition.xyz, 1.0);

  // Point cloud starts at z = 1.0
  float inverseDepth = (fadeDepth - posWorldSpace.z + 1.0f) / fadeDepth;
  // Fade out with increasing depth
  #ifdef WITH_COLOR
  vec3 color = vertexColor.bgr;
  #else
  vec3 color = vec3(1.0f);
  #endif
  frag_color = vec4(color, inverseDepth);
}
