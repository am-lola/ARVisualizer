#include "arvisualizer.hpp"
#include "renderer.hpp"
#include "mesh/meshfactory.hpp"
#include "windowmanager/windowmanager.hpp"
#include <iostream>
#include <stdlib.h>

namespace ar
{

ARVisualizer::ARVisualizer()
{
  _renderer = nullptr;
}

ARVisualizer::~ARVisualizer()
{
  if (_renderer->IsRunning())
  {
    Stop();
  }
}

void ARVisualizer::Start(int width, int height)
{
  _renderer = WindowManager::Instance().NewRenderer(width, height, "AR visualizer");
}

void ARVisualizer::Stop()
{
  if (_renderer)
  {
    _renderer->Stop();
  }
}

bool ARVisualizer::IsRunning()
{
  if (_renderer)
  {
    return _renderer->IsRunning();
  }
  else
  {
    return false;
  }
}

void ARVisualizer::NotifyNewVideoFrame(int width, int height, unsigned char* pixels)
{
  _renderer->NotifyNewVideoFrame(width, height, pixels);
}

void ARVisualizer::SetCameraPose(double position[3], double forward[3], double up[3])
{
  glm::vec3 vPos = glm::vec3( position[0], position[1], position[2] );
  glm::vec3 vFor = glm::vec3(  forward[0],  forward[1],  forward[2] );
  glm::vec3 vUp  = glm::vec3(       up[0],       up[1],       up[2] );

  _renderer->SetCameraPose(vPos, vFor, vUp);
}

mesh_handle ARVisualizer::AddTriangle(double vertexPositions[3][3], double color[4])
{
  std::vector<glm::vec3> positions = {
    { vertexPositions[0][0], vertexPositions[0][1], vertexPositions[0][2] },
    { vertexPositions[1][0], vertexPositions[1][1], vertexPositions[1][2] },
    { vertexPositions[2][0], vertexPositions[2][1], vertexPositions[2][2] }
  };
  glm::vec4 vColor = glm::vec4( color[0], color[1], color[2], color[3] );

  return _renderer->Add3DMesh(MeshFactory::MakeTriangle(positions, vColor));
}

mesh_handle ARVisualizer::AddQuad(double center[3], double normal[3], double width, double height, double color[4])
{
  glm::vec3 vCenter = glm::vec3( center[0], center[1], center[2] );
  glm::vec3 vNormal = glm::vec3( normal[0], normal[1], normal[2] );
  glm::vec4 vColor  = glm::vec4(  color[0],  color[1],  color[2], color[3] );

  return _renderer->Add3DMesh(MeshFactory::MakeQuad(vCenter, vNormal, width, height, vColor));
}

mesh_handle ARVisualizer::AddSphere(double center[3], double radius, double color[4])
{
  glm::vec3 vCenter = glm::vec3( center[0], center[1], center[2] );
  glm::vec4 vColor  = glm::vec4(  color[0],  color[1],  color[2], color[3] );

  return _renderer->Add3DMesh(MeshFactory::MakeIcosphere(vCenter, radius, ICOSPHERE_SUBDIV_LEVELS, vColor));
}

mesh_handle ARVisualizer::AddCapsule(double center1[3], double center2[3], double radius, double color[4])
{
  glm::vec3 vCenter1 = glm::vec3( center1[0], center1[1], center1[2] );
  glm::vec3 vCenter2 = glm::vec3( center2[0], center2[1], center2[2] );
  glm::vec4 vColor   = glm::vec4(   color[0],   color[1],   color[2], color[3] );

  return _renderer->Add3DMesh(MeshFactory::MakeCapsule(vCenter1, vCenter2, radius, vColor, UVSPHERE_RESOLUTION));
}

void ARVisualizer::Remove(mesh_handle handle)
{
  _renderer->RemoveMesh(handle);
}

} // namespace ar
