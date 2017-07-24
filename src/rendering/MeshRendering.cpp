#include "MeshRendering.hpp"

namespace ar
{

template <typename VertexT>
MeshRenderer<VertexT>::MeshRenderer()
{
}

template <typename VertexT>
void MeshRenderer<VertexT>::Init()
{
  _vertexBuffer.Init();
  _indexBuffer.Init();
}

template <typename VertexT>
void MeshRenderer<VertexT>::Release()
{
  _vertexBuffer.Release();
  _indexBuffer.Release();
}

template <typename VertexT>
void MeshRenderer<VertexT>::Update()
{
  if (_vertexBufferNeedsRebuild)
  {
    _vertexBuffer.ClearAll();
    _indexBuffer.ClearAll();

    for (auto& mesh : _meshes)
    {
      mesh->SetVertexOffset(_vertexBuffer.AddVertices(mesh->GetVertices()));
      mesh->SetIndexOffset(_indexBuffer.AddIndices(mesh->GetIndices()));
      mesh->ClearDirty();
    }

    _vertexBufferNeedsRebuild = false;
  }

  _vertexBuffer.BufferData();
  _indexBuffer.BufferData();
}

template <typename VertexT>
void MeshRenderer<VertexT>::RenderPass(const SceneInfo& sceneInfo)
{
  for (auto& m : _meshes)
  {
    if (!sceneInfo.shouldDraw(m->ID()))
      continue;
    else if (m->GetMaterial()->GetOpaque() != sceneInfo.onlyOpaque)
      continue;

    const auto& shader = m->GetShader();
    shader->enable();

    // uniforms global to all objects
    glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, &(m->GetTransform()[0][0]));
    glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, &(sceneInfo.viewMatrix[0][0]));
    glUniform3fv(shader->getUniform("light_dir"), 1, &(sceneInfo.lightDir[0]));

    glUniform1i(shader->getUniform("lightAlpha"), (int)sceneInfo.lightAlpha);

    // object-specific uniforms
    glm::mat4 mvp = sceneInfo.projectionMatrix * sceneInfo.viewMatrix * m->GetTransform();
    glUniformMatrix4fv(shader->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
    m->GetMaterial()->Apply();

    glBindVertexArray(_vertexBuffer._vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer._vio);

    glDrawElementsBaseVertex(sceneInfo.renderType,
                             m->IndexCount(),
                             GL_UNSIGNED_INT,
                             (void*)(m->GetIndexOffset() * sizeof(GLuint)),
                             m->GetVertexOffset());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}

template <typename VertexT>
void MeshRenderer<VertexT>::AddMesh(Mesh<VertexT>* mesh)
{
  mesh->SetShader(_defaultShader);

  if (_removedMesh)
  {
    _removedMesh = false;
    _vertexBufferNeedsRebuild = true;
  }
  else
  {
    mesh->SetVertexOffset(_vertexBuffer.AddVertices(mesh->GetVertices()));
    mesh->SetIndexOffset(_indexBuffer.AddIndices(mesh->GetIndices()));
    mesh->ClearDirty();
  }

  _meshes.emplace_back(mesh);
  _handleIndexMap[mesh->ID()] = _meshes.size() - 1;
}

template <typename VertexT>
void MeshRenderer<VertexT>::RemoveMesh(unsigned int handle)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  const auto index = _handleIndexMap[handle];
  _handleIndexMap[_meshes.back()->ID()] = index;
  _handleIndexMap.erase(handle);

  // Swap with end of meshes list
  std::swap(_meshes[index], _meshes.back());
  _meshes.pop_back();

  // Don't rebuild the vertex buffer just yet, only when we add a new mesh
  _removedMesh = true;
}

template <typename VertexT>
void MeshRenderer<VertexT>::RemoveAllMeshes()
{
  _meshes.clear();
  _handleIndexMap.clear();

  _removedMesh = true;
}

template <typename VertexT>
void MeshRenderer<VertexT>::UpdateMesh(unsigned int handle, Mesh<VertexT>* mesh)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  mesh->SetShader(_defaultShader);
  mesh->SetID(handle);

  _meshes[_handleIndexMap[handle]] = UniquePtr<Mesh<VertexT>>(mesh);
  _vertexBufferNeedsRebuild = true;
}

template <typename VertexT>
void MeshRenderer<VertexT>::SetMeshTransform(unsigned int handle, const glm::mat4& transform, bool absolute)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  auto& mesh = _meshes[_handleIndexMap[handle]];
  mesh->SetTransform(absolute ? transform : transform * mesh->GetTransform());
}

template class MeshRenderer<Vertex3D>;
template class MeshRenderer<VertexLine>;

}
