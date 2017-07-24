#include "VoxelRendering.hpp"
#include "ShaderSources.g.hpp"
#include "mesh/MeshFactory.hpp"

namespace ar
{

VoxelRenderer::VoxelRenderer()
{
}

void VoxelRenderer::Init()
{
  // init buffers
  _instancedVertexBuffer.Init();
  _indexBuffer.Init();

  auto voxel_base_mesh = MeshFactory::MakeCube<Mesh<VertexP3N3>>(glm::vec3(0, 0, 0), 1.0);
  _instancedVertexBuffer.SetVertices(voxel_base_mesh.GetVertices());
  _indexBuffer.SetIndices(voxel_base_mesh.GetIndices());

  _shader.loadAndLink(ShaderSources::sh_voxel_vert, ShaderSources::sh_voxel_frag);
}

void VoxelRenderer::Release()
{
  _instancedVertexBuffer.Release();
  _indexBuffer.Release();
}

void VoxelRenderer::Update()
{
  _instancedVertexBuffer.BufferData();
  _indexBuffer.BufferData();
}

void VoxelRenderer::RenderPass(const SceneInfo& sceneInfo)
{
  if (sceneInfo.onlyOpaque) // only render in transparent passes
    return;

  if (_instancedVertexBuffer.InstanceCount() > 0)
  {
    ShaderProgram& shader = _shader;
    shader.enable();

    glm::mat4 modelTransform = glm::mat4(1.0f);

    // set uniforms
    glm::mat4 mv = sceneInfo.viewMatrix * modelTransform;
    glm::mat4 mvp = sceneInfo.projectionMatrix * mv;
    glm::vec4 lightDirWorldSpace = sceneInfo.viewMatrix * glm::vec4(sceneInfo.lightDir, 0);
    glm::vec3 ldir = glm::vec3(lightDirWorldSpace);
    glUniformMatrix4fv(shader.getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
    glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(shader.getUniform("light_dir"), 1, &(ldir[0]));

    glBindVertexArray(_instancedVertexBuffer._vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer._vio);
    glBindBuffer(GL_ARRAY_BUFFER, _instancedVertexBuffer._ibo);

    glDrawElementsInstanced(sceneInfo.renderType, _indexBuffer._indices.size(), GL_UNSIGNED_INT, 0, _instancedVertexBuffer.InstanceCount());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}

void VoxelRenderer::SetVoxels(const Vector<Voxel>& voxels)
{
  const VertexP3C4S* vertices = reinterpret_cast<const VertexP3C4S*>(voxels.data());
  _instancedVertexBuffer.SetInstances(vertices, voxels.size());
}

void VoxelRenderer::ClearVoxels()
{
  _instancedVertexBuffer.ClearInstances();
}

}
