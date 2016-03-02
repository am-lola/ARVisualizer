#include "ARVisualizer.hpp"
#include "Renderer.hpp"
#include "mesh/MeshFactory.hpp"
#include "windowmanager/WindowManager.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace ar
{

ARVisualizer::ARVisualizer()
{
  _renderer = nullptr;
}

ARVisualizer::~ARVisualizer()
{
  if (IsRunning())
  {
    Stop();
  }
}

void ARVisualizer::Start()
{
  Start("AR Visualizer", 1024, 768);
}

void ARVisualizer::Start(const char* name)
{
  Start(name, 1024, 768);
}

void ARVisualizer::Start(int width, int height)
{
  Start("AR Visualizer", width, height);
}

void ARVisualizer::Start(const char* name, int width, int height)
{
  _renderer = WindowManager::Instance().NewRenderer(width, height, name);
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

mesh_handle ARVisualizer::Add(Triangle t)
{
  if (!IsRunning()) { return 0; }
  Vector<glm::vec3> positions = {
    { t.p1[0], t.p1[1], t.p1[2] },
    { t.p2[0], t.p2[1], t.p2[2] },
    { t.p3[0], t.p3[1], t.p3[2] }
  };

  return _renderer->Add3DMesh(MeshFactory::MakeTriangle<Mesh<Vertex3D>>(positions), std::make_shared<FlatColorMaterial>(t.color));
}

mesh_handle ARVisualizer::Add(Quad quad)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( quad.center[0], quad.center[1], quad.center[2] );
  glm::vec3 vNormal = glm::vec3( quad.normal[0], quad.normal[1], quad.normal[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeQuad<Mesh<Vertex3D>>(vCenter, vNormal, quad.width, quad.height), std::make_shared<FlatColorMaterial>(quad.color));
}

mesh_handle ARVisualizer::Add(Polygon polygon)
{
  if (!IsRunning()) { return 0; }
  Vector<glm::vec3> points;
  for(int i = 0; i < polygon.numPoints * 3; i += 3)
  {
    points.push_back({ polygon.points[i], polygon.points[i+1], polygon.points[i+2] });
  }

  return _renderer->Add3DMesh(MeshFactory::MakeTriangleFan<Mesh<Vertex3D>>(points), std::make_shared<FlatColorMaterial>(polygon.color));
}

mesh_handle ARVisualizer::Add(PolyMesh mesh)
{
  if (!IsRunning()) { return 0; }
  Vector<glm::vec3> vertices;
  Vector<glm::vec3> normals;
  Vector<GLuint> indices;

  // get vertices & normals (if any)
  for (int i = 0; i < mesh.numPoints * 3; i += 3)
  {
    vertices.push_back({ mesh.points[i], mesh.points[i+1], mesh.points[i+2] });

    if (mesh.normals != NULL)
    {
      normals.push_back({ mesh.normals[i], mesh.normals[i+1], mesh.normals[i+2] });
    }
  }

  // if indices were provided, collect them, otherwise assume all vertices are to be used in order
  if (mesh.numIndices > 0)
  {
    for (int i = 0; i < mesh.numIndices; i++)
    {
      indices.push_back(mesh.indices[i]);
    }
  }
  else
  {
    for (int i = 0; i < mesh.numPoints; i++)
    {
      indices.push_back(i);
    }
  }

  return _renderer->Add3DMesh(MeshFactory::MakeTriangleMesh<Mesh<Vertex3D>>(vertices, indices, normals), std::make_shared<FlatColorMaterial>(mesh.color));
}

mesh_handle ARVisualizer::Add(Box box)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( box.center[0], box.center[1], box.center[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeBox<Mesh<Vertex3D>>(vCenter, box.sizeX, box.sizeY, box.sizeZ), std::make_shared<FlatColorMaterial>(box.color));
}

mesh_handle ARVisualizer::Add(Cube cube)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( cube.center[0], cube.center[1], cube.center[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeCube<Mesh<Vertex3D>>(vCenter, cube.size), std::make_shared<FlatColorMaterial>(cube.color));
}

mesh_handle ARVisualizer::Add(Sphere sphere)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( sphere.center[0], sphere.center[1], sphere.center[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeUVSphere<Mesh<Vertex3D>>(vCenter, sphere.radius, UVSPHERE_RESOLUTION), std::make_shared<FlatColorMaterial>(sphere.color));
}

mesh_handle ARVisualizer::Add(Capsule capsule)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter1 = glm::vec3( capsule.center1[0], capsule.center1[1], capsule.center1[2] );
  glm::vec3 vCenter2 = glm::vec3( capsule.center2[0], capsule.center2[1], capsule.center2[2] );

  return _renderer->Add3DMesh(MeshFactory::MakeCapsule<Mesh<Vertex3D>>(vCenter1, vCenter2, capsule.radius, UVSPHERE_RESOLUTION), std::make_shared<FlatColorMaterial>(capsule.color));
}

mesh_handle ARVisualizer::Add(Ellipsoid ellipsoid)
{
  if (!IsRunning()) { return 0; }
  glm::vec3 vCenter = glm::vec3( ellipsoid.center[0], ellipsoid.center[1], ellipsoid.center[2] );

  Mesh<Vertex3D> mesh = MeshFactory::MakeUVSphere<Mesh<Vertex3D>>(vCenter, ellipsoid.radius, UVSPHERE_RESOLUTION);

  float* t = ellipsoid.transform;

  float transform[16]
  {
    t[6],                t[7],                t[8],                0,
    t[3],                t[4],                t[5],                0,
    t[0],                t[1],                t[2],                0,
    ellipsoid.center[0], ellipsoid.center[1], ellipsoid.center[2], 1,
  };

  mesh.SetTransform(glm::make_mat4(transform));
  return _renderer->Add3DMesh(mesh, std::make_shared<FlatColorMaterial>(ellipsoid.color));
}

mesh_handle ARVisualizer::Add(PointCloudData pointcloud)
{
  if (!IsRunning()) { return 0; }

  switch (pointcloud.type)
  {
    case PCL_PointXYZ:
      break;
    case PCL_PointXYZRGBA:
      throw std::runtime_error("PCL_PointXYZRGBA is not supported yet.");
  }

  return _renderer->AddPointCloud(pointcloud.pointData, pointcloud.numPoints, pointcloud.color);
}

void ARVisualizer::Update(mesh_handle handle, Triangle t)
{
  if (!IsRunning()) { return; }
  Vector<glm::vec3> positions = {
    { t.p1[0], t.p1[1], t.p1[2] },
    { t.p2[0], t.p2[1], t.p2[2] },
    { t.p3[0], t.p3[1], t.p3[2] }
  };

  _renderer->Update(handle, MeshFactory::MakeTriangle<Mesh<Vertex3D>>(positions), std::make_shared<FlatColorMaterial>(t.color));
}

void ARVisualizer::Update(mesh_handle handle, Quad quad)
{
  if (!IsRunning()) { return; }
  glm::vec3 vCenter = glm::vec3( quad.center[0], quad.center[1], quad.center[2] );
  glm::vec3 vNormal = glm::vec3( quad.normal[0], quad.normal[1], quad.normal[2] );

  _renderer->Update(handle, MeshFactory::MakeQuad<Mesh<Vertex3D>>(vCenter, vNormal, quad.width, quad.height), std::make_shared<FlatColorMaterial>(quad.color));
}

void ARVisualizer::Update(mesh_handle handle, Polygon polygon)
{
  if (!IsRunning()) { return; }
  Vector<glm::vec3> points;
  for(int i = 0; i < polygon.numPoints * 3; i += 3)
  {
    points.push_back({ polygon.points[i], polygon.points[i+1], polygon.points[i+2] });
  }

  _renderer->Update(handle, MeshFactory::MakeTriangleFan<Mesh<Vertex3D>>(points), std::make_shared<FlatColorMaterial>(polygon.color));
}

void ARVisualizer::Update(mesh_handle handle, PolyMesh mesh)
{
  if (!IsRunning()) { return; }
  Vector<glm::vec3> vertices;
  Vector<glm::vec3> normals;
  Vector<GLuint> indices;

  // get vertices
  for (int i = 0; i < mesh.numPoints * 3; i += 3)
  {
    vertices.push_back({ mesh.points[i], mesh.points[i+1], mesh.points[i+2] });

    if (mesh.normals != NULL)
    {
      normals.push_back({ mesh.normals[i], mesh.normals[i+1], mesh.normals[i+2] });
    }
  }

  // if indices were provided, collect them, otherwise assume all vertices are to be used in order
  if (mesh.numIndices > 0)
  {
    for (int i = 0; i < mesh.numIndices; i++)
    {
      indices.push_back(mesh.indices[i]);
    }
  }
  else
  {
    for (int i = 0; i < mesh.numPoints; i++)
    {
      indices.push_back(i);
    }
  }

  _renderer->Update(handle, MeshFactory::MakeTriangleMesh<Mesh<Vertex3D>>(vertices, indices, normals), std::make_shared<FlatColorMaterial>(mesh.color));
}

void ARVisualizer::Update(mesh_handle handle, Box box)
{
  if (!IsRunning()) { return; }
  glm::vec3 vCenter = glm::vec3( box.center[0], box.center[1], box.center[2] );

  _renderer->Update(handle, MeshFactory::MakeBox<Mesh<Vertex3D>>(vCenter, box.sizeX, box.sizeY, box.sizeZ), std::make_shared<FlatColorMaterial>(box.color));
}

void ARVisualizer::Update(mesh_handle handle, Cube cube)
{
  if (!IsRunning()) { return; }
  glm::vec3 vCenter = glm::vec3( cube.center[0], cube.center[1], cube.center[2] );

  _renderer->Update(handle, MeshFactory::MakeCube<Mesh<Vertex3D>>(vCenter, cube.size), std::make_shared<FlatColorMaterial>(cube.color));
}

void ARVisualizer::Update(mesh_handle handle, Sphere sphere)
{
  if (!IsRunning()) { return; }
  glm::vec3 vCenter = glm::vec3( sphere.center[0], sphere.center[1], sphere.center[2] );

  _renderer->Update(handle, MeshFactory::MakeUVSphere<Mesh<Vertex3D>>(vCenter, sphere.radius, UVSPHERE_RESOLUTION), std::make_shared<FlatColorMaterial>(sphere.color));
}

void ARVisualizer::Update(mesh_handle handle, Capsule capsule)
{
  if (!IsRunning()) { return; }
  glm::vec3 vCenter1 = glm::vec3( capsule.center1[0], capsule.center1[1], capsule.center1[2] );
  glm::vec3 vCenter2 = glm::vec3( capsule.center2[0], capsule.center2[1], capsule.center2[2] );

  _renderer->Update(handle, MeshFactory::MakeCapsule<Mesh<Vertex3D>>(vCenter1, vCenter2, capsule.radius, UVSPHERE_RESOLUTION), std::make_shared<FlatColorMaterial>(capsule.color));
}

void ARVisualizer::Update(mesh_handle handle, Ellipsoid ellipsoid)
{
  if (!IsRunning()) { return; }
  glm::vec3 vCenter = glm::vec3( ellipsoid.center[0], ellipsoid.center[1], ellipsoid.center[2] );

  Mesh<Vertex3D> mesh = MeshFactory::MakeUVSphere<Mesh<Vertex3D>>(vCenter, ellipsoid.radius, UVSPHERE_RESOLUTION);

  float* t = ellipsoid.transform;

  float transform[16]
  {
    t[6],                t[7],                t[8],                0,
    t[3],                t[4],                t[5],                0,
    t[0],                t[1],                t[2],                0,
    ellipsoid.center[0], ellipsoid.center[1], ellipsoid.center[2], 1,
  };

  mesh.SetTransform(glm::make_mat4(transform));
  _renderer->Update(handle, mesh, std::make_shared<FlatColorMaterial>(ellipsoid.color));
}

void ARVisualizer::Update(mesh_handle handle, PointCloudData pointcloud)
{
  if (!IsRunning()) { return; }

  _renderer->UpdatePointCloud(handle, pointcloud.pointData, pointcloud.numPoints, pointcloud.color);
}

void ARVisualizer::Update(mesh_handle handle, ar::Transform transform, bool absolute)
{
  if (!IsRunning()) { return; }
  glm::mat4 transformMat = glm::mat4(
    transform.rotation[0][0], transform.rotation[1][0], transform.rotation[2][0], 0,
    transform.rotation[0][1], transform.rotation[1][1], transform.rotation[2][1], 0,
    transform.rotation[0][2], transform.rotation[1][2], transform.rotation[2][2], 0,
    transform.translation[0], transform.translation[1], transform.translation[2], 1
  );

  _renderer->UpdateTransform(handle, transformMat, absolute);
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

void ARVisualizer::DrawVoxels(const Voxel* voxels, unsigned long numVoxels)
{
  if (!IsRunning()) { return; }
  _renderer->DrawVoxels(voxels, numVoxels);
}
} // namespace ar
