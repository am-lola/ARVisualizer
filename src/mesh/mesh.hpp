#ifndef _MESH_H
#define _MESH_H
#include <vector>
#include "vertex.hpp"

namespace ar
{

template <typename VertexT>
class Mesh
{
public:
  Mesh() : _id(0) {};
  Mesh(ShaderProgram* s) : _id(0), _shader(s) {};
  Mesh(std::vector<VertexT> v) : _id(0), _vertices(v), _dirty(true) {};
  Mesh(std::vector<VertexT> v, ShaderProgram* s) : _id(0), _vertices(v), _shader(s), _dirty(true) {};

  int ID() { return _id; };

  bool Dirty() { return _dirty; };

  unsigned int VertexCount() { return _vertices.size(); };
  std::vector<VertexT> GetVertices() { return _vertices; };
  void SetVertices(std::vector<VertexT> v) { _vertices = v; _dirty = true; };

  unsigned int IndexCount() { return _indices.size(); };
  std::vector<GLuint> GetIndices() { return _indices; };
  void SetIndices(std::vector<GLuint> v) { _indices = v; _dirty = true; };

  ShaderProgram* GetShader() { return _shader; };
  void SetShader(ShaderProgram* s) { _shader = s; };

  int GetOffset() { return _offset; };
  void SetOffset(int o) { _offset = o; };

private:
  int _id = 0;
  bool _dirty = false; // marked True if we have vertex data the renderer doesn't know about, yet
  int _offset = 0;     // offset of first vertex of this mesh in the VBO it's drawn from
  std::vector<VertexT> _vertices;
  std::vector<GLuint> _indices;
  ShaderProgram* _shader;
};

class TexturedQuad : public Mesh<VertexP2T2>
{
public:
  GLuint GetTexture() { return _texture; };
  void SetTexture(GLuint tex) { _texture = tex; };

private:
  GLuint _texture;
};

} // namespace ar

#endif // _MESH_H
