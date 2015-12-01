#ifndef _MESH_H
#define _MESH_H
#include <vector>
#include "ShaderProgram.hpp"
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
  Mesh(std::vector<VertexT> v, std::vector<GLuint> i) : _id(0), _vertices(v), _indices(i), _dirty(true) {};
  Mesh(std::vector<VertexT> v, ShaderProgram* s) : _id(0), _vertices(v), _shader(s), _dirty(true) {};
  Mesh(std::vector<VertexT> v, std::vector<GLuint> i, ShaderProgram* s) : _id(0), _vertices(v), _indices(i), _shader(s), _dirty(true) {};

  int ID() { return _id; };

  bool Dirty() { return _dirty; };
  void ClearDirty() { _dirty = false; };

  unsigned int VertexCount() { return _vertices.size(); };
  std::vector<VertexT> GetVertices() { return _vertices; };
  void SetVertices(std::vector<VertexT> v) { _vertices = v; _dirty = true; };

  unsigned int IndexCount() { return _indices.size(); };
  std::vector<GLuint> GetIndices() { return _indices; };
  void SetIndices(std::vector<GLuint> v) { _indices = v; _dirty = true; };

  ShaderProgram* GetShader() { return _shader; };
  void SetShader(ShaderProgram* s) { _shader = s; };

  int GetVertexOffset() { return _vtx_offset; };
  void SetVertexOffset(int o) { _vtx_offset = o; };

  int GetIndexOffset() { return _idx_offset; };
  void SetIndexOffset(int o) { _idx_offset = o; };

  glm::mat4 GetTransform() { return _transform; };
  void SetTransform(glm::mat4 t) { _transform = t; };

private:
  int _id = 0;
  bool _dirty = false; // marked True if we have vertex data the renderer doesn't know about, yet
  int _vtx_offset = 0; // offset of first vertex of this mesh in the VBO it's drawn from
  int _idx_offset = 0; // offset of first index of this mesh in the index buffer it's drawn from
  std::vector<VertexT> _vertices;
  std::vector<GLuint> _indices;
  ShaderProgram* _shader;
  glm::mat4 _transform = glm::mat4(1.0); // transformation of this object from the origin
};

class TexturedQuad : public Mesh<VertexP2T2>
{
public:
  using Mesh::Mesh;

  GLuint GetTexture() { return _texture; };
  void SetTexture(GLuint tex) { _texture = tex; };

private:
  GLuint _texture;
};

} // namespace ar

#endif // _MESH_H
