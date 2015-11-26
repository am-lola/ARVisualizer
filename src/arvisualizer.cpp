#include "arvisualizer.hpp"
#include "renderer.hpp"
#include "mesh/meshfactory.hpp"
#include <iostream>
#include <stdlib.h>

namespace ar
{

ARVisualizer::ARVisualizer()
{
  _renderer = new Renderer();
}

ARVisualizer::ARVisualizer(int windowWidth, int windowHeight)
{
  _renderer = new Renderer(windowWidth, windowHeight);
}

ARVisualizer::~ARVisualizer()
{
  if (_renderer->IsRunning())
  {
    Stop();
  }
}

void ARVisualizer::Start()
{
  _renderer->Start();
}

void ARVisualizer::Stop()
{
  _renderer->Stop();
}

bool ARVisualizer::IsRunning()
{
  return _renderer->IsRunning();
}

void ARVisualizer::NotifyNewVideoFrame(int width, int height, unsigned char* pixels)
{
  _renderer->NotifyNewVideoFrame(width, height, pixels);
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

} // namespace ar
