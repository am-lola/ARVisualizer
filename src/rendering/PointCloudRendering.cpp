#include "PointCloudRendering.hpp"
#include "ShaderSources.g.hpp"

namespace ar
{

PointCloudRenderer::PointCloudRenderer()
{
}

void PointCloudRenderer::Init()
{
  _pointCloudShader.loadAndLink(ShaderSources::sh_pointCloud_vert, ShaderSources::sh_flatShaded_frag);
  // super hacky #define insert
  std::string str = ShaderSources::sh_pointCloud_vert;
  str.insert(str.find("\n", str.find("#version")), "\n#define WITH_COLOR\n");
  _pointCloudColorShader.loadAndLink(str, ShaderSources::sh_flatShaded_frag);
}

void PointCloudRenderer::Release()
{
  // Release all point clouds
  for (auto& cloud : _pointClouds)
  {
    cloud->Release();
  }
}

void PointCloudRenderer::Update()
{
  for (auto& cloud : _pointClouds)
    cloud->UpdateBuffer();
}

void PointCloudRenderer::RenderPass(const SceneInfo& sceneInfo)
{
  for (auto& cloud : _pointClouds)
  {
    if (cloud->ShouldDraw())
    {
      const auto& shader = cloud->GetShader();
      shader->enable();
      cloud->GetMaterial()->Apply();

      glm::mat4 mvp = sceneInfo.projectionMatrix * sceneInfo.viewMatrix * cloud->GetTransform();
      glUniformMatrix4fv(shader->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
      glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, &cloud->GetTransform()[0][0]);
      glUniform1f(shader->getUniform("fadeDepth"), cloud->_fadeDepth);


      GLfloat storedPointSize;
      glGetFloatv(GL_POINT_SIZE, &storedPointSize);
      glPointSize(cloud->_pointSize);

      glBindVertexArray(cloud->GetVAO());
      glDrawArrays(GL_POINTS, 0, cloud->NumPoints());
      glBindVertexArray(0);

      glPointSize(storedPointSize);
    }
  }
}

void PointCloudRenderer::RenderGUI()
{
  for (auto& cloud : _pointClouds)
  {
    cloud->RenderGUI();
  }
}

void PointCloudRenderer::AddPointCloud(UniquePtr<BasePointCloud> pointCloud, bool colored, Color color)
{
  pointCloud->Init();
  pointCloud->SetShader(colored ? &_pointCloudColorShader : &_pointCloudShader);
  pointCloud->SetMaterial(std::make_shared<FlatColorMaterial>(color));

  _handleIndexMap[pointCloud->ID()] = _pointClouds.size();
  _pointClouds.push_back(std::move(pointCloud));
}

void PointCloudRenderer::UpdatePointCloud(unsigned int handle, Vector<VertexP4>& points, Color color)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  auto base_pc = _pointClouds[_handleIndexMap[handle]].get();
  auto pc = dynamic_cast<PointCloud<VertexP4>*>(base_pc);
  if (pc == nullptr)
    throw std::runtime_error("The point cloud for given handle has wrong type. Don't update a point cloud with a different type!");

  // TODO: Set color
  //pc->GetMaterial()->
  std::swap(pc->_points, points);
  pc->_dirty = true;
}

void PointCloudRenderer::UpdatePointCloud(unsigned int handle, Vector<Vertex_PCL_PointXYZRGBA>& points)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  auto pc = dynamic_cast<PointCloud<Vertex_PCL_PointXYZRGBA>*>(_pointClouds[_handleIndexMap[handle]].get());
  if (pc == nullptr)
    throw std::runtime_error("The point cloud for given handle has wrong type. Don't update a point cloud with a different type!");

  // TODO: error if pc == nullptr
  std::swap(pc->_points, points);
  pc->_dirty = true;
}

void PointCloudRenderer::SetPointCloudTransform(unsigned int handle, const glm::mat4& transform, bool absolute)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  auto& pc = _pointClouds[_handleIndexMap[handle]];
  pc->SetTransform(absolute ? transform : transform * pc->GetTransform());
}

void PointCloudRenderer::RemovePointCloud(unsigned int handle)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  const auto index = _handleIndexMap[handle];
  _handleIndexMap[_pointClouds.back()->ID()] = index;
  _handleIndexMap.erase(handle);

  // Swap with end of point clouds list
  std::swap(_pointClouds[index], _pointClouds.back());
  // Release and remove
  _pointClouds.back()->Release();
  _pointClouds.pop_back();
}

void PointCloudRenderer::RemoveAllPointClouds()
{
  // Release all point clouds
  for (auto& cloud : _pointClouds)
  {
    cloud->Release();
  }

  _pointClouds.clear();
  _handleIndexMap.clear();
}

}
