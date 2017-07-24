#ifndef _ARSCENEINFO_H
#define _ARSCENEINFO_H

#include <glm/glm.hpp>
#include <unordered_map>

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
  std::unordered_map<unsigned int, bool>* visibilityMap;
  bool lightAlpha;
  bool onlyOpaque;

  bool shouldDraw(unsigned handle) const
  {
    return visibilityMap->find(handle) == visibilityMap->end() || visibilityMap->at(handle) == true;
  }
};

} // namespace ar

#endif // _ARSCENEINFO_H
