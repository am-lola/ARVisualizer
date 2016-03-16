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
  {
    cloud->UpdateBuffer();
    cloud->GetVertexBuffer().BufferData();
  }
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

      glBindVertexArray(cloud->_vertexBuffer._vao);
      glDrawArrays(GL_POINTS, 0, cloud->_vertexBuffer._vertices.size());
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

void PointCloudRenderer::AddPointCloud(UniquePtr<PointCloud<VertexP4>> pointCloud, Color color)
{
  pointCloud->Init();
  pointCloud->SetShader(&_pointCloudShader);
  pointCloud->SetMaterial(std::make_shared<FlatColorMaterial>(color));

  _handleIndexMap[pointCloud->ID()] = _pointClouds.size();
  _pointClouds.push_back(std::move(pointCloud));
}

void PointCloudRenderer::UpdatePointCloud(unsigned int handle, Vector<VertexP4>& points, Color color)
{
  if (_handleIndexMap.find(handle) == _handleIndexMap.end())
    return;

  auto& pc = _pointClouds[_handleIndexMap[handle]];
  // TODO: Set color
  //pc->GetMaterial()->
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
