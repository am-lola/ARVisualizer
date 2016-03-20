#ifndef _VERTEX_H
#define _VERTEX_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

struct VertexLine
{
  GLfloat position[3];
  GLfloat otherPosition[3];
  GLfloat otherDir;

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexLine),
                          (const GLvoid*)offsetof(VertexLine, position));
    glVertexAttribPointer(attribOffset + 1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexLine),
                          (const GLvoid*)offsetof(VertexLine, otherPosition));
    glVertexAttribPointer(attribOffset + 2, 1, GL_FLOAT, GL_FALSE,
                          sizeof(VertexLine),
                          (const GLvoid*)offsetof(VertexLine, otherDir));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    glEnableVertexAttribArray(attribOffset + 2);
    return 3;
  }
};

// A vertex with two position coordinates (XY)
struct VertexP2
{
  GLfloat position[2];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP2),
                         (const GLvoid*)offsetof(VertexP2, position));
    glEnableVertexAttribArray(attribOffset);
    return 1;
  }
};

// A vertex with two position coordinates and two texture coordinates (XYUV)
struct VertexP2T2
{
  GLfloat position[2];
  GLfloat uv[2];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP2T2),
                         (const GLvoid*)offsetof(VertexP2T2, position));
    glVertexAttribPointer(attribOffset + 1, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP2T2),
                         (const GLvoid*)offsetof(VertexP2T2, uv));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    return 2;
  }
};

// A vertex with three position coordinates (XYZ)
struct VertexP3
{
  GLfloat position[3];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3),
                         (const GLvoid*)offsetof(VertexP3, position));
    glEnableVertexAttribArray(attribOffset);
    return 1;
  }
};

// A vertex with four position coordinates (XYZW)
struct VertexP4
{
  GLfloat position[4];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP4),
                          (const GLvoid*)offsetof(VertexP4, position));
    glEnableVertexAttribArray(attribOffset);
    return 1;
  }
};

// A vertex with three position coordinates and two texture coords (XYZUV)
struct VertexP3T2
{
  GLfloat position[3];
  GLfloat uv[2];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3T2),
                         (const GLvoid*)offsetof(VertexP3T2, position));
    glVertexAttribPointer(attribOffset + 1, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3T2),
                         (const GLvoid*)offsetof(VertexP3T2, uv));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    return 2;
  }
};

// A vertex with three position coordinates and three color values (XYZRGB)
struct VertexP3C3
{
  GLfloat position[3];
  GLfloat color[3];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C3),
                         (const GLvoid*)offsetof(VertexP3C3, position));
    glVertexAttribPointer(attribOffset + 1, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C3),
                         (const GLvoid*)offsetof(VertexP3C3, color));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    return 2;
  }
};

// A vertex with three position coordinates and four color values (XYZRGBA)
struct VertexP3C4
{
  GLfloat position[3];
  GLfloat color[4];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C4),
                         (const GLvoid*)offsetof(VertexP3C4, position));
    glVertexAttribPointer(attribOffset + 1, 4, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C4),
                         (const GLvoid*)offsetof(VertexP3C4, color));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    return 2;
  }
};

struct VertexP3C4S
{
  GLfloat position[3];
  GLfloat color[4];
  GLfloat scale;

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3C4S),
                          (const GLvoid*)offsetof(VertexP3C4S, position));
    glVertexAttribPointer(attribOffset + 1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3C4S),
                          (const GLvoid*)offsetof(VertexP3C4S, color));
    glVertexAttribPointer(attribOffset + 2, 1, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3C4S),
                          (const GLvoid*)offsetof(VertexP3C4S, scale));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    glEnableVertexAttribArray(attribOffset + 2);
    return 3;
  }
};

// PCL-specific pcl::PointXYZRGBA format
struct Vertex_PCL_PointXYZRGBA
{
  GLfloat position[4];
  uint32_t color;
  uint32_t padding[3];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 4, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex_PCL_PointXYZRGBA),
                          (const GLvoid*)offsetof(Vertex_PCL_PointXYZRGBA, position));
    glVertexAttribPointer(attribOffset + 1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                          sizeof(Vertex_PCL_PointXYZRGBA),
                          (const GLvoid*)offsetof(Vertex_PCL_PointXYZRGBA, color));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    return 2;
  }
};

// A vertex with three position coordinates and three normal coordinates (XYZNNN)
struct VertexP3N3
{
  GLfloat position[3];
  GLfloat normal[3];

  static GLuint EnableVertexAttribArray(GLuint attribOffset = 0)
  {
    glVertexAttribPointer(attribOffset, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3N3),
                          (const GLvoid*)offsetof(VertexP3N3, position));
    glVertexAttribPointer(attribOffset + 1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3N3),
                          (const GLvoid*)offsetof(VertexP3N3, normal));
    glEnableVertexAttribArray(attribOffset);
    glEnableVertexAttribArray(attribOffset + 1);
    return 2;
  }
};

} // namespace ar

#endif // _VERTEX_H
