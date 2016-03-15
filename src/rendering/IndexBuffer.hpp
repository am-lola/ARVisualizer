#ifndef _INDEXBUFFER_H
#define _INDEXBUFFER_H

#include "common.hpp"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

class IndexBuffer
{
public:

  virtual void InitGL() = 0;
  virtual void BufferData() = 0;

  virtual ~IndexBuffer() { }
};

/*
  Index buffer.
*/
class GenericIndexBuffer : public IndexBuffer
{
public:

  GenericIndexBuffer() = default;

  ~GenericIndexBuffer()
  {
    glDeleteBuffers(1, &_vio);
  }

  virtual void InitGL() override
  {
    if (glfwGetCurrentContext() == nullptr)
    {
      throw std::runtime_error("VertexBuffer cannot be created without an OpenGL Context");
    }

    glGenBuffers(1, &_vio);
  }

  int AddIndices(const Vector<GLuint>& indices)
  {
    int offset = _indices.size();

    // append new indices to the end of our existing list
    _indices.insert(std::end(_indices), std::begin(indices), std::end(indices));
    _dirty = true;

    return offset;
  }

  virtual void BufferData() override
  {
    if (!_dirty)
      return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _dirty = false;
  }

  void ClearAll()
  {
    _indices.clear();

    _dirty = true;
  }


  Vector<GLuint> _indices;
  GLuint _vio;
  bool _dirty = false;
};

} // namespace ar

#endif // _INDEXBUFFER_H
