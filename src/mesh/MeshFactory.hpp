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
  // @MeshT The <Mesh> type to construct
  // @vertexPositions Coordinates of the triangle vertices
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec2> vertexPositions);

  // 3D triangles
  // @MeshT The <Mesh> type to construct
  // @vertexPositions Coordinates of the triangle vertices
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec3> vertexPositions);

  // 3D triangles with per-vertex colors
  // @MeshT           The <Mesh> type to construct
  // @vertexPositions Coordinates of the triangle vertices
  // @vertexColors    Colors to assign to each vertex. Length must match <vertexPositions>
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec3> vertexPositions, Vector<Color> vertexColors);

  // 3D colored triangles (constant color)
  // @MeshT           The <Mesh> type to construct
  // @vertexPositions Coordinates of the triangle vertices
  // @vertexColors    Color to apply at each vertex
  template <typename MeshT>
  static MeshT MakeTriangle(Vector<glm::vec3> vertexPositions, Color vertexColors);

  // 2D quads
  // @MeshT  The <Mesh> type to construct
  // @center The center point of the shape
  // @width  Width of the quad
  // @height Height of the quad
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec2 center, double width, double height);

  // 3D quads
  // @MeshT  The <Mesh> type to construct
  // @center The center point of the shape
  // @normal Vector normal to the quad's surface
  // @width  Width of the quad
  // @height Height of the quad
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height);

  // 3D quads with per-vertex colors
  // @MeshT  The <Mesh> type to construct
  // @center The center point of the shape
  // @normal Vector normal to the quad's surface
  // @width  Width of the quad
  // @height Height of the quad
  // @vertexColors Colors to assign to each vertex. Length should be 4.
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, Vector<Color> vertexColors);

  // 3D quads with constant color
  // @MeshT  The <Mesh> type to construct
  // @center The center point of the shape
  // @normal Vector normal to the quad's surface
  // @width  Width of the quad
  // @height Height of the quad
  // @vertexColors <Color> to apply at each vertex
  template <typename MeshT>
  static MeshT MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, Color vertexColors);

  // 3D boxes
  // @MeshT   The <Mesh> type to construct
  // @center  The center point of the shape
  // @xLength Length of box along X-axis
  // @yLength Length of box along Y-axis
  // @zLength Length of box along Z-axis
  //
  // @return New <Mesh> object
  template <typename MeshT>
  static MeshT MakeBox(glm::vec3 center, double xLength, double yLength, double zLength);

  // 3D cubes
  // @MeshT   The <Mesh> type to construct
  // @center  The center point of the shape
  // @size    Length of the cube's sides
  template <typename MeshT>
  static MeshT MakeCube(glm::vec3 center, double size);

  // Basic Icosphere
  // @MeshT        The <Mesh> type to construct
  // @center       The center point of the shape
  // @radius       Radius of the sphere
  // @subdivisions Number of times to subdivide each face of the sphere. Higher->Smoother, but slower.
  template <typename MeshT>
  static MeshT MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions);

  // Icosphere with constant color
  // @MeshT        The <Mesh> type to construct
  // @center       The center point of the shape
  // @radius       Radius of the sphere
  // @subdivisions Number of times to subdivide each face of the sphere. Higher->Smoother, but slower.
  // @color        Color to apply at each vertex
  template <typename MeshT>
  static MeshT MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions, Color color);

  // Basic UVSphere
  // @MeshT        The <Mesh> type to construct
  // @center       The center point of the shape
  // @radius       Radius of the sphere
  // @resolution   Number of vertices to generate around the major axis. Must be a positive, even number.
  template <typename MeshT>
  static MeshT MakeUVSphere(glm::vec3 center, double radius, int resolution);

  // UVSphere with constant color
  // @MeshT        The <Mesh> type to construct
  // @center       The center point of the shape
  // @radius       Radius of the sphere
  // @color        <Color> to apply at each vertex
  // @resolution   Number of vertices to generate around the major axis. Must be a positive, even number.
  template <typename MeshT>
  static MeshT MakeUVSphere(glm::vec3 center, double radius, Color color, int resolution);

  // Basic Capsule
  // @MeshT        The <Mesh> type to construct
  // @center1      Center of first lobe
  // @center2      Center of second lobe
  // @radius       Radius of the Capsule
  // @resolution   Number of vertices to generate around the major axis. Must be a positive, even number.
  template <typename MeshT>
  static MeshT MakeCapsule(glm::vec3 center1, glm::vec3 center2, double radius, int resolution);

  // Capsule with constant color
  // @MeshT        The <Mesh> type to construct
  // @center1      Center of first lobe
  // @center2      Center of second lobe
  // @radius       Radius of the Capsule
  // @color        <Color> to apply at each vertex
  // @resolution   Number of vertices to generate around the major axis. Must be a positive, even number.
  template <typename MeshT>
  static MeshT MakeCapsule(glm::vec3 center1, glm::vec3 center2, double radius, Color color, int resolution);

  // Constructs a mesh from raw data
  // @MeshT           The <Mesh> type to construct
  // @vertexPositions All vertex positions for the mesh
  // @indices         Indices into <vertexPositions> for constructing triangles
  // @normals         Normal vectors for each vertex in <vertexPositions>
  template <typename MeshT>
  static MeshT MakeTriangleMesh(Vector<glm::vec3> vertexPositions, Vector<GLuint> indices, Vector<glm::vec3> normals);

  // Triangle fan
  // @vertexPositions All vertex positions for the mesh. Ideally, these should describe a convex polygon.
  // @doubleSided     If True, triangles will be generated for both sides of the mesh
  //
  // Generates a triangle fan using the first vertex as the root. Vertex positions should be given in counter-clockwise order.
  // For best results, <vertexPositions> should describe a *convex* shape.
  // Normals for each triangle are automatically generated, and are uniform across each face.
  template <typename MeshT>
  static MeshT MakeTriangleFan(Vector<glm::vec3> vertexPositions, bool doubleSided = false);

  static LineMesh MakeLineMesh(const Vector<glm::vec3>& vertexPositions);

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
