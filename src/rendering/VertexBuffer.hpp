#ifndef _VERTEXBUFFER_H
#define _VERTEXBUFFER_H

#include "common.hpp"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

class VertexBuffer
{
public:

  virtual void InitGL() = 0;
  virtual void BufferData() = 0;

  virtual ~VertexBuffer() { }
};

/*
  Vertex buffer for a specific vertex format.
*/
template <typename VertexT>
class GenericVertexBuffer : public VertexBuffer
{
public:
  GenericVertexBuffer() = default;

  // TODO: Don't put this in the destructor
  ~GenericVertexBuffer()
  {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
  }

  virtual void InitGL() override
  {
    // the gl* calls below will all fail if a GL context doesn't exist
    if (glfwGetCurrentContext() == nullptr)
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

  bool Dirty() { return _dirty; };

  // Adds the given vertices to the buffer
  // Returns the offset to the first new vertex
  int AddVertices(const Vector<VertexT>& vertices)
  {
    int offset = _vertices.size();

    // append new vertices to the end of our existing list
    _vertices.insert(std::end(_vertices), std::begin(vertices), std::end(vertices));
    _dirty = true;

    return offset;
  }

  void SetVertices(const Vector<VertexT>& vertices)
  {
    //std::cout << "setting\n";
    _vertices = vertices;
    _dirty = true;
  }

  // sends all currently-held vertex data to GPU
  // Must ONLY be called from the thread owning the OpenGL Context!
  virtual void BufferData() override
  {
    if (!_dirty)
      return;

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexT) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _dirty = false;
  }

  // removes ALL vertex and index data from the buffer
  void ClearAll()
  {
    _vertices.clear();
    _dirty = true;
  }

//private:

  bool _dirty = false;
  GLuint _vao; // vertex array object
  GLuint _vbo; // vertex buffer object
  Vector<VertexT> _vertices;
};

} // namespace ar

#endif // _VERTEXBUFFER_H
