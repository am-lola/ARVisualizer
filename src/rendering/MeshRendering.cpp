#include "MeshRendering.hpp"
#include "SceneInfo.hpp"

namespace ar
{

MeshRenderer::MeshRenderer()
{

}

void MeshRenderer::Init()
{
  _vertexBuffer.InitGL();
  _indexBuffer.InitGL();
}

void MeshRenderer::Release()
{

}

void MeshRenderer::Update()
{
  if (_vertexBufferNeedsRebuild)
  {
    _vertexBuffer.ClearAll();
    _indexBuffer.ClearAll();

    for (auto& mesh : _meshes)
    {
      mesh.SetVertexOffset(_vertexBuffer.AddVertices(mesh.GetVertices()));
      mesh.SetIndexOffset(_indexBuffer.AddIndices(mesh.GetIndices()));
      mesh.ClearDirty();
    }

    _vertexBufferNeedsRebuild = false;
  }

  _vertexBuffer.BufferData();
  _indexBuffer.BufferData();
}

void MeshRenderer::RenderPass(const SceneInfo& sceneInfo)
{
  for (auto& m : _meshes)
  {
    const auto& shader = m.GetShader();
    shader->enable();

    // uniforms global to all objects
    glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, &(m.GetTransform()[0][0]));
    glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, &(sceneInfo.viewMatrix[0][0]));
    glUniform3fv(shader->getUniform("light_dir"), 1, &(sceneInfo.lightDir[0]));

    // object-specific uniforms
    glm::mat4 mvp = sceneInfo.projectionMatrix * sceneInfo.viewMatrix * m.GetTransform();
    glUniformMatrix4fv(shader->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
    m.GetMaterial()->Apply();

    //_vertexBuffer.Draw(sceneInfo.renderType, m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
    glBindVertexArray(_vertexBuffer._vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer._vio);

    glDrawElementsBaseVertex(sceneInfo.renderType,
                             m.IndexCount(),
                             GL_UNSIGNED_INT,
                             (void*)(m.GetIndexOffset() * sizeof(GLuint)),
                             m.GetVertexOffset());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}
void MeshRenderer::AddMesh(Mesh<Vertex3D>& mesh)
{
  mesh.SetShader(_defaultShader);

  if (_removedMesh)
  {
    _removedMesh = false;
    _vertexBufferNeedsRebuild = true;
  }
  else
  {
    mesh.SetVertexOffset(_vertexBuffer.AddVertices(mesh.GetVertices()));
    mesh.SetIndexOffset(_indexBuffer.AddIndices(mesh.GetIndices()));
    mesh.ClearDirty();
  }

  _meshes.push_back(mesh);
  _handleIndexMap[mesh.ID()] = _meshes.size() - 1;
}

void MeshRenderer::RemoveMesh(unsigned int handle)
{
  const auto index = _handleIndexMap[handle];
  _handleIndexMap[_meshes.back().ID()] = index;
  _handleIndexMap.erase(handle);

  // Swap with end of meshes list
  _meshes[index] = _meshes.back();
  _meshes.pop_back();

  // Don't rebuild the vertex buffer just yet, only when we add a new mesh
  _removedMesh = true;
}

void MeshRenderer::UpdateMesh(unsigned int handle, const Mesh<Vertex3D>& mesh)
{
  _meshes[_handleIndexMap[handle]] = mesh;
  _vertexBufferNeedsRebuild = true;
}

void MeshRenderer::SetMeshTransform(unsigned int handle, const glm::mat4& transform, bool absolute)
{
  auto& mesh = _meshes[_handleIndexMap[handle]];
  mesh.SetTransform(absolute ? transform : transform * mesh.GetTransform());
}

}