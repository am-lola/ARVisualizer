#include "meshfactory.hpp"
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace ar
{

Mesh<VertexP2> MeshFactory::MakeTriangle(std::vector<glm::vec2> vertexPositions)
{
  std::vector<VertexP2> verts;
  std::vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++)
  {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y});
  }

  return Mesh<VertexP2>(verts, indices);
}

Mesh<VertexP3> MeshFactory::MakeTriangle(std::vector<glm::vec3> vertexPositions)
{
  std::vector<VertexP3> verts;
  std::vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++)
  {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z});
  }

  return Mesh<VertexP3>(verts, indices);
}

Mesh<VertexP3C3> MeshFactory::MakeTriangle(std::vector<glm::vec3> vertexPositions, std::vector<glm::vec3> vertexColors)
{
  std::vector<VertexP3C3> verts;
  std::vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++) {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z,
                     vertexColors[i].r,    vertexColors[i].g,    vertexColors[i].b});
  }

  return Mesh<VertexP3C3>(verts, indices);
}

Mesh<VertexP3C4> MeshFactory::MakeTriangle(std::vector<glm::vec3> vertexPositions, std::vector<glm::vec4> vertexColors)
{
  std::vector<VertexP3C4> verts;
  std::vector<GLuint> indices = {0, 1, 2};
  for (size_t i = 0; i < 3; i++) {
    verts.push_back({vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z,
                     vertexColors[i].r,    vertexColors[i].g,    vertexColors[i].b, vertexColors[i].a});
  }

  return Mesh<VertexP3C4>(verts, indices);
}

Mesh<VertexP3C3> MeshFactory::MakeTriangle(std::vector<glm::vec3> vertexPositions, glm::vec3 color)
{
  std::vector<glm::vec3> vertexColors = { color, color, color };
  return MakeTriangle(vertexPositions, vertexColors);
}

Mesh<VertexP3C4> MeshFactory::MakeTriangle(std::vector<glm::vec3> vertexPositions, glm::vec4 color)
{
  std::vector<glm::vec4> vertexColors = { color, color, color };
  return MakeTriangle(vertexPositions, vertexColors);
}

Mesh<VertexP2> MeshFactory::MakeQuad(glm::vec2 center, double width, double height)
{
  float halfwidth = width / 2.0;
  float halfheight = height / 2.0;

  std::vector<VertexP2> verts = {
    { center.x - halfwidth, center.y - halfheight }, // top-left
    { center.x + halfwidth, center.y - halfheight }, // top-right
    { center.x - halfwidth, center.y + halfheight }, // bottom-left
    { center.x + halfwidth, center.y + halfheight }  // bottom-right
  };

  std::vector<GLuint> indices = {
    0, 1, 3,
    0, 2, 3
  };

  return Mesh<VertexP2>(verts, indices);
}

Mesh<VertexP3> MeshFactory::MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height)
{
  float halfwidth = width / 2.0;
  float halfheight = height / 2.0;

  // create quad at origin aligned with XY plane
  std::vector<VertexP3> verts  = {
    { -halfwidth, -halfheight, 0.0f }, // top-left
    {  halfwidth, -halfheight, 0.0f }, // top-right
    { -halfwidth,  halfheight, 0.0f }, // bottom-left
    {  halfwidth,  halfheight, 0.0f }  // bottom-right
  };

  std::vector<GLuint> indices = {
    0, 1, 3,
    0, 2, 3
  };

  Mesh<VertexP3> m = Mesh<VertexP3>(verts, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(0.0f, 0.0f, 1.0f), normal));

  return Mesh<VertexP3>(verts, indices);
}

Mesh<VertexP3C3> MeshFactory::MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, std::vector<glm::vec3> vertexColors)
{
  float halfwidth = width / 2.0;
  float halfheight = height / 2.0;

  // create quad at origin aligned with XY plane
  std::vector<VertexP3C3> verts  = {
    {{ -halfwidth,  halfheight, 0.0f }, { vertexColors[0].r, vertexColors[0].g, vertexColors[0].b }}, // top-left
    {{  halfwidth,  halfheight, 0.0f }, { vertexColors[1].r, vertexColors[1].g, vertexColors[1].b }}, // top-right
    {{ -halfwidth, -halfheight, 0.0f }, { vertexColors[2].r, vertexColors[2].g, vertexColors[2].b }}, // bottom-left
    {{  halfwidth, -halfheight, 0.0f }, { vertexColors[3].r, vertexColors[3].g, vertexColors[3].b }}, // bottom-right
  };

  std::vector<GLuint> indices = {
    0, 1, 3,
    0, 2, 3
  };

  // make mesh
  Mesh<VertexP3C3> m = Mesh<VertexP3C3>(verts, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(0.0f, 0.0f, 1.0f), normal));

  return m;
}

