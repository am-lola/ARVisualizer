#include "ShaderSources.g.hpp"
#include "VoxelRendering.hpp"
#include "SceneInfo.hpp"
#include "mesh/MeshFactory.hpp"

namespace ar
{

VoxelRenderer::VoxelRenderer()
{
}

void VoxelRenderer::Init()
{
  // init buffers
  _instancedVertexBuffer.reset(new InstancedVertexBuffer<VertexP3N3, VertexP3C4S>());

  auto voxel_base_mesh = MeshFactory::MakeCube<Mesh<VertexP3N3>>(glm::vec3(0, 0, 0), 1.0);
  _instancedVertexBuffer->SetVertices(voxel_base_mesh.GetVertices());
  _instancedVertexBuffer->SetIndices(voxel_base_mesh.GetIndices());

  _shader.loadAndLink(ShaderSources::sh_voxel_vert, ShaderSources::sh_voxel_frag);
}

void VoxelRenderer::Release()
{
  _instancedVertexBuffer.reset();
}

void VoxelRenderer::Update()
{
  _instancedVertexBuffer->BufferData();
}

void VoxelRenderer::RenderPass(const SceneInfo& sceneInfo)
{
  if (_instancedVertexBuffer->InstanceCount() > 0)
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

    _instancedVertexBuffer->Draw(sceneInfo.renderType);
  }
}

void VoxelRenderer::SetVoxels(const Vector<Voxel>& voxels)
{
  const VertexP3C4S* vertices = reinterpret_cast<const VertexP3C4S*>(voxels.data());
  _instancedVertexBuffer->SetInstances(vertices, voxels.size());
}

}
