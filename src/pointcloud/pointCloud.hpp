#ifndef _POINTCLOUD_H
#define _POINTCLOUD_H

#include "ShaderProgram.hpp"
#include "material.hpp"
#include "mesh/vertex.hpp"
#include "common.hpp"
#include "pointCloudVertexBuffer.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <imgui.h>
#include <fstream>

namespace ar
{

template <typename VertexT>
class PointCloud
{
public:

  using VertexType = VertexT;
  using VertexBufferType = PointCloudVertexBuffer<VertexType>;

  PointCloud()
  {
  }

  void InitVertexBuffer()
  {
    _vertexBuffer = UniquePtr<VertexBufferType>(new VertexBufferType());
  }

  unsigned int ID() { return _id; }
  void SetID(unsigned int id) { _id = id; _name = std::string("Point Cloud ") + std::to_string(_id); }

  bool Dirty() { return _dirty; }

  ShaderProgram* GetShader() { return _shaderProgram; }
  void SetShader(ShaderProgram* s)
  {
    _shaderProgram = s;
  }

  VertexBufferType* GetVertexBuffer() { return _vertexBuffer.get(); }

  glm::mat4 GetTransform() const
  {
    // Quick&Dirty adjusting the transformation for Asus Xtion
    return glm::orientate4(glm::vec3(glm::radians(12.0f), glm::radians(180.0f), 0.0f));
  }

  void SetPoints(const VertexType* points, size_t numPoints)
  {
    _numPoints = numPoints;
    _newPoints.assign(points, points + numPoints);
    _dirty = true;
  }

  void UpdateBuffer()
  {
    if (!_vertexBuffer)
    {
      InitVertexBuffer();
    }

    _vertexBuffer->SetVertices(_newPoints);
    _dirty = false;
  }

  // Removes all points from the point cloud
  void ClearPoints()
  {
    _numPoints = 0;
    _vertexBuffer->ClearAll();
  }

  inline bool ShouldDraw() const { return _shouldDraw && _numPoints > 0 && _vertexBuffer; }
  inline size_t NumPoints() const { return _numPoints; }

  void RenderGUI()
  {
    if (!ImGui::Begin(_name.c_str()))
    {
      ImGui::End();
      return;
    }

    ImGui::Text("Num points: %d", (int)_numPoints);
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

    ImGui::Separator();

    static char filePath[1024] { 0 };
    ImGui::InputText("File path", filePath, 1024);
    if (ImGui::Button("Save"))
      SaveToFile(filePath);

    ImGui::End();
  }

  // For prototyping
  void SaveToFile(const char* filePath)
  {
    std::ofstream fout(filePath);
    if (!fout.is_open())
      return;

    auto vertices = _vertexBuffer->GetVertices();

    for (int i = 0; i < vertices.size(); i++)
    {
      VertexT& vert = vertices[i];
      if (!std::isnan(vert.position[0]) && !std::isnan(vert.position[1]) && !std::isnan(vert.position[2]))
        fout << vert.position[0] << " " << vert.position[1] << " " << vert.position[2] << std::endl;
    }

    fout.close();
  }

  float _fadeDepth = 5.0f;
  float _pointSize = 1.0f;

private:
  unsigned int _id;
  bool _dirty = false;
  std::string _name;
  Vector<VertexType> _newPoints;
  size_t _numPoints = 0;
  ShaderProgram* _shaderProgram;

  UniquePtr<VertexBufferType> _vertexBuffer;

  bool _shouldDraw = true;
};

}

#endif // _AR_POINTCLOUD_H
