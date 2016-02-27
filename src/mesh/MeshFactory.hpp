#ifndef _MESHFACTORY_H
#define _MESHFACTORY_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "geometry/Color.hpp"
#include "Mesh.hpp"

namespace ar
{

class MeshFactory
{
public:

  // 2D triangles
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec2> vertexPositions);

  // 3D triangles
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec3> vertexPositions);

  // 3D triangles with per-vertex colors
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec3> vertexPositions, Vector<Color> vertexColors);

  // 3D colored triangles (constant color)
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec3> vertexPositions, Color vertexColors);


  // 2D quads
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec2 center, double width, double height);

  // 3D quads
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height);

  // 3D quads with per-vertex colors
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, Vector<Color> vertexColors);

  // 3D quads with constant color
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, Color vertexColors);

  // 3D boxes
  template <typename MeshT>
  static MeshT MakeBox(glm::vec3 center, double xLength, double yLength, double zLength);

  // 3D cubes
  template <typename MeshT>
  static MeshT MakeCube(glm::vec3 center, double size);

  // Basic Icosphere
  template <typename MeshT>
  static MeshT MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions);

  // Icosphere with constant color
  template <typename MeshT>
  static MeshT MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions, Color color);


  // Basic UVSphere
  template <typename MeshT>
  static MeshT MakeUVSphere(glm::vec3 center, double radius, int resolution);

  // UVSphere with constant color
  template <typename MeshT>
  static MeshT MakeUVSphere(glm::vec3 center, double radius, Color color, int resolution);

  // Basic Capsule
  template <typename MeshT>
  static MeshT MakeCapsule(glm::vec3 center1, glm::vec3 center2, double radius, int resolution);

  // Capsule with constant color
  template <typename MeshT>
  static MeshT MakeCapsule(glm::vec3 center1, glm::vec3 center2, double radius, Color color, int resolution);

  // Triangle fan
  // Generates a triangle fan using the first vertex as the root. Vertex positions should be given in counter-clockwise order.
  // For best results, vertexPositions should describe a *convex* shape
  // Normals for each triangle are automatically generated, and are uniform across each face.
  template <typename MeshT>
  static MeshT MakeTriangleFan(Vector<glm::vec3> vertexPositions);

protected:
  // creates a transformation matrix to change orientation between from_rotation and to_rotation with a translation of offset
  static glm::mat4 MakeTransform(glm::vec3 offset, glm::vec3 from_rotation, glm::vec3 to_rotation);

  // Creates a quad, centered at the origin & aligned with the XY plane
  static void MakeQuadMesh(double width, double height, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices);

  // Creates a unit Icosphere and returns the vertex positions & indices in the given vectors
  static void MakeIcosphereMesh(unsigned int subdivisions, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices);

  // Creates a unit UVSphere and returns the vertex positions & indices in the given vectors
  static void MakeUVSphereMesh(unsigned int resolution, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices);

  // Creates a capsule at the origin with the given dimensions & returns the positions & indices in the given vectors
  static void MakeCapsuleMesh(double length, double radius, unsigned int resolution, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices);

  // Creates a box at the origin with the given dimensions & returns the positions & indices in the given vectors
  static void MakeBoxMesh(double xLength, double yLength, double zLength, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices);
};

} // namespace ar

#endif // _MESHFACTORY_H
