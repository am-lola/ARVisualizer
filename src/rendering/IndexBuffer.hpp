#ifndef _ARINDEXBUFFER_HPP
#define _ARINDEXBUFFER_HPP

#include "common.hpp"
#include "RenderResource.hpp"
#include "RenderDefinitions.hpp"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

class IndexBuffer : public RenderResource
{
public:
  virtual ~IndexBuffer() = default;

  virtual void BufferData() = 0;
};

/*
  Index buffer.
*/
class GenericIndexBuffer : public IndexBuffer
{
public:

  GenericIndexBuffer() = default;
  GenericIndexBuffer(BufferUsage usage) : _usage(usage) { }

  virtual void InitResource() override
  {
    glGenBuffers(1, &_vio);
  }

  virtual void ReleaseResource() override
  {
    glDeleteBuffers(1, &_vio);
  }

  int AddIndices(const Vector<GLuint>& indices)
  {
    size_t offset = _indices.size();

    // append new indices to the end of our existing list
    _indices.insert(std::end(_indices), std::begin(indices), std::end(indices));
    _dirty = true;

    return (int)offset;
  }

  void SetIndices(const Vector<GLuint>& indices)
  {
    _indices = indices;
    _dirty = true;
  }

  virtual void BufferData() override
  {
    if (!_initialized)
      throw std::runtime_error("The index buffer was not initialized.");

    if (!_dirty)
      return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), &_indices[0], GetGLUsage(_usage));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _dirty = false;
  }

  void ClearAll()
  {
    _indices.clear();

    _dirty = true;
  }

  bool _dirty = false;
  BufferUsage _usage = BufferUsage::Static;
  GLuint _vio;
  Vector<GLuint> _indices;
};

} // namespace ar

#endif // _ARINDEXBUFFER_HPP
