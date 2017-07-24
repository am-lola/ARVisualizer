#ifndef _ARPPOINTCLOUD_RENDERING_HPP
#define _ARPPOINTCLOUD_RENDERING_HPP

#include "RenderingCommon.hpp"
#include "mesh/Vertex.hpp"
#include "pointcloud/PointCloud.hpp"

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
  virtual void RenderPass(const SceneInfo& sceneInfo) override;
  virtual void RenderGUI() override;


  void AddPointCloud(UniquePtr<BasePointCloud> pointCloud, bool colored, Color color);
  // NOTE: swaps out points vector!
  void UpdatePointCloud(unsigned int handle, Vector<VertexP4>& points, Color color);
  // NOTE: swaps out points vector!
  void UpdatePointCloud(unsigned int handle, Vector<Vertex_PCL_PointXYZRGBA>& points);
  void SetPointCloudTransform(unsigned int handle, const glm::mat4& transform, bool absolute);
  void RemovePointCloud(unsigned int handle);
  void RemoveAllPointClouds();

private:

  Vector<UniquePtr<BasePointCloud>> _pointClouds;
  ShaderProgram _pointCloudShader;
  ShaderProgram _pointCloudColorShader;

  std::unordered_map<unsigned int, size_t> _handleIndexMap;
};

} // namespace ar

#endif // _ARPPOINTCLOUD_RENDERING_HPP
