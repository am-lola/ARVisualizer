#include "meshfactory.hpp"
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
namespace ar
{

template <>
Mesh<VertexP2> MeshFactory::MakeTriangle<Mesh<VertexP2>>(Vector<glm::vec2> vertexPositions)
{
  Vector<VertexP2> verts;
  Vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++)
  {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y});
  }

  return Mesh<VertexP2>(verts, indices);
}

template <>
Mesh<VertexP3> MeshFactory::MakeTriangle<Mesh<VertexP3>>(Vector<glm::vec3> vertexPositions)
{
  Vector<VertexP3> verts;
  Vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++)
  {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z});
  }

  return Mesh<VertexP3>(verts, indices);
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeTriangle<Mesh<VertexP3C4>>(Vector<glm::vec3> vertexPositions, Vector<Color> vertexColors)
{
  Vector<VertexP3C4> verts;
  Vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++) {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z,
                     vertexColors[i].r,    vertexColors[i].g,    vertexColors[i].b, vertexColors[i].a});
  }

  return Mesh<VertexP3C4>(verts, indices);
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeTriangle<Mesh<VertexP3C4>>(Vector<glm::vec3> vertexPositions, Color color)
{
  Vector<Color> vertexColors = { color, color, color };
  return MakeTriangle<Mesh<VertexP3C4>>(vertexPositions, vertexColors);
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeTriangle<Mesh<VertexP3N3>>(Vector<glm::vec3> vertexPositions)
{
  Vector<VertexP3N3> verts;
  Vector<GLuint> indices = {0, 1, 2};
  glm::vec3 normal = glm::normalize(glm::cross( vertexPositions[1] - vertexPositions[0],
                                                vertexPositions[2] - vertexPositions[0]
                                              ));

  for (size_t i = 0; i < 3; i++)
  {
    verts.push_back({
      {vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z},
      {normal.x, normal.y, normal.z}
    });
  }

  return Mesh<VertexP3N3>(verts, indices);
}

template <>
Mesh<VertexP2> MeshFactory::MakeQuad<Mesh<VertexP2>>(glm::vec2 center, double width, double height)
{
  Vector<glm::vec3> vertex_positions;
  Vector<GLuint> indices;
  Vector<VertexP2> verts;

  MakeQuadMesh(width, height, &vertex_positions, &indices);

  for (auto v : vertex_positions)
  {
    verts.push_back( {v.x, v.y} );
  }

  return Mesh<VertexP2>(verts, indices);
}

template <>
TexturedMesh<VertexP2T2> MeshFactory::MakeQuad<TexturedMesh<VertexP2T2>>(glm::vec2 center, double width, double height)
{
  Vector<glm::vec3> vertex_positions;
  Vector<GLuint> indices;
  Vector<VertexP2T2> verts;
  Vector<Vector<float>> UVs =  {
                      {0.0f, 1.0f},
                      {1.0f, 1.0f},
                      {0.0f, 0.0f},
                      {1.0f, 0.0f}
  };

  MakeQuadMesh(width, height, &vertex_positions, &indices);

  float tu, tv = 0;
  for (int i = 0; i < vertex_positions.size(); i++)
  {
    verts.push_back({
      {vertex_positions[i].x, vertex_positions[i].y},
      {UVs[i][0], UVs[i][1]}
    });
  }

  return TexturedMesh<VertexP2T2>(verts, indices);
}

template <>
Mesh<VertexP3> MeshFactory::MakeQuad<Mesh<VertexP3>>(glm::vec3 center, glm::vec3 normal, double width, double height)
{
  Vector<glm::vec3> vertex_positions;
  Vector<GLuint> indices;
  Vector<VertexP3> verts;

  MakeQuadMesh(width, height, &vertex_positions, &indices);

  for (auto v : vertex_positions)
  {
    verts.push_back( {v.x, v.y, v.z} );
  }

  Mesh<VertexP3> m = Mesh<VertexP3>(verts, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(0.0f, 0.0f, 1.0f), normal));

  return Mesh<VertexP3>(verts, indices);
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeQuad<Mesh<VertexP3C4>>(glm::vec3 center, glm::vec3 normal, double width, double height, Vector<Color> vertexColors)
{
  Vector<glm::vec3> vertex_positions;
  Vector<GLuint> indices;
  Vector<VertexP3C4> verts;

  MakeQuadMesh(width, height, &vertex_positions, &indices);

  for (int i = 0; i < vertex_positions.size(); i++)
  {
    verts.push_back({
      { vertex_positions[i].x, vertex_positions[i].y, vertex_positions[i].z },
      { vertexColors[i].r, vertexColors[i].g, vertexColors[i].b, vertexColors[i].a }
    });
  }

  Mesh<VertexP3C4> m = Mesh<VertexP3C4>(verts, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(0.0f, 0.0f, 1.0f), normal));

  return m;
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeQuad<Mesh<VertexP3C4>>(glm::vec3 center, glm::vec3 normal, double width, double height, Color color)
{
  Vector<Color> vertexColors = {color, color, color, color};
  return MakeQuad<Mesh<VertexP3C4>>(center, normal, width, height, vertexColors);
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeQuad<Mesh<VertexP3N3>>(glm::vec3 center, glm::vec3 normal, double width, double height)
{
  Vector<glm::vec3> vertex_positions;
  Vector<GLuint> indices;
  Vector<VertexP3N3> verts;
  glm::vec3 n_normal = glm::normalize(normal); // make sure normal is a unit vector

  MakeQuadMesh(width, height, &vertex_positions, &indices);

  for (auto v : vertex_positions)
  {
    verts.push_back({
      { v.x, v.y, v.z },
      { n_normal.x, n_normal.y, n_normal.z }
    });
  }

  Mesh<VertexP3N3> m = Mesh<VertexP3N3>(verts, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(0.0f, 0.0f, 1.0f), n_normal));

  return m;
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeBox(glm::vec3 center, double xLength, double yLength, double zLength)
{
  Vector<glm::vec3> vertex_positions;
  Vector<GLuint> indices;
  Vector<VertexP3N3> verts;
  Vector<glm::vec3> normals
  {
    {  0,  0,  1 },
    {  0,  0, -1 },
    {  0,  1,  0 },
    {  0, -1,  0 },
    {  1,  0,  0 },
    { -1,  0,  0 },
  };

  MakeBoxMesh(xLength, yLength, zLength, &vertex_positions, &indices);

  for (int i = 0; i < normals.size(); i++)
  {
    for (int j = 0; j < 4; j++) // four vertices per face in the cube
    {
      verts.push_back({
        { vertex_positions[4*i + j].x, vertex_positions[4*i + j].y, vertex_positions[4*i + j].z },
        { normals[i].x, normals[i].y, normals[i].z }
      });
    }
  }

  Mesh<VertexP3N3> m = Mesh<VertexP3N3>(verts, indices);

  // assign it a transform translating it to center
  m.SetTransform(glm::translate(glm::mat4(1.0f), center));
  return m;
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeCube(glm::vec3 center, double size)
{
  return MakeBox<Mesh<VertexP3N3>>(center, size, size, size);
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeIcosphere<Mesh<VertexP3C4>>(glm::vec3 center, double radius, unsigned int subdivisions, Color color)
{
  Vector<glm::vec3> vertex_positions;
  Vector<VertexP3C4> vertices;
  Vector<GLuint> indices;

  // create a sphere centered at origin
  MakeIcosphereMesh(subdivisions, &vertex_positions, &indices);

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    v = glm::normalize(v) * (float)radius; // normalize all vertices to the given radius
    vertices.push_back({
      { v.x, v.y, v.z },
      { color.r, color.g, color.b, color.a }
    });
  }

  Mesh<VertexP3C4> m = Mesh<VertexP3C4>(vertices, indices);

  // assign it a transform translating it to center
  m.SetTransform(glm::translate(glm::mat4(1.0f), center));
  return m;
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeIcosphere<Mesh<VertexP3N3>>(glm::vec3 center, double radius, unsigned int subdivisions)
{
  Vector<glm::vec3> vertex_positions;
  Vector<VertexP3N3> vertices;
  Vector<GLuint> indices;

  // create a sphere centered at origin
  MakeIcosphereMesh(subdivisions, &vertex_positions, &indices);

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    v = glm::normalize(v) * (float)radius; // normalize all vertices to the given radius
    glm::vec3 n = glm::normalize(v);

    vertices.push_back({
      { v.x, v.y, v.z },
      { n.x, n.y, n.z }
    });
  }

  Mesh<VertexP3N3> m = Mesh<VertexP3N3>(vertices, indices);

  // assign it a transform translating it to center
  m.SetTransform(glm::translate(glm::mat4(1.0f), center));
  return m;
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeUVSphere<Mesh<VertexP3C4>>(glm::vec3 center, double radius, Color color, int resolution)
{
  Vector<glm::vec3> vertex_positions;
  Vector<VertexP3C4> vertices;
  Vector<GLuint> indices;

  // Create a sphere centered at the origin
  MakeUVSphereMesh(resolution, &vertex_positions, &indices);

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    v = glm::normalize(v) * (float)radius; // normalize all vertices to the given radius
    vertices.push_back({
      { v.x, v.y, v.z },
      { color.r, color.g, color.b, color.a }
    });
  }

  Mesh<VertexP3C4> m = Mesh<VertexP3C4>(vertices, indices);
  m.SetTransform(glm::translate(glm::mat4(1.0f), center));
  return m;
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeUVSphere<Mesh<VertexP3N3>>(glm::vec3 center, double radius, int resolution)
{
  Vector<glm::vec3> vertex_positions;
  Vector<VertexP3N3> vertices;
  Vector<GLuint> indices;

  // Create a sphere centered at the origin
  MakeUVSphereMesh(resolution, &vertex_positions, &indices);

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    glm::vec3 n = glm::normalize(v); // normal at a point is just the vector to that point
    v = glm::normalize(v) * (float)radius; // normalize all vertices to the given radius
    vertices.push_back({
      { v.x, v.y, v.z },
      { n.x, n.y, n.z }
    });
  }

  Mesh<VertexP3N3> m = Mesh<VertexP3N3>(vertices, indices);
  m.SetTransform(glm::translate(glm::mat4(1.0f), center));
  return m;
}

template <>
Mesh<VertexP3C4> MeshFactory::MakeCapsule<Mesh<VertexP3C4>>(glm::vec3 center1, glm::vec3 center2, double radius, Color color, int resolution)
{
  Vector<glm::vec3> vertex_positions;
  Vector<VertexP3C4> vertices;
  Vector<GLuint> indices;

  /// The capsule is generated as two halves of a UVSphere, with each half being
  /// offset along the polar axis by half the distance between center1 & center2
  /// We then generate a transform to translate & rotate the resulting shape into
  /// the correct position.

  glm::vec3 axis = glm::normalize(center2 - center1);    // axis capsule aligns to
  float distance = glm::length(center2 - center1);       // distance between centers
  glm::vec3 center = center1 + (distance / 2.0f) * axis; // midpoint between centers ("location" of resulting mesh)

  // Create capsule at origin
  MakeCapsuleMesh(distance, radius, resolution, &vertex_positions, &indices);

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    vertices.push_back({
      { v.x, v.y, v.z },
      { color.r, color.g, color.b, color.a }
    });
  }

  Mesh<VertexP3C4> m = Mesh<VertexP3C4>(vertices, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(1,0,0), axis)); // from_rotation is (1,0,0) because our capsule is generated along the x-axis
  return m;
}

template <>
Mesh<VertexP3N3> MeshFactory::MakeCapsule<Mesh<VertexP3N3>>(glm::vec3 center1, glm::vec3 center2, double radius, int resolution)
{
  Vector<glm::vec3> vertex_positions;
  Vector<VertexP3N3> vertices;
  Vector<GLuint> indices;

  /// The capsule is generated as two halves of a UVSphere, with each half being
  /// offset along the polar axis by half the distance between center1 & center2
  /// We then generate a transform to translate & rotate the resulting shape into
  /// the correct position.

  glm::vec3 axis = glm::normalize(center2 - center1);    // axis capsule aligns to
  float distance = glm::length(center2 - center1);       // distance between centers
  float halfd = distance / 2;
  glm::vec3 center = center1 + (distance / 2.0f) * axis; // midpoint between centers ("location" of resulting mesh)

  // Create capsule at origin
  MakeCapsuleMesh(distance, radius, resolution, &vertex_positions, &indices);

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    glm::vec3 offset = v.x > 0 ? glm::vec3(-halfd, 0, 0) : glm::vec3(halfd, 0, 0);
    glm::vec3 n = glm::normalize(v + offset);
    vertices.push_back({
      { v.x, v.y, v.z },
      { n.x, n.y, n.z }
    });
  }

  Mesh<VertexP3N3> m = Mesh<VertexP3N3>(vertices, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(1,0,0), axis)); // from_rotation is (1,0,0) because our capsule is generated along the x-axis
  return m;
}

