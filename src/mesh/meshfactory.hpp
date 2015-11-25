#ifndef _MESHFACTORY_H
#define _MESHFACTORY_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "mesh.hpp"

namespace ar
{

class MeshFactory
{
public:
  static Mesh<VertexP2> MakeTriangle(std::vector<glm::vec2> vertexPositions);
  static Mesh<VertexP3> MakeTriangle(std::vector<glm::vec3> vertexPositions);
  static Mesh<VertexP3C3> MakeTriangle(std::vector<glm::vec3> vertexPositions, std::vector<glm::vec3> vertexColors);
  static Mesh<VertexP3C3> MakeTriangle(std::vector<glm::vec3> vertexPositions, glm::vec3 vertexColors);

  static Mesh<VertexP2> MakeQuad(glm::vec2 center, double width, double height);
  static Mesh<VertexP3> MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height);
  static Mesh<VertexP3C3> MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, std::vector<glm::vec3> vertexColors);
  static Mesh<VertexP3C3> MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, glm::vec3 vertexColors);

protected:
  // creates a transformation matrix to change orientation between from_rotation and to_rotation with a translation of offset
  static glm::mat4 MakeTransform(glm::vec3 offset, glm::vec3 from_rotation, glm::vec3 to_rotation);
};

} // namespace ar

#endif // _MESHFACTORY_H
