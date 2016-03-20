#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

#include "ui.hpp"
#include "geometry/Color.hpp"
#include "geometry/Transform.hpp"
#include "geometry/Triangle.hpp"
#include "geometry/Quad.hpp"
#include "geometry/Box.hpp"
#include "geometry/Sphere.hpp"
#include "geometry/Capsule.hpp"
#include "geometry/Ellipsoid.hpp"
#include "geometry/Polygon.hpp"
#include "geometry/PolyMesh.hpp"
#include "geometry/PointCloudData.hpp"
#include "geometry/Voxel.hpp"
#include "geometry/Line.hpp"

namespace ar
{

// Number of times to subdivide icosphere meshes (larger == smoother spheres)
// This should be a non-negative number between 0 and 5. Smaller is faster.
#define ICOSPHERE_SUBDIV_LEVELS 2

// Number of vertices around major axis to generate on UV sphere-based meshes.
// This should be a non-negative even number. Smaller is faster, larger is smoother.
#define UVSPHERE_RESOLUTION 16

class Renderer;
class UserInterface;

typedef unsigned int mesh_handle;
typedef unsigned int ui_element_handle;

/*
  The main interface to the AR Visualization library
*/
class ARVisualizer
{
public:
  ARVisualizer();
  ~ARVisualizer();

  // Opens a window and begins rendering
  void Start();

  // Opens a window and begins rendering
  // @name Name of the window to be created
  void Start(const char* name);

  // Opens a window and begins rendering
  // @width  width, in pixels, of the window
  // @height height, in pixels, of the window
  void Start(int width, int height);

  // Opens a window and begins rendering
  // @name   Name of the window to be created
  // @width  width, in pixels, of the window
  // @height height, in pixels, of the window
  void Start(const char* name, int width, int height);

  // Stops all rendering activity
  void Stop();

  // Checks if the visualizer has been started successfully
  //
  // @return True if the visualizer has been started, False otherwise
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  // @width  width, in pixels, of the image
  // @height height, in pixles, of the image
  // @pixels image data
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

  // Updates the camera parameters used for rendering.
  // @position Position of the camera in world-coordinates
  // @forward  Vector pointing in the direction the camera is facing
  // @up       Orthogonal to Forward, defines the vertical axis for the camera
  void SetCameraPose(double position[3], double forward[3], double up[3]);

  // Updates the camera parameters used for rendering.
  // @position    Position of the camera in world-coordinates
  // @orientation Rotation matrix describing the current orientation of the camera
  void SetCameraPose(double position[3], double orientation[3][3]);

  // Updates the camera projection matrix
  // @camera_matrix camera intrinsic parameters
  void SetCameraIntrinsics(double camera_matrix[3][3]);

  // Adds a <Triangle> to the scene
  // @triangle Struct describing the triangle
  //
  // @return   Handle which can be used to update or remove the object in the future
  mesh_handle Add(Triangle triangle);

  // Adds a <Quad> to the scene
  // @quad   Struct describing the quad
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Quad quad);

  // Adds a <Polygon> to the scene
  // @polygon Struct describing the polygon
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Polygon polygon);

  // Adds a <PolyMesh> to the scene
  // @mesh Struct describing the mesh
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(PolyMesh mesh);

  // Adds a <Box> to the scene
  // @box Struct describing the box
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Box box);

  // Adds a <Cube> to the scene
  // @cube Struct describing the cube
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Cube cube);

  // Adds a <Sphere> to the scene
  // @sphere Struct describing the sphere
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Sphere sphere);

  // Adds a <Capsule> to the scene
  // @capsule Struct describing the capsule
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Capsule capsule);

  // Adds an <Ellipsoid> to the scene
  // @ellipsoid Struct describing the ellipsoid
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(Ellipsoid ellipsoid);

  // Adds a <Line> to the scene
  // @Line Struct describing the line
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(LinePath linePath);

  // Adds a PointCloud to the scene
  // @pointcloud Struct describing the PointCloud
  //
  // @return Handle which can be used to update or remove the object in the future
  mesh_handle Add(PointCloudData pointcloud);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @triangle <Triangle> to replace the object with
  void Update(mesh_handle handle, Triangle triangle);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @quad <Quad> to replace the object with
  void Update(mesh_handle handle, Quad quad);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @polygon <Polygon> to replace the object with
  void Update(mesh_handle handle, Polygon polygon);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @mesh <PolyMesh> to replace the object with
  void Update(mesh_handle handle, PolyMesh mesh);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @box <Box> to replace the object with
  void Update(mesh_handle handle, Box box);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @cube <Cube> to replace the object with
  void Update(mesh_handle handle, Cube cube);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @sphere <Sphere> to replace the object with
  void Update(mesh_handle handle, Sphere sphere);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @capsule <Capsule> to replace the object with
  void Update(mesh_handle handle, Capsule capsule);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @ellipsoid <Ellipsoid> to replace the object with
  void Update(mesh_handle handle, Ellipsoid ellipsoid);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @linePath <LinePath> to replace the object with
  void Update(mesh_handle handle, LinePath linePath);

  // Updates an existing object to match the given shape
  // @handle <mesh_handle> for the object to be updated
  // @pointcloud PointCloud to replace the object with
  void Update(mesh_handle handle, PointCloudData pointcloud);

  // Updates an existing object's position and/or orientation
  // @handle    <mesh_handle> for the object to be updated
  // @transform <Transform> to apply to the object
  // @absolute  If true, object will be transformed to match the given values.
  //            If false, the transform will be added to the object's current transform.
  void Update(mesh_handle handle, ar::Transform transform, bool absolute);

  // Removes an object from the scene
  // @handle <mesh_handle> for the object to be removed
  void Remove(mesh_handle handle);

  // Removes all objects from the scene
  void RemoveAll();

  void DrawVoxels(const Voxel* voxels, unsigned long numVoxels);

  // Adds a GUI window to the current windw
  // @name Name of the GUI window
  IUIWindow* AddUIWindow(const char* name);

  // Adds a GUI window to the current window
  // @name Name of the GUI window
  IUIWindow* AddUIWindow(const char* name, float initialWidth, float initialHeight);

private:
  Renderer* _renderer;
  UserInterface* _ui;

  // Renders GUI elements provided by the host application
  void renderExternGUI();
};

} // namespace ar

#endif // _ARVISUALIZER_H