glm::mat4 MeshFactory::MakeTransform(glm::vec3 offset, glm::vec3 from_rotation, glm::vec3 to_rotation)
{
  float eps = 0.001f; // tolerance for detecting vectors which are nearly opposite
  glm::vec3 source_vec = glm::normalize(from_rotation);
  glm::vec3 target_vec = glm::normalize(to_rotation);

  float rot_angle = 0.0f;
  glm::vec3 rot_axis;
  glm::mat4 rot_mat = glm::mat4(1.0f);
  glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), offset);

  float d = glm::dot(source_vec, target_vec);
  // if dot product is 1, vectors are the same; we only rotate if they're different
  if (d < 1.0f)
  {
    // if the vectors are opposite, rotate 180 degrees
    if (d < -1.0f + eps)
    {
      // guess to find a good axis for rotation
      rot_axis = glm::cross(source_vec, glm::vec3(1.0f, 0.0f, 0.0f));
      if (glm::length(rot_axis) == 0) // try another axis if that one failed
      {
        rot_axis = glm::cross(source_vec, glm::vec3(0.0f, 1.0f, 0.0f));
      }
      rot_axis = glm::normalize(rot_axis);
      rot_angle = glm::pi<float>();
    }
    else
    {
      rot_axis = glm::normalize(glm::cross(source_vec, target_vec));
      rot_angle = glm::acos(d);
    }

    // final rotation from from_rotation to to_rotation
    rot_mat = glm::rotate(rot_mat, rot_angle, rot_axis);
  }

  return trans_mat * rot_mat;
}

