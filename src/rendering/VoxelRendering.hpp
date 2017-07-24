#ifndef _ARVOXEL_RENDERING_HPP
#define _ARVOXEL_RENDERING_HPP

#include "RenderingCommon.hpp"
#include "InstancedVertexBuffer.hpp"
#include "mesh/Vertex.hpp"
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
  void ClearVoxels();

private:

  ShaderProgram _shader;
  InstancedVertexBuffer<VertexP3N3, VertexP3C4S> _instancedVertexBuffer;
  GenericIndexBuffer _indexBuffer;
};

} // namespace ar


#endif // _ARVOXEL_RENDERING_HPP
