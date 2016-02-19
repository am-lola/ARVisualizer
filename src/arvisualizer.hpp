#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

#include "color.hpp"
#include "triangle.hpp"
#include "quad.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "capsule.hpp"
#include "ellipsoid.hpp"
#include "pointclouddata.hpp"
#include "voxel.hpp"

namespace ar
{

// Number of times to subdivide icosphere meshes (larger == smoother spheres)
// This should be a non-negative number between 0 and 5. Smaller is faster.
#define ICOSPHERE_SUBDIV_LEVELS 2

// Number of vertices around major axis to generate on UV sphere-based meshes.
// This should be a non-negative even number. Smaller is faster, larger is smoother.
#define UVSPHERE_RESOLUTION 16

class Renderer;

typedef unsigned int mesh_handle;

/*
  The main interface to the AR Visualization library
*/

class ARVisualizer
{
public:
  ARVisualizer();
  ~ARVisualizer();

  // Begins rendering
  void Start(int width, int height);
  void Start();

  // Stops all rendering activity
  void Stop();

  // True if the visualizer has been started
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

  // Updates the camera parameters used for rendering. Values should correspond
  // as closely as possible to the physical position & orientation of the camera
  //    Position: Position of the camera in world-coordinates
  //    Forward:  Vector pointing in the direction the camera is facing
  //    Up:       Orthogonal to Forward, defines the vertical axis for the camera
  void SetCameraPose(double position[3], double forward[3], double up[3]);

  //    Position:    Position of the camera in world-coordinates
  //    Orientation: Rotation matrix describing the current orientation of the camera
  void SetCameraPose(double position[3], double orientation[3][3]);

  // Updates the projection based on the given camera matrix
  void SetCameraIntrinsics(double camera_matrix[3][3]);

  // Adds an object to the scene
  mesh_handle Add(Triangle triangle);
  mesh_handle Add(Quad quad);
  mesh_handle Add(Box box);
  mesh_handle Add(Cube cube);
  mesh_handle Add(Sphere sphere);
  mesh_handle Add(Capsule capsule);
  mesh_handle Add(Ellipsoid ellipsoid);
  mesh_handle Add(PointCloudData pointcloud);

  // Updates an existing scene object
  void Update(mesh_handle handle, Triangle triangle);
  void Update(mesh_handle handle, Quad quad);
  void Update(mesh_handle handle, Sphere sphere);
  void Update(mesh_handle handle, Capsule capsule);
  void Update(mesh_handle handle, Ellipsoid ellipsoid);
  void Update(mesh_handle handle, PointCloudData pointcloud);

  // Removes an object from the scene
  void Remove(mesh_handle handle);

  // Removes all objects
  void RemoveAll();

  void DrawVoxels(const Voxel* voxels, unsigned long numVoxels);

private:
  Renderer* _renderer;
};

} // namespace ar

#endif // _ARVISUALIZER_H
