#ifndef _ARPPOINTCLOUD_RENDERING_HPP
#define _ARPPOINTCLOUD_RENDERING_HPP

#include "RenderComponent.hpp"
#include "ShaderProgram.hpp"
#include "mesh/Vertex.hpp"
#include "pointcloud/PointCloud.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "common.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace ar
{

class PointCloudRenderer : public RenderComponent
{
public:
  PointCloudRenderer();

  virtual void Init() override;
  virtual void Release() override;
  virtual void Update() override;
  virtual void RenderPass(const class SceneInfo& sceneInfo) override;
  virtual void RenderGUI() override;


  void AddPointCloud(UniquePtr<PointCloud<VertexP4>> pointCloud, Color color);
  void UpdatePointCloud(unsigned int handle, Vector<VertexP4>& points, Color color);

private:

  Vector<UniquePtr<PointCloud<VertexP4>>> _pointClouds;
  ShaderProgram _pointCloudShader;

  GenericVertexBuffer<VertexP4> _vertexBuffer;

  std::unordered_map<unsigned int, size_t> _handleIndexMap;
};

}

#endif // _ARPPOINTCLOUD_RENDERING_HPP
