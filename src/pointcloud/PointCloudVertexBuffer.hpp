#ifndef _PC_VERTEXBUFFER_H
#define _PC_VERTEXBUFFER_H

#include "common.hpp"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

/*
  Keeps track of a vertex buffer for point clouds, and
  handles sending the data to the GPU.
*/
template<typename VertexT>
class PointCloudVertexBuffer
{
public:
  PointCloudVertexBuffer()
  {
    // the gl* calls below will all fail if a GL context doesn't exist
    if (glfwGetCurrentContext() == NULL)
    {
      throw std::runtime_error("VertexBuffer cannot be created without an OpenGL Context");
    }

    // generate buffers
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // tell OpenGL about our vertex format
    VertexT::EnableVertexAttribArray();

    // reset GL state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _dirty = false;
  }

  ~PointCloudVertexBuffer()
  {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
  }

  inline bool Dirty() const
  {
    return _dirty;
  }

  // Sets the vertices stored in the buffer
  void SetVertices(const VertexT* vertices, size_t numVertices)
  {
    _vertices.assign(vertices, vertices + numVertices);
    _dirty = true;
  }

  void SetVertices(Vector<VertexT> vertices)
  {
    _vertices = std::move(vertices); // steal vertices from caller
    _dirty = true;
  }

  // sends all currently-held vertex data to GPU
  // Must ONLY be called from the thread owning the OpenGL Context!
  void BufferData()
  {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexT) * _vertices.size(), &_vertices[0], GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _dirty = false;
  }

  // removes ALL vertex data from the buffer
  void ClearAll()
  {
    _vertices.clear();

    _dirty = true;
  }

  void Draw()
  {
    glBindVertexArray(_vao);
    glDrawArrays(GL_POINTS, 0, _vertices.size());
    glBindVertexArray(0);
  }

  const Vector<VertexT>& GetVertices() const { return _vertices; }

private:
  bool _dirty;
  GLuint _vao;
  GLuint _vbo;  // vertex buffer
  Vector<VertexT> _vertices;
};

}

#endif // _PC_VERTEXBUFFER_H