Mesh<VertexP3C4> MeshFactory::MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, std::vector<glm::vec4> vertexColors)
{
  float halfwidth = width / 2.0;
  float halfheight = height / 2.0;

  // create quad at origin aligned with XY plane
  std::vector<VertexP3C4> verts  = {
    {{ -halfwidth,  halfheight, 0.0f }, { vertexColors[0].r, vertexColors[0].g, vertexColors[0].b, vertexColors[0].a }}, // top-left
    {{  halfwidth,  halfheight, 0.0f }, { vertexColors[1].r, vertexColors[1].g, vertexColors[1].b, vertexColors[1].a }}, // top-right
    {{ -halfwidth, -halfheight, 0.0f }, { vertexColors[2].r, vertexColors[2].g, vertexColors[2].b, vertexColors[2].a }}, // bottom-left
    {{  halfwidth, -halfheight, 0.0f }, { vertexColors[3].r, vertexColors[3].g, vertexColors[3].b, vertexColors[3].a }}, // bottom-right
  };

  std::vector<GLuint> indices = {
    0, 1, 3,
    0, 2, 3
  };

  // make mesh
  Mesh<VertexP3C4> m = Mesh<VertexP3C4>(verts, indices);
  m.SetTransform(MakeTransform(center, glm::vec3(0.0f, 0.0f, 1.0f), normal));

  return m;
}

Mesh<VertexP3C3> MeshFactory::MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, glm::vec3 color)
{
  std::vector<glm::vec3> vertexColors = {color, color, color};
  return MakeQuad(center, normal, width, height, vertexColors);
}

Mesh<VertexP3C4> MeshFactory::MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, glm::vec4 color)
{
  std::vector<glm::vec4> vertexColors = {color, color, color, color};
  return MakeQuad(center, normal, width, height, vertexColors);
}

