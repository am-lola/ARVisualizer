#ifndef _ARSCENEINFO_H
#define _ARSCENEINFO_H

#include <glm/glm.hpp>

namespace ar
{

struct SceneInfo
{
  GLuint renderType;
  glm::mat4 viewMatrix;
  glm::mat4 projectionMatrix;
  glm::vec3 lightDir;
  float nearClip;
  float farClip;
  float aspect;
};

}

#endif // _ARSCENEINFO_H
