#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 otherPosition;
layout(location = 2) in float otherDir;

uniform mat4 MV;
uniform mat4 P;
uniform float lineThickness = 0.004;
uniform float nearPlane = 0.2;
uniform float aspect = 1.0;

void clipLineSegmentToNearPlane(vec3 nearPoint, vec3 farPoint, out vec3 nearPointClipped, out bool cullPoints)
{
  vec3 dir = normalize(farPoint - nearPoint);
  float dirZ = -dir.z;

  nearPointClipped = nearPoint;
  cullPoints = false;

  float nearPointDist =  -(nearPlane + nearPoint.z);
  float farPointDist =  -(nearPlane + farPoint.z);

  // both points behind near plane, cull both points
  if (nearPointDist < 0.0 && farPointDist < 0.0)
  {
    cullPoints = true;
  }
  // near point behind near plane and not quite parallel
  else if (nearPointDist < 0.0 && abs(dirZ) > 0.01)
  {
    // calculate intersection with near plane
    float t = (nearPlane + nearPoint.z) / dirZ;
    nearPointClipped = nearPoint + t * dir;
  }
}

void main()
{
  float direction = ((gl_VertexID % 2) - 0.5) * 2.0;

  vec4 posViewSpace = MV * vec4(vertexPosition, 1.0);
  vec4 otherViewSpace = MV * vec4(otherPosition, 1.0);

  bool cullPoints;
  // decide which point is nearer
  if (posViewSpace.z > otherViewSpace.z)
  {
    // vertices that are behind the near plane need to be clipped to the near plane in order to get a valid projection
    clipLineSegmentToNearPlane(posViewSpace.xyz, otherViewSpace.xyz, posViewSpace.xyz, cullPoints);
  }
  else
  {
    clipLineSegmentToNearPlane(otherViewSpace.xyz, posViewSpace.xyz, otherViewSpace.xyz, cullPoints);
  }

  if (cullPoints)
  {
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  vec4 posProjected = P * posViewSpace;
  vec4 otherProjected = P * otherViewSpace;

  vec2 currentScreen = posProjected.xy / posProjected.w;
  vec2 otherScreen = otherProjected.xy / otherProjected.w;

  vec2 dir = normalize(otherScreen - currentScreen) * otherDir;
  vec2 normal = vec2(-dir.y, dir.x);

  normal *= direction * lineThickness * -0.5;
  normal.x /= aspect;

  gl_Position = vec4(currentScreen + normal, posProjected.z / posProjected.w, 1.0);
}