void MeshFactory::MakeQuadMesh(double width, double height, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices)
{
  float halfwidth = width / 2.0;
  float halfheight = height / 2.0;

  // create quad at origin aligned with XY plane
  *vertex_positions  = {
    { -halfwidth, -halfheight, 0.0f }, // top-left
    {  halfwidth, -halfheight, 0.0f }, // top-right
    { -halfwidth,  halfheight, 0.0f }, // bottom-left
    {  halfwidth,  halfheight, 0.0f }  // bottom-right
  };

  *indices = {
    0, 1, 2,  // first triangle
    1, 3, 2   // second triangle
  };
}

// Adapted from: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
void MeshFactory::MakeIcosphereMesh(unsigned int subdivisions, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices)
{
  Vector<glm::vec3>& r_positions = *vertex_positions;
  Vector<GLuint>& r_indices = *indices;

  // create the 12 vertices of an icosahedron
  double t = (1.0 + glm::sqrt(5.0f)) / 2.0;

  r_positions.push_back(glm::vec3( -1.0f,  t, 0.0f ));
  r_positions.push_back(glm::vec3(  1.0f,  t, 0.0f ));
  r_positions.push_back(glm::vec3( -1.0f, -t, 0.0f ));
  r_positions.push_back(glm::vec3(  1.0f, -t, 0.0f ));

  r_positions.push_back(glm::vec3( 0.0f, -1.0f,  t ));
  r_positions.push_back(glm::vec3( 0.0f,  1.0f,  t ));
  r_positions.push_back(glm::vec3( 0.0f, -1.0f, -t ));
  r_positions.push_back(glm::vec3( 0.0f,  1.0f, -t ));

  r_positions.push_back(glm::vec3(  t, 0.0f, -1.0f ));
  r_positions.push_back(glm::vec3(  t, 0.0f,  1.0f ));
  r_positions.push_back(glm::vec3( -t, 0.0f, -1.0f ));
  r_positions.push_back(glm::vec3( -t, 0.0f,  1.0f ));

  // create the faces of the icosahedron (each line of 3 indices corresponds to one tri)
  r_indices.push_back( 0); r_indices.push_back(11); r_indices.push_back( 5);
  r_indices.push_back( 0); r_indices.push_back( 5); r_indices.push_back( 1);
  r_indices.push_back( 0); r_indices.push_back( 1); r_indices.push_back( 7);
  r_indices.push_back( 0); r_indices.push_back( 7); r_indices.push_back(10);
  r_indices.push_back( 0); r_indices.push_back(10); r_indices.push_back(11);

  r_indices.push_back( 1); r_indices.push_back( 5); r_indices.push_back( 9);
  r_indices.push_back( 5); r_indices.push_back(11); r_indices.push_back( 4);
  r_indices.push_back(11); r_indices.push_back(10); r_indices.push_back( 2);
  r_indices.push_back(10); r_indices.push_back( 7); r_indices.push_back( 6);
  r_indices.push_back( 7); r_indices.push_back( 1); r_indices.push_back( 8);

  r_indices.push_back( 3); r_indices.push_back( 9); r_indices.push_back( 4);
  r_indices.push_back( 3); r_indices.push_back( 4); r_indices.push_back( 2);
  r_indices.push_back( 3); r_indices.push_back( 2); r_indices.push_back( 6);
  r_indices.push_back( 3); r_indices.push_back( 6); r_indices.push_back( 8);
  r_indices.push_back( 3); r_indices.push_back( 8); r_indices.push_back( 9);

  r_indices.push_back( 4); r_indices.push_back( 9); r_indices.push_back( 5);
  r_indices.push_back( 2); r_indices.push_back( 4); r_indices.push_back(11);
  r_indices.push_back( 6); r_indices.push_back( 2); r_indices.push_back(10);
  r_indices.push_back( 8); r_indices.push_back( 6); r_indices.push_back( 7);
  r_indices.push_back( 9); r_indices.push_back( 8); r_indices.push_back( 1);

  // refine faces
  for (unsigned int i = 0; i < subdivisions; i++)
  {
    Vector<GLuint> newIndices; // new index list for all vertices in next subdiv level
    for (unsigned int j = 0; j < r_indices.size(); j += 3)
    {
      // find midpoint between each pair of vertices for current faces
      int mid1, mid2, mid3; // midpoint indices
      glm::vec3 midpoint, p1, p2; // midpoint position, positions of endpoints

      /** mid1 = indexof( midpoint(vertices[j+0], vertices[j+1]) ) **/
      p1 = r_positions[r_indices[j+0]];
      p2 = r_positions[r_indices[j+1]];
      midpoint = glm::vec3(
        (p1.x + p2.x) / 2.0f,
        (p1.y + p2.y) / 2.0f,
        (p1.z + p2.z) / 2.0f
      );
      // if midpoint is new, add it to vertex_positions and give mid1 the new position
      auto v1 = std::find(r_positions.begin(), r_positions.end(), midpoint);
      if (v1 == r_positions.end())
      {
        mid1 = r_positions.size();
        r_positions.push_back(midpoint);
      }
      else // if midpoint already exists, just give mid1 the existing index
      {
        mid1 = v1 - r_positions.begin();
      }

      /** mid2 = indexof( midpoint(vertices[j+1], vertices[j+2]) ) **/
      p1 = r_positions[r_indices[j+1]];
      p2 = r_positions[r_indices[j+2]];
      midpoint = glm::vec3(
        (p1.x + p2.x) / 2.0f,
        (p1.y + p2.y) / 2.0f,
        (p1.z + p2.z) / 2.0f
      );
      // if midpoint is new, add it to vertex_positions and give mid1 the new position
      auto v2 = std::find(r_positions.begin(), r_positions.end(), midpoint);
      if (v2 == r_positions.end())
      {
        mid2 = r_positions.size();
        r_positions.push_back(midpoint);
      }
      else // if midpoint already exists, just give mid2 the existing index
      {
        mid2 = v2 - r_positions.begin();
      }

      /** mid3 = indexof( midpoint(vertices[j+2], vertices[j+0]) ) **/
      p1 = r_positions[r_indices[j+2]];
      p2 = r_positions[r_indices[j+0]];
      midpoint = glm::vec3(
        (p1.x + p2.x) / 2.0f,
        (p1.y + p2.y) / 2.0f,
        (p1.z + p2.z) / 2.0f
      );
      // if midpoint is new, add it to vertex_positions and give mid1 the new position
      auto v3 = std::find(r_positions.begin(), r_positions.end(), midpoint);
      if (v3 == r_positions.end())
      {
        mid3 = r_positions.size();
        r_positions.push_back(midpoint);
      }
      else // if midpoint already exists, just give mid3 the existing index
      {
        mid3 = v3 - r_positions.begin();
      }

      /** Create new triangles from the midpoints **/
      newIndices.push_back(r_indices[j+0]); newIndices.push_back(mid1); newIndices.push_back(mid3);
      newIndices.push_back(r_indices[j+1]); newIndices.push_back(mid2); newIndices.push_back(mid1);
      newIndices.push_back(r_indices[j+2]); newIndices.push_back(mid3); newIndices.push_back(mid2);
      newIndices.push_back(mid1);           newIndices.push_back(mid2); newIndices.push_back(mid3);
    }

    // replace indices from previous subdivision level with new ones
    r_indices.clear();
    for (auto idx : newIndices)
    {
      r_indices.push_back(idx);
    }
  }
}

