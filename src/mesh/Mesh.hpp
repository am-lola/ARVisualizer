#ifndef _MESH_H
#define _MESH_H

#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Vertex.hpp"
#include "common.hpp"
#include <glm/glm.hpp>

namespace ar
{

template <typename VertexT>
class Mesh
{
public:
  Mesh() : _id(0) {};
  Mesh(ShaderProgram* s) : _id(0), _shader(s) {};
  Mesh(Vector<VertexT> v) : _id(0), _dirty(true), _vertices(v) {};
  Mesh(Vector<VertexT> v, Vector<GLuint> i) : _id(0), _dirty(true), _vertices(v), _indices(i) {};
  Mesh(Vector<VertexT> v, ShaderProgram* s) : _id(0), _dirty(true), _vertices(v), _shader(s) {};
  Mesh(Vector<VertexT> v, Vector<GLuint> i, ShaderProgram* s) : _id(0), _dirty(true), _vertices(v), _indices(i), _shader(s) {};

  unsigned int ID() const { return _id; };
  void SetID(unsigned int id) { _id = id; };

  bool Dirty() const { return _dirty; };
  void ClearDirty() { _dirty = false; };

  unsigned int VertexCount() const { return _vertices.size(); };
  Vector<VertexT> GetVertices() const { return _vertices; };
  void SetVertices(Vector<VertexT> v) { _vertices = v; _dirty = true; };

  unsigned int IndexCount() const { return _indices.size(); };
  Vector<GLuint> GetIndices() const { return _indices; };
  void SetIndices(Vector<GLuint> v) { _indices = v; _dirty = true; };

  ShaderProgram* GetShader() const { return _shader; };
  void SetShader(ShaderProgram* s) {
    _shader = s;
    if (_material != nullptr)
      _material->SetShader(s);
  };

  SharedPtr<Material> GetMaterial() const { return _material; };
  void SetMaterial(SharedPtr<Material> m) {
    _material = m;
    if (_shader != nullptr)
      _material->SetShader(_shader);
  };

  int GetVertexOffset() const { return _vtx_offset; };
  void SetVertexOffset(int o) { _vtx_offset = o; };

  int GetIndexOffset() const { return _idx_offset; };
  void SetIndexOffset(int o) { _idx_offset = o; };

  glm::mat4 GetTransform() const { return _transform; };
  void SetTransform(glm::mat4 t) { _transform = t; };

private:
  unsigned int _id = 0;
  bool _dirty = false; // marked True if we have vertex data the renderer doesn't know about, yet
  int _vtx_offset = 0; // offset of first vertex of this mesh in the VBO it's drawn from
  int _idx_offset = 0; // offset of first index of this mesh in the index buffer it's drawn from
  Vector<VertexT> _vertices;
  Vector<GLuint> _indices;
  ShaderProgram* _shader = nullptr;
  SharedPtr<Material> _material;
  glm::mat4 _transform = glm::mat4(1.0); // transformation of this object from the origin
};

template <typename VertexT>
class TexturedMesh : public Mesh<VertexT>
{
public:
  using Mesh<VertexT>::Mesh;

  GLuint GetTexture() { return _texture; };
  void SetTexture(GLuint tex) { _texture = tex; };

private:
  GLuint _texture;
};

class LineMesh : public Mesh<VertexLine>
{
public:
  using Mesh<VertexLine>::Mesh;

  void SetThickness(float thickness) { _thickness = thickness; }
  float GetThickness() const { return _thickness; }

private:

  float _thickness;
};


} // namespace ar

#endif // _MESH_H
