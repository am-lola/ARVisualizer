#ifndef _POINTCLOUD_H
#define _POINTCLOUD_H

#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "mesh/Vertex.hpp"
#include "common.hpp"
#include "rendering/VertexBuffer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <imgui.h>

namespace ar
{

class BasePointCloud : public RenderResource
{
public:

  BasePointCloud() = default;

  unsigned int ID() const { return _id; }
  void SetID(unsigned int id) { _id = id; _name = std::string("Point Cloud ") + std::to_string(_id); }

  bool Dirty() const { return _dirty; }

  ShaderProgram* GetShader() { return _shaderProgram; }
  void SetShader(ShaderProgram* s)
  {
    _shaderProgram = s;
    if (_material != nullptr)
      _material->SetShader(s);
  }

  SharedPtr<Material> GetMaterial() { return _material; };
  void SetMaterial(SharedPtr<Material> m)
  {
    _material = m;
    if (_shaderProgram != nullptr)
      _material->SetShader(_shaderProgram);
  }

  glm::mat4 GetTransform() const  { return _transform; }
  void SetTransform(glm::mat4 transform) { _transform = transform; }

  virtual bool ShouldDraw() const = 0;
  virtual size_t NumPoints() const = 0;
  virtual void UpdateBuffer() = 0;
  virtual GLuint GetVAO() = 0;
  virtual void RenderGUI() = 0;

  float _fadeDepth = 5.0f;
  float _pointSize = 3.5f;

protected:

  friend class PointCloudRenderer;

  unsigned int _id;
  bool _dirty = false;
  bool _pendingDelete = false;

  std::string _name;
  ShaderProgram* _shaderProgram;
  SharedPtr<Material> _material;
  glm::mat4 _transform = glm::mat4(1.0); // transformation of this object from the origin
  bool _shouldDraw = true;
};

template <typename VertexT>
class PointCloud : public BasePointCloud
{
public:

  using VertexType = VertexT;
  using VertexBufferType = GenericVertexBuffer<VertexType>;

  PointCloud() = default;

  virtual void InitResource() override
  {
    _vertexBuffer.Init();
  }

  virtual void ReleaseResource() override
  {
    _vertexBuffer.Release();
  }

  VertexBufferType& GetVertexBuffer() { return _vertexBuffer; }

  void SetPoints(const VertexType* points, size_t numPoints)
  {
    _points.assign(points, points + numPoints);
    _dirty = true;
  }

  virtual void UpdateBuffer() override
  {
    if (!_dirty)
      return;

    _vertexBuffer.SetVertices(_points);
    _vertexBuffer.BufferData();
    _dirty = false;
  }

  virtual GLuint GetVAO()
  {
    return _vertexBuffer._vao;
  }

  // Removes all points from the point cloud
  void ClearPoints()
  {
    _points.clear();
    _vertexBuffer.ClearAll();
  }

  virtual bool ShouldDraw() const override { return _shouldDraw && NumPoints() > 0; }
  virtual size_t NumPoints() const override { return _points.size(); }

  virtual void RenderGUI() override
  {
    if (!ImGui::Begin(_name.c_str()))
    {
      ImGui::End();
      return;
    }

    ImGui::Text("Num points: %d", (int)NumPoints());
    ImGui::Separator();
    ImGui::PushItemWidth(-100);

    static bool shouldDraw;
    shouldDraw = _shouldDraw;
    ImGui::Checkbox("Draw", &shouldDraw);
    _shouldDraw = shouldDraw;

    static float fadeDepth = 5.0f;
    fadeDepth = _fadeDepth;
    ImGui::SliderFloat("Fade Depth", &fadeDepth, 1.0f, 10.0f);
    _fadeDepth = fadeDepth;

    static float pointsSize = 1.0f;
    pointsSize = _pointSize;
    ImGui::SliderFloat("Points Size", &pointsSize, 0.0f, 5.0f);
    _pointSize = pointsSize;

    ImGui::End();
  }

private:

  friend class PointCloudRenderer;

  Vector<VertexType> _points;
  VertexBufferType _vertexBuffer;
};

}

#endif // _AR_POINTCLOUD_H
