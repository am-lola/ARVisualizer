#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

#include "color.hpp"

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

  mesh_handle AddTriangle(double vertexPositions[3][3], Color color);

  mesh_handle AddQuad(double center[3], double normal[3], double width, double height, Color color);

  mesh_handle AddSphere(double center[3], double radius, Color color);

  mesh_handle AddCapsule(double center1[3], double center2[3], double radius, Color color);

  //    Transformation: Column-major 3x3 matrix with orthogonal axes as columns, ordered by descending size for best results.
  mesh_handle AddEllipsoid(float center[3], float* transformation, double radius, Color color);

  void Remove(mesh_handle handle);

  void RemoveAll();

  enum PointCloudDataType
  {
    PCL_PointXYZ,
    PCL_PointXYZRGBA
  };

  // Draw a point cloud
  //    Points: Points xyzw (w is ignored, easier because pcl::PointXYZ is aligned to 4 floats).
  //    NumPoints: Number of points in the array.
  void DrawPointCloud(const void* pointData, unsigned long numPoints, PointCloudDataType dataType);

  struct Voxel
  {
    float center[3];
    float color[4];
    float size;
  };

  void DrawVoxels(const Voxel* voxels, unsigned long numVoxels);

private:
  Renderer* _renderer;
};

} // namespace ar

#endif // _ARVISUALIZER_H
