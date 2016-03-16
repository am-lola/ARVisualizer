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

template <typename VertexT>
class PointCloud : public RenderResource
{
public:

  using VertexType = VertexT;
  using VertexBufferType = GenericVertexBuffer<VertexType>;

  PointCloud() = default;

  virtual void Init() override
  {
    _vertexBuffer.Init();
  }

  virtual void Release() override
  {
    _vertexBuffer.Release();
  }

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
  void SetMaterial(SharedPtr<Material> m) {
    _material = m;
    if (_shaderProgram != nullptr)
      _material->SetShader(_shaderProgram);
  };

  VertexBufferType& GetVertexBuffer() { return _vertexBuffer; }

  glm::mat4 GetTransform() const  { return _transform; }
  void SetTransform(glm::mat4 transform) { _transform = transform; }

  void SetPoints(const VertexType* points, size_t numPoints)
  {
    _points.assign(points, points + numPoints);
    _dirty = true;
  }

  void UpdateBuffer()
  {
    if (!_dirty)
      return;

    _vertexBuffer.SetVertices(_points);
    _dirty = false;
  }

  // Removes all points from the point cloud
  void ClearPoints()
  {
    _points.clear();
    _vertexBuffer.ClearAll();
  }

  inline bool ShouldDraw() const { return _shouldDraw && NumPoints() > 0; }
  inline size_t NumPoints() const { return _points.size(); }

  void RenderGUI()
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

  float _fadeDepth = 5.0f;
  float _pointSize = 1.0f;

private:
  friend class PointCloudRenderer;

  unsigned int _id;
  bool _dirty = false;
  bool _pendingDelete = false;

  std::string _name;
  Vector<VertexType> _points;
  ShaderProgram* _shaderProgram;
  SharedPtr<Material> _material;
  glm::mat4 _transform = glm::mat4(1.0); // transformation of this object from the origin
  VertexBufferType _vertexBuffer;

  bool _shouldDraw = true;
};

}

#endif // _AR_POINTCLOUD_H