void MeshFactory::MakeUVSphereMesh(unsigned int resolution, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices)
{
  Vector<glm::vec3>& r_positions = *vertex_positions;
  Vector<GLuint>& r_indices = *indices;

  float dx = 1.0 / (float)(resolution - 1); // distance between vertices
  float dy = dx;

  for (int i = 0; i < resolution; i++)
  {
    for (int j = 0; j < resolution; j++)
    {
      double x = glm::sin( - glm::half_pi<float>() + glm::pi<float>() * i * dx);
      double y = glm::cos( 2.0f * glm::pi<float>() * j * dy) * glm::sin( glm::pi<float>() * i * dx );
      double z = glm::sin( 2.0f * glm::pi<float>() * j * dy) * glm::sin( glm::pi<float>() * i * dx );

      r_positions.push_back(glm::vec3((float)x, (float)y, (float)z));
    }
  }

  // generate a quad (as two tris) for each face on the sphere
  for (int i = 0; i < resolution-1; i++)
  {
    for (int j = 0; j < resolution-1; j++)
    {
      // First triangle
      r_indices.push_back(i * resolution + j);         // bottom-left
      r_indices.push_back((i+1) * resolution + (j+1)); // top-right
      r_indices.push_back((i+1) * resolution + j);     // top-left

      // Second triangle
      r_indices.push_back(i * resolution + j);         // bottom-left
      r_indices.push_back(i * resolution + (j + 1));   // bottom-right
      r_indices.push_back((i+1) * resolution + (j+1)); // top-right
    }
  }
}

