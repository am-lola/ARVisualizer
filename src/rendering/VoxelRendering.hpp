#ifndef _ARVOXEL_RENDERING_HPP
#define _ARVOXEL_RENDERING_HPP

#include "RenderComponent.hpp"
#include "ShaderProgram.hpp"
#include "mesh/Vertex.hpp"
#include "mesh/InstancedVertexBuffer.hpp"
#include "geometry/Voxel.hpp"

namespace ar
{

class VoxelRenderer : public RenderComponent
{
public:

  VoxelRenderer();

  virtual void Init() override;
  virtual void Release() override;
  virtual void Update() override;
  virtual void RenderPass(const class SceneInfo& sceneInfo) override;

  void SetVoxels(const Vector<Voxel>& voxels);

private:

  ShaderProgram _shader;
  UniquePtr<InstancedVertexBuffer<VertexP3N3, VertexP3C4S>> _instancedVertexBuffer;
};

}


#endif // _ARVOXEL_RENDERING_HPP
