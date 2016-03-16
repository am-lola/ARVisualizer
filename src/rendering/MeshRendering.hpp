#ifndef _ARMESH_RENDERING_H
#define _ARMESH_RENDERING_H

#include "RenderingCommon.hpp"
#include "mesh/Mesh.hpp"
#include "mesh/Vertex.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace ar
{

class ShaderProgram;

typedef VertexP3N3 Vertex3D;

class MeshRenderer : public RenderComponent
{
public:

  MeshRenderer();

  virtual void Init() override;
  virtual void Release() override;
  virtual void Update() override;
  virtual void RenderPass(const class SceneInfo& sceneInfo) override;

  void AddMesh(Mesh<Vertex3D>& mesh);
  void RemoveMesh(unsigned int handle);
  void RemoveAllMeshes();
  void UpdateMesh(unsigned int handle, const Mesh<Vertex3D>& mesh);


  void SetMeshTransform(unsigned int handle, const glm::mat4& transform, bool absolute);

  inline void SetDefaultShader(ShaderProgram* shader) { _defaultShader = shader; }

private:

  ShaderProgram* _defaultShader;

  GenericVertexBuffer<Vertex3D> _vertexBuffer;
  GenericIndexBuffer _indexBuffer;
  Vector<Mesh<Vertex3D>> _meshes;

  std::unordered_map<unsigned int, unsigned int> _handleIndexMap;

  bool _vertexBufferNeedsRebuild = false;
  bool _removedMesh = false;
};

}

#endif // _ARMESH_RENDERING_H