void MeshFactory::MakeCapsuleMesh(double length, double radius, unsigned int resolution, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices)
{
  Vector<glm::vec3>& r_positions = *vertex_positions;
  Vector<GLuint>& r_indices = *indices;

  float dx = 1.0 / (float)(resolution - 1); // distance between vertices
  float dy = dx;

  // first half -- centered at (-length/2,0,0)
  for (int i = 0; i < resolution / 2; i++)
  {
    for (int j = 0; j < resolution; j++)
    {
      double x = glm::sin( - glm::half_pi<float>() + glm::pi<float>() * i * dx);
      double y = glm::cos( 2.0f * glm::pi<float>() * j * dy) * glm::sin( glm::pi<float>() * i * dx );
      double z = glm::sin( 2.0f * glm::pi<float>() * j * dy) * glm::sin( glm::pi<float>() * i * dx );

      glm::vec3 newVert = (float)radius * glm::vec3((float)x, (float)y, (float)z);
      newVert.x -= (length / 2.0f);

      r_positions.push_back(newVert);
    }
  }

  // second half -- centered at (+length/2,0,0)
  for (int i = resolution / 2; i < resolution; i++)
  {
    for (int j = 0; j < resolution; j++)
    {
      double x = glm::sin( - glm::half_pi<float>() + glm::pi<float>() * i * dx);
      double y = glm::cos( 2.0f * glm::pi<float>() * j * dy) * glm::sin( glm::pi<float>() * i * dx );
      double z = glm::sin( 2.0f * glm::pi<float>() * j * dy) * glm::sin( glm::pi<float>() * i * dx );

      glm::vec3 newVert = (float)radius * glm::vec3((float)x, (float)y, (float)z);
      newVert.x += (length / 2.0f);

      r_positions.push_back(newVert);
    }
  }

  // generate a quad (as two tris) for each face (done as if this were a regular sphere)
  for (int i = 0; i < resolution-1; i++)
  {
    for (int j = 0; j < resolution-1; j++)
    {
      // First triangle
      r_indices.push_back(i * resolution + j);         // bottom-left
      r_indices.push_back((i+1) * resolution + (j+1)); // top-right
      r_indices.push_back((i+1) * resolution + j);     // top-left

      // Second triangle
      r_indices.push_back(i * resolution + j);         // bottom-left
      r_indices.push_back(i * resolution + (j + 1));   // bottom-right
      r_indices.push_back((i+1) * resolution + (j+1)); // top-right
    }
  }
}

