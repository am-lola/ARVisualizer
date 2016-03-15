#include "PointCloudRendering.hpp"
#include "SceneInfo.hpp"
#include "ShaderSources.g.hpp"

namespace ar
{

PointCloudRenderer::PointCloudRenderer()
{
}

void PointCloudRenderer::Init()
{
  //_vertexBuffer.InitGL();
  _pointCloudShader.loadAndLink(ShaderSources::sh_pointCloud_vert, ShaderSources::sh_flatShaded_frag);
}

void PointCloudRenderer::Release()
{

}

void PointCloudRenderer::Update()
{
  // TODO: Stream draw for vertex buffer
  for (auto& cloud : _pointClouds)
  {
    if (cloud->Dirty())
    {
      cloud->UpdateBuffer();
    }

    if (cloud->GetVertexBuffer()->Dirty())
    {
      cloud->GetVertexBuffer()->BufferData();
    }
  }

  //_vertexBuffer.BufferData();
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
      //glUniform4f(shader->getUniform("color"), 1,1,1,1);

      glm::mat4 mvp = sceneInfo.projectionMatrix * sceneInfo.viewMatrix * cloud->GetTransform();
      glUniformMatrix4fv(shader->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
      glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, &cloud->GetTransform()[0][0]);
      glUniform1f(shader->getUniform("fadeDepth"), cloud->_fadeDepth);


      GLfloat storedPointSize;
      glGetFloatv(GL_POINT_SIZE, &storedPointSize);
      glPointSize(cloud->_pointSize);

      glBindVertexArray(cloud->_vertexBuffer->_vao);
      glDrawArrays(GL_POINTS, 0, cloud->_vertexBuffer->_vertices.size());
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
  pointCloud->SetShader(&_pointCloudShader);
  pointCloud->SetMaterial(std::make_shared<FlatColorMaterial>(color));

  _handleIndexMap[pointCloud->ID()] = _pointClouds.size();
  _pointClouds.push_back(std::move(pointCloud));
}

void PointCloudRenderer::UpdatePointCloud(unsigned int handle, Vector<VertexP4>& points, Color color)
{
  auto& pc = _pointClouds[_handleIndexMap[handle]];
  // TODO: Set color
  //pc->GetMaterial()->
  std::swap(pc->_points, points);
  pc->_dirty = true;
}

}
