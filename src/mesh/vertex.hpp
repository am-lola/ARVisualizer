#ifndef _VERTEX_H
#define _VERTEX_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

// A vertex with two position coordinates (XY)
struct VertexP2
{
  GLfloat position[2];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP2),
                         (const GLvoid*)offsetof(VertexP2, position));
    glEnableVertexAttribArray(0);
  }
};

// A vertex with two position coordinates and two texture coordinates (XYUV)
struct VertexP2T2
{
  GLfloat position[2];
  GLfloat uv[2];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP2T2),
                         (const GLvoid*)offsetof(VertexP2T2, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP2T2),
                         (const GLvoid*)offsetof(VertexP2T2, uv));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
  }
};

// A vertex with three position coordinates (XYZ)
struct VertexP3
{
  GLfloat position[3];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3),
                         (const GLvoid*)offsetof(VertexP3, position));
    glEnableVertexAttribArray(0);
  }
};

// A vertex with three position coordinates and two texture coords (XYZUV)
struct VertexP3T2
{
  GLfloat position[3];
  GLfloat uv[2];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3T2),
                         (const GLvoid*)offsetof(VertexP3T2, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3T2),
                         (const GLvoid*)offsetof(VertexP3T2, uv));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
  }
};

// A vertex with three position coordinates and three color values (XYZRGB)
struct VertexP3C3
{
  GLfloat position[3];
  GLfloat color[3];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C3),
                         (const GLvoid*)offsetof(VertexP3C3, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C3),
                         (const GLvoid*)offsetof(VertexP3C3, color));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
  }
};

// A vertex with three position coordinates and four color values (XYZRGBA)
struct VertexP3C4
{
  GLfloat position[3];
  GLfloat color[4];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C4),
                         (const GLvoid*)offsetof(VertexP3C4, position));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                         sizeof(VertexP3C4),
                         (const GLvoid*)offsetof(VertexP3C4, color));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
  }
};

// A vertex with three position coordinates and three normal coordinates (XYZNNN)
struct VertexP3N3
{
  GLfloat position[3];
  GLfloat normal[3];

  static void EnableVertexAttribArray()
  {
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3N3),
                          (const GLvoid*)offsetof(VertexP3N3, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexP3N3),
                          (const GLvoid*)offsetof(VertexP3N3, position));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
  }
};

} // namespace ar

#endif // _VERTEX_H