void MeshFactory::MakeBoxMesh(double xLength, double yLength, double zLength, Vector<glm::vec3>* vertex_positions, Vector<GLuint>* indices)
{
  double halfX = xLength / 2.0;
  double halfY = yLength / 2.0;
  double halfZ = zLength / 2.0;

  *vertex_positions = {
    { -halfX, -halfY,  halfZ }, {  halfX, -halfY,  halfZ },
    {  halfX,  halfY,  halfZ }, { -halfX,  halfY,  halfZ },
    { -halfX, -halfY, -halfZ }, { -halfX,  halfY, -halfZ },
    {  halfX,  halfY, -halfZ }, {  halfX, -halfY, -halfZ },
    { -halfX,  halfY, -halfZ }, { -halfX,  halfY,  halfZ },
    {  halfX,  halfY,  halfZ }, {  halfX,  halfY, -halfZ },
    { -halfX, -halfY, -halfZ }, {  halfX, -halfY, -halfZ },
    {  halfX, -halfY,  halfZ }, { -halfX, -halfY,  halfZ },
    {  halfX, -halfY, -halfZ }, {  halfX,  halfY, -halfZ },
    {  halfX,  halfY,  halfZ }, {  halfX, -halfY,  halfZ },
    { -halfX, -halfY, -halfZ }, { -halfX, -halfY,  halfZ },
    { -halfX,  halfY,  halfZ }, { -halfX,  halfY, -halfZ }
  };

  *indices = {
     0,  1,  2,      0,  2,  3,
     4,  5,  6,      4,  6,  7,
     8,  9, 10,      8, 10, 11,
    12, 13, 14,     12, 14, 15,
    16, 17, 18,     16, 18, 19,
    20, 21, 22,     20, 22, 23
  };
}

} // namespace ar
