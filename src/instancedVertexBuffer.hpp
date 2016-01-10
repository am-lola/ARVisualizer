#ifndef _INSTANCEDVERTEXBUFFER_H
#define _INSTANCEDVERTEXBUFFER_H

#include "common.hpp"

namespace ar
{

template <typename VertexT, typename InstanceT>
class InstancedVertexBuffer
{
public:

  InstancedVertexBuffer()
  {
    // the gl* calls below will all fail if a GL context doesn't exist
    if (glfwGetCurrentContext() == NULL)
    {
      throw std::runtime_error("VertexBuffer cannot be created without an OpenGL Context");
    }

    // generate buffers
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vio);

    // instance buffer
    glGenBuffers(1, &_ibo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);

    // tell OpenGL about our vertex format
    auto numVertexAttribs = VertexT::EnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

  ~InstancedVertexBuffer()
  {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vio);
    glDeleteBuffers(1, &_ibo);
  }

  void SetVertices(Vector<VertexT> vertices)
  {
    _vertices = vertices;
    _dirtyVertices = true;
  }

  void SetIndices(Vector<GLuint> indices)
  {
    _indices = indices;
    _dirtyVertices = true;
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

  void ClearAll()
  {
    _indices.clear();
    _vertices.clear();
    _instances.clear();

    _dirtyInstances = true;
    _dirtyVertices = true;
  }

  void BufferData()
  {
    if (_dirtyInstances)
    {
      glBindBuffer(GL_ARRAY_BUFFER, _ibo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceT) * _instances.size(), &_instances[0], GL_DYNAMIC_DRAW);
    }

    if (_dirtyVertices)
    {
      glBindBuffer(GL_ARRAY_BUFFER, _vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(VertexT) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
    }

    // Cleanup state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _dirtyInstances = false;
    _dirtyVertices = false;
  }

  void Draw(GLuint mode)
  {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);
    glBindBuffer(GL_ARRAY_BUFFER, _ibo);

    glDrawElementsInstanced(mode, _indices.size(), GL_UNSIGNED_INT, 0, _instances.size());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  size_t InstanceCount() const { return _instances.size(); }

private:

  bool _dirtyVertices = false;
  bool _dirtyInstances = false;

  GLuint _vao;
  GLuint _vbo;  // vertex buffer
  GLuint _vio;  // index buffer

  GLuint _ibo;  // instance buffer

  Vector<VertexT> _vertices;
  Vector<GLuint> _indices;
  Vector<InstanceT> _instances;
};

}

#endif // _INSTANCEDVERTEXBUFFER_H
