#ifndef _VERTEXBUFFER_H
#define _VERTEXBUFFER_H

namespace ar
{

/*
  Keeps track of vertex and index buffers for a specific vertex format, and
  handles sending the data to the GPU.
*/
template <typename VertexT>
class VertexBuffer
{
public:
  VertexBuffer()
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

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);

    // tell OpenGL about our vertex format
    VertexT::EnableVertexAttribArray();

    // reset GL state
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _dirty = false;
  }

  ~VertexBuffer()
  {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vio);
  }

  bool Dirty() { return _dirty; };

  // Adds the given vertices to the buffer
  // Returns the offset to the first new vertex
  int AddVertices(std::vector<VertexT> v)
  {
    int offset = _vertices.size();

    // append new vertices to the end of our existing list
    _vertices.insert(std::end(_vertices), std::begin(v), std::end(v));
    _dirty = true;

    return offset;
  }

  // Adds the given vertex indices to the buffer
  // Returns the offset to the first new index
  int AddIndices(std::vector<GLuint> i)
  {
    int offset = _indices.size();

    // append new indices to the end of our existing list
    _indices.insert(std::end(_indices), std::begin(i), std::end(i));
    _dirty = true;

    return offset;
  }

  // sends all currently-held vertex data to GPU
  // Must ONLY be called from the thread owning the OpenGL Context!
  void BufferData()
  {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexT) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), &_indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _dirty = false;
  }

  // removes ALL vertex and index data from the buffer
  void ClearAll()
  {
    _indices.clear();
    _vertices.clear();

    _dirty = true;
  }

  // Draws numVertices previously-buffered vertices beginning at offset in the vertex buffer
  void Draw(unsigned int numVertices, int vertex_offset, int index_offset)
  {
    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vio);

    glDrawElementsBaseVertex(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, (void*)(index_offset*sizeof(GLuint)), vertex_offset);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

private:
  bool _dirty;
  GLuint _vao;
  GLuint _vbo;  // vertex buffer
  GLuint _vio;  // index buffer
  std::vector<VertexT> _vertices;
  std::vector<GLuint> _indices;
};

} // namespace ar

#endif // _VERTEXBUFFER_H