Mesh<VertexP3C4> MeshFactory::MakeIcosphere(glm::vec3 center, double radius, unsigned int subdivisions, glm::vec4 color)
{
  std::vector<glm::vec3> vertex_positions;
  std::vector<VertexP3C4> vertices;
  std::vector<GLuint> indices;

  // create a sphere centered at origin, and assign it a transform translating it to center
  // Adapted from: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
  int index = 0;

  // create the 12 vertices of an icosahedron
  double t = (1.0 + glm::sqrt(5.0f)) / 2.0;

  vertex_positions.push_back(glm::vec3( -1.0f,  t, 0.0f ));
  vertex_positions.push_back(glm::vec3(  1.0f,  t, 0.0f ));
  vertex_positions.push_back(glm::vec3( -1.0f, -t, 0.0f ));
  vertex_positions.push_back(glm::vec3(  1.0f, -t, 0.0f ));

  vertex_positions.push_back(glm::vec3( 0.0f, -1.0f,  t ));
  vertex_positions.push_back(glm::vec3( 0.0f,  1.0f,  t ));
  vertex_positions.push_back(glm::vec3( 0.0f, -1.0f, -t ));
  vertex_positions.push_back(glm::vec3( 0.0f,  1.0f, -t ));

  vertex_positions.push_back(glm::vec3(  t, 0.0f, -1.0f ));
  vertex_positions.push_back(glm::vec3(  t, 0.0f,  1.0f ));
  vertex_positions.push_back(glm::vec3( -t, 0.0f, -1.0f ));
  vertex_positions.push_back(glm::vec3( -t, 0.0f,  1.0f ));

  // create the faces of the icosahedron (each line of 3 indices corresponds to one tri)
  indices.push_back( 0); indices.push_back(11); indices.push_back( 5);
  indices.push_back( 0); indices.push_back( 5); indices.push_back( 1);
  indices.push_back( 0); indices.push_back( 1); indices.push_back( 7);
  indices.push_back( 0); indices.push_back( 7); indices.push_back(10);
  indices.push_back( 0); indices.push_back(10); indices.push_back(11);

  indices.push_back( 1); indices.push_back( 5); indices.push_back( 9);
  indices.push_back( 5); indices.push_back(11); indices.push_back( 4);
  indices.push_back(11); indices.push_back(10); indices.push_back( 2);
  indices.push_back(10); indices.push_back( 7); indices.push_back( 6);
  indices.push_back( 7); indices.push_back( 1); indices.push_back( 8);

  indices.push_back( 3); indices.push_back( 9); indices.push_back( 4);
  indices.push_back( 3); indices.push_back( 4); indices.push_back( 2);
  indices.push_back( 3); indices.push_back( 2); indices.push_back( 6);
  indices.push_back( 3); indices.push_back( 6); indices.push_back( 8);
  indices.push_back( 3); indices.push_back( 8); indices.push_back( 9);

  indices.push_back( 4); indices.push_back( 9); indices.push_back( 5);
  indices.push_back( 2); indices.push_back( 4); indices.push_back(11);
  indices.push_back( 6); indices.push_back( 2); indices.push_back(10);
  indices.push_back( 8); indices.push_back( 6); indices.push_back( 7);
  indices.push_back( 9); indices.push_back( 8); indices.push_back( 1);

  // refine faces
  for (unsigned int i = 0; i < subdivisions; i++)
  {
    std::vector<GLuint> newIndices; // new index list for all vertices in next subdiv level
    for (unsigned int j = 0; j < indices.size(); j += 3)
    {
      // find midpoint between each pair of vertices for current faces
      int mid1, mid2, mid3; // midpoint indices
      glm::vec3 midpoint, p1, p2; // midpoint position, positions of endpoints

      /** mid1 = indexof( midpoint(vertices[j+0], vertices[j+1]) ) **/
      p1 = vertex_positions[indices[j+0]];
      p2 = vertex_positions[indices[j+1]];
      midpoint = glm::vec3(
        (p1.x + p2.x) / 2.0f,
        (p1.y + p2.y) / 2.0f,
        (p1.z + p2.z) / 2.0f
      );
      // if midpoint is new, add it to vertex_positions and give mid1 the new position
      auto v1 = std::find(vertex_positions.begin(), vertex_positions.end(), midpoint);
      if (v1 == vertex_positions.end())
      {
        mid1 = vertex_positions.size();
        vertex_positions.push_back(midpoint);
      }
      else // if midpoint already exists, just give mid1 the existing index
      {
        mid1 = v1 - vertex_positions.begin();
      }

      /** mid2 = indexof( midpoint(vertices[j+1], vertices[j+2]) ) **/
      p1 = vertex_positions[indices[j+1]];
      p2 = vertex_positions[indices[j+2]];
      midpoint = glm::vec3(
        (p1.x + p2.x) / 2.0f,
        (p1.y + p2.y) / 2.0f,
        (p1.z + p2.z) / 2.0f
      );
      // if midpoint is new, add it to vertex_positions and give mid1 the new position
      auto v2 = std::find(vertex_positions.begin(), vertex_positions.end(), midpoint);
      if (v2 == vertex_positions.end())
      {
        mid2 = vertex_positions.size();
        vertex_positions.push_back(midpoint);
      }
      else // if midpoint already exists, just give mid1 the existing index
      {
        mid2 = v2 - vertex_positions.begin();
      }

      /** mid3 = indexof( midpoint(vertices[j+2], vertices[j+0]) ) **/
      p1 = vertex_positions[indices[j+2]];
      p2 = vertex_positions[indices[j+0]];
      midpoint = glm::vec3(
        (p1.x + p2.x) / 2.0f,
        (p1.y + p2.y) / 2.0f,
        (p1.z + p2.z) / 2.0f
      );
      // if midpoint is new, add it to vertex_positions and give mid1 the new position
      auto v3 = std::find(vertex_positions.begin(), vertex_positions.end(), midpoint);
      if (v3 == vertex_positions.end())
      {
        mid3 = vertex_positions.size();
        vertex_positions.push_back(midpoint);
      }
      else // if midpoint already exists, just give mid1 the existing index
      {
        mid3 = v3 - vertex_positions.begin();
      }

      /** Create new triangles from the midpoints **/
      newIndices.push_back(indices[j+0]); newIndices.push_back(mid1); newIndices.push_back(mid3);
      newIndices.push_back(indices[j+1]); newIndices.push_back(mid2); newIndices.push_back(mid1);
      newIndices.push_back(indices[j+2]); newIndices.push_back(mid3); newIndices.push_back(mid2);
      newIndices.push_back(mid1);         newIndices.push_back(mid2); newIndices.push_back(mid3);
    }

    // replace indices from previous subdivision level with new ones
    indices.clear();
    for (auto idx : newIndices)
    {
      indices.push_back(idx);
    }
  }

  // assemble vertex data
  for (auto v : vertex_positions)
  {
    v = glm::normalize(v) * (float)radius;
    vertices.push_back({
      { v.x, v.y, v.z },
      { color.r, color.g, color.b, color.a }
    });
  }

  Mesh<VertexP3C4> m = Mesh<VertexP3C4>(vertices, indices);
  m.SetTransform(glm::translate(glm::mat4(1.0f), center));
  return m;
}

glm::mat4 MeshFactory::MakeTransform(glm::vec3 offset, glm::vec3 from_rotation, glm::vec3 to_rotation)
{
  float eps = 0.001f; // tolerance for detecting vectors which are nearly opposite
  // create transform from default position/orientation to actual position/orientation
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
      rot_angle = 3.1415f;
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

} // namespace ar
