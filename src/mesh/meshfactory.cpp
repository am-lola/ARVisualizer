#include "meshfactory.hpp"
#include <vector>
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

Mesh<VertexP3C3> MeshFactory::MakeTriangle(std::vector<glm::vec3> vertexPositions, glm::vec3 color)
{
  std::vector<glm::vec3> vertexColors = { color, color, color };
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

Mesh<VertexP3C3> MeshFactory::MakeQuad(glm::vec3 center, glm::vec3 normal, double width, double height, glm::vec3 color)
{
  std::vector<glm::vec3> vertexColors = {color, color, color, color};
  return MakeQuad(center, normal, width, height, vertexColors);
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
