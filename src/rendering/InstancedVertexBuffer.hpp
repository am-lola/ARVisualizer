#ifndef _INSTANCEDVERTEXBUFFER_H
#define _INSTANCEDVERTEXBUFFER_H

#include "common.hpp"
#include "VertexBuffer.hpp"

namespace ar
{

template <typename VertexT, typename InstanceT>
class InstancedVertexBuffer : public GenericVertexBuffer<VertexT>
{
  typedef GenericVertexBuffer<VertexT> Base;

public:

  virtual void InitResource() override
  {
    // the gl* calls below will all fail if a GL context doesn't exist
    if (glfwGetCurrentContext() == NULL)
    {
      throw std::runtime_error("VertexBuffer cannot be created without an OpenGL Context");
    }

    // generate buffers
    glGenVertexArrays(1, &(Base::_vao));
    glGenBuffers(1, &(Base::_vbo));

    // instance buffer
    glGenBuffers(1, &_ibo);

    glBindVertexArray(Base::_vao);
    glBindBuffer(GL_ARRAY_BUFFER, Base::_vbo);

    // tell OpenGL about our vertex format
    auto numVertexAttribs = VertexT::EnableVertexAttribArray(0);

    // set vertex format for instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, _ibo);
    auto numInstaceAttribs = InstanceT::EnableVertexAttribArray(numVertexAttribs);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (GLuint i = 0; i < numVertexAttribs; i++)
    {
      glVertexAttribDivisor(i, 0); // attributes per draw call (mesh vertices etc.)
    }
    for (GLuint i = numVertexAttribs; i < numInstaceAttribs + numInstaceAttribs; i++)
    {
      glVertexAttribDivisor(i, 1); // attributes per instance
    }

    // reset GL state
    glBindVertexArray(0);
  }

  virtual void ReleaseResource() override
  {
    Base::Release();
    glDeleteBuffers(1, &_ibo);
  }

  void SetInstances(Vector<InstanceT> instances)
  {
    _instances = instances;
    _dirtyInstances = true;
  }

  void SetInstances(const InstanceT* instances, size_t numInstances)
  {
    _instances.assign(instances, instances + numInstances);
    _dirtyInstances = true;
  }

  virtual void ClearAll() override
  {
    Base::ClearAll();
    ClearInstances();
  }

  void ClearInstances()
  {
    _instances.clear();
    _dirtyInstances = true;
  }

  virtual void BufferData() override
  {
    Base::BufferData();

    if (!_dirtyInstances)
      return;

    glBindBuffer(GL_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceT) * _instances.size(), &_instances[0], GetGLUsage(Base::_usage));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _dirtyInstances = false;
  }

  inline size_t InstanceCount() const { return _instances.size(); }

  bool _dirtyInstances = false;
  GLuint _ibo;  // instance buffer
  Vector<InstanceT> _instances;
};

}

#endif // _INSTANCEDVERTEXBUFFER_H
