#ifndef _MESHFACTORY_H
#define _MESHFACTORY_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "color.hpp"
#include "mesh.hpp"

namespace ar
{

class MeshFactory
{
public:

  // 2D triangles
  template <typename VertexT>
  static Mesh<VertexT> MakeTriangle(std::vector<glm::vec2> vertexPositions);

  // 3D triangles
  template <typename VertexT>
  static Mesh<VertexT> MakeTriangle(std::vector<glm::vec3> vertexPositions);

  // 3D triangles with per-vertex colors
  template <typename VertexT>
  static Mesh<VertexT> MakeTriangle(std::vector<glm::vec3> vertexPositions, std::vector<Color> vertexColors);

  // 3D colored triangles (constant color)
  template <typename VertexT>
  static Mesh<VertexT> MakeTriangle(std::vector<glm::vec3> vertexPositions, Color vertexColors);


  // 2D quads
  template <typename VertexT>
  static Mesh<VertexT> MakeQuad(glm::vec2 center, double width, double height);

  // 3D quads
  template <typename VertexT>
  static Mesh<VertexT> MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height);

  // 3D quads with per-vertex colors
  template <typename VertexT>
  static Mesh<VertexT> MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, std::vector<Color> vertexColors);

  // 3D quads with constant color
  template <typename VertexT>
  static Mesh<VertexT> MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, Color vertexColors);


  // Basic Icosphere
  template <typename VertexT>
  static Mesh<VertexT> MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions);

  // Icosphere with constant color
  template <typename VertexT>
  static Mesh<VertexT> MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions, Color color);


  // Basic UVSphere
  template <typename VertexT>
  static Mesh<VertexT> MakeUVSphere(glm::vec3 center, double radius, int resolution);

  // UVSphere with constant color
  template <typename VertexT>
  static Mesh<VertexT> MakeUVSphere(glm::vec3 center, double radius, Color color, int resolution);

  // Basic Capsule
  template <typename VertexT>
  static Mesh<VertexT> MakeCapsule(glm::vec3 center1, glm::vec3 center2, double radius, int resolution);

  // Capsule with constant color
  template <typename VertexT>
  static Mesh<VertexT> MakeCapsule(glm::vec3 center1, glm::vec3 center2, double radius, Color color, int resolution);

protected:
  // creates a transformation matrix to change orientation between from_rotation and to_rotation with a translation of offset
  static glm::mat4 MakeTransform(glm::vec3 offset, glm::vec3 from_rotation, glm::vec3 to_rotation);

  // Creates a quad, centered at the origin & aligned with the XY plane
  static void MakeQuadMesh(double width, double height, std::vector<glm::vec3>* vertex_positions, std::vector<GLuint>* indices);

  // Creates a unit Icosphere and returns the vertex positions & indices in the given vectors
  static void MakeIcosphereMesh(unsigned int subdivisions, std::vector<glm::vec3>* vertex_positions, std::vector<GLuint>* indices);

  // Creates a unit UVSphere and returns the vertex positions & indices in the given vectors
  static void MakeUVSphereMesh(unsigned int resolution, std::vector<glm::vec3>* vertex_positions, std::vector<GLuint>* indices);

  // Creates a capsule at the origin with the given dimensions & returns the positions & indices in the given vectors
  static void MakeCapsuleMesh(double length, double radius, unsigned int resolution, std::vector<glm::vec3>* vertex_positions, std::vector<GLuint>* indices);
};

} // namespace ar

#endif // _MESHFACTORY_H
