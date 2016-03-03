#include "arvisualizer.hpp"
#include "renderer.hpp"
#include "mesh/meshfactory.hpp"
#include "windowmanager/windowmanager.hpp"
#include "ui/ui_internal.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace ar
{

ARVisualizer::ARVisualizer()
  : _ui(new UserInterface)
{
  _renderer = nullptr;
}

ARVisualizer::~ARVisualizer()
{
  if (IsRunning())
  {
    Stop();
  }

  delete _ui;
}

void ARVisualizer::Start(int width, int height)
{
  _renderer = WindowManager::Instance().NewRenderer(width, height, "AR visualizer");
  _renderer->_renderGUIDelegate += [this]()
  {
    this->renderExternGUI();
  };
}

void ARVisualizer::Start()
{
  Start(1024, 768);
}

void ARVisualizer::Stop()
{
  if (_renderer)
  {
    _renderer->Stop();
    _renderer = nullptr;
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
  if (!IsRunning()) { return; }
  _renderer->NotifyNewVideoFrame(width, height, pixels);
}

void ARVisualizer::SetCameraPose(double position[3], double forward[3], double up[3])
{
  if (!IsRunning()) { return; }
  glm::vec3 vPos = glm::vec3( position[0], position[1], position[2] );
  glm::vec3 vFor = glm::vec3(  forward[0],  forward[1],  forward[2] );
  glm::vec3 vUp  = glm::vec3(       up[0],       up[1],       up[2] );

  _renderer->SetCameraPose(vPos, vFor, vUp);
}

void ARVisualizer::SetCameraPose(double position[3], double orientation[3][3])
{
  if (!IsRunning()) { return; }
  glm::vec3 vPos = glm::vec3( position[0], position[1], position[2] );
  glm::vec3 vFor = glm::vec3( 1.0, 0.0, 0.0 );
  glm::vec3 vUp  = glm::vec3( 0.0, 0.0, 1.0 );

  // fill rotation matrix from given orientation
  glm::mat3 r(1.0);
  for (size_t i = 0; i < 3; i++)
  {
    for (size_t j = 0; j < 3; j++)
    {
      r[i][j] = orientation[i][j];
    }
  }

  // rotate forward & up vectors
  vFor = r * vFor;
  vUp  = r * vUp;

  _renderer->SetCameraPose(vPos, vFor, vUp);
}

void ARVisualizer::SetCameraIntrinsics(double camera_matrix[3][3])
{
  if (!IsRunning()) { return; }
  _renderer->SetCameraIntrinsics(camera_matrix);
}

mesh_handle ARVisualizer::AddTriangle(double vertexPositions[3][3], Color color)
{
  if (!IsRunning()) { return 0; }
  Vector<glm::vec3> positions = {
    { vertexPositions[0][0], vertexPositions[0][1], vertexPositions[0][2] },
    { vertexPositions[1][0], vertexPositions[1][1], vertexPositions[1][2] },
    { vertexPositions[2][0], vertexPositions[2][1], vertexPositions[2][2] }
  };

  return _renderer->Add3DMesh(MeshFactory::MakeTriangle<Vertex3D>(positions), std::make_shared<FlatColorMaterial>(color));
}

mesh_handle ARVisualizer::AddQuad(double center[3], double normal[3], double width, double height, Color color)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( center[0], center[1], center[2] );
  glm::vec3 vNormal = glm::vec3( normal[0], normal[1], normal[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeQuad<Vertex3D>(vCenter, vNormal, width, height), std::make_shared<FlatColorMaterial>(color));
}

mesh_handle ARVisualizer::AddSphere(double center[3], double radius, Color color)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( center[0], center[1], center[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeUVSphere<Vertex3D>(vCenter, radius, UVSPHERE_RESOLUTION), std::make_shared<FlatColorMaterial>(color));
}

mesh_handle ARVisualizer::AddCapsule(double center1[3], double center2[3], double radius, Color color)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter1 = glm::vec3( center1[0], center1[1], center1[2] );
  glm::vec3 vCenter2 = glm::vec3( center2[0], center2[1], center2[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeCapsule<Vertex3D>(vCenter1, vCenter2, radius, UVSPHERE_RESOLUTION), std::make_shared<FlatColorMaterial>(color));
}

mesh_handle ARVisualizer::AddEllipsoid(float* center, float* transformation, double radius, Color color)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( center[0], center[1], center[2] );

  Mesh<Vertex3D> mesh = MeshFactory::MakeUVSphere<Vertex3D>(vCenter, radius, UVSPHERE_RESOLUTION);

  float transform[16]
    {
      transformation[6],transformation[7],transformation[8], 0,
      transformation[3],transformation[4],transformation[5], 0,
      transformation[0],transformation[1],transformation[2], 0,
      center[0], center[1], center[2], 1,
    };

  mesh.SetTransform(glm::make_mat4(transform));
  return _renderer->Add3DMesh(mesh, std::make_shared<FlatColorMaterial>(color));
}

void ARVisualizer::Remove(mesh_handle handle)
{
  if (!IsRunning()) { return; }
  _renderer->RemoveMesh(handle);
}

void ARVisualizer::RemoveAll()
{
  if (!IsRunning()) { return; }
  _renderer->RemoveAllMeshes();
}

void ARVisualizer::DrawPointCloud(const void* pointData, unsigned long numPoints, PointCloudDataType dataType)
{
  if (!IsRunning()) { return; }

  switch (dataType)
  {
    case PCL_PointXYZ:
      break;
    case PCL_PointXYZRGBA:
      throw std::runtime_error("PCL_PointXYZRGBA is not supported yet.");
  }

  _renderer->DrawPointCloud(pointData, numPoints);
}

void ARVisualizer::DrawVoxels(const ARVisualizer::Voxel* voxels, unsigned long numVoxels)
{
  if (!IsRunning()) { return; }
  _renderer->DrawVoxels(voxels, numVoxels);
}

IUIWindow* ARVisualizer::AddUIWindow(const char* name)
{
  return _ui->AddWindow(name);
}

IUIWindow* ARVisualizer::AddUIWindow(const char* name, float initialWidth, float initialHeight)
{
  return _ui->AddWindow(name, initialWidth, initialHeight);
}

void ARVisualizer::renderExternGUI()
{
  _ui->draw();
}

} // namespace ar
