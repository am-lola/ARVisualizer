#ifndef _ARRENDERER_H
#define _ARRENDERER_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include "RenderPassParams.hpp"
#include "ShaderSources.g.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "mesh/Mesh.hpp"
#include "windowmanager/GLFWWindowEvents.hpp"
#include "windowmanager/WindowManager.hpp"
#include "ImguiRenderer.hpp"
#include "Camera.hpp"
#include "geometry/Voxel.hpp"
#include "common.hpp"

#include "rendering/MeshRendering.hpp"
#include "rendering/VideoRendering.hpp"
#include "rendering/PointCloudRendering.hpp"
#include "rendering/VoxelRendering.hpp"
#include "rendering/LineRendering.hpp"

namespace ar
{

class RenderCommand
{
public:
  virtual ~RenderCommand() { }

  // this executes on the rendering thread
  virtual void execute() = 0;
};

class CommandQueue
{
public:

  void Enqueue(RenderCommand* renderCommand)
  {
    MutexLockGuard guard(_mutex);
    _renderCommands.push(renderCommand);
  }

  RenderCommand* Dequeue()
  {
    MutexLockGuard guard(_mutex);
    RenderCommand* last = _renderCommands.front();
    _renderCommands.pop();
    return last;
  }

  size_t NumEnqueuedCommands()
  {
    MutexLockGuard guard(_mutex);
    return _renderCommands.size();
  }

private:

  std::mutex _mutex;
  std::queue<RenderCommand*> _renderCommands;
};

class Renderer
{
private:

  template <typename T>
  class RenderCommandAddMesh;
  class RenderCommandAddLineMesh;
  class RenderCommandUpdateTransform;
  class RenderCommandUpdateMesh;
  class RenderCommandUpdateLineMesh;
  class RenderCommandRemoveMesh;
  class RenderCommandRemoveAllMeshes;
  class RenderCommandNotifyNewVideoFrame;
  class RenderCommandAddPointCloud;
  class RenderCommandUpdatePointCloud;
  class RenderCommandDrawVoxels;

public:
  // Constructor
  // @window The window holding the OpenGL context we should use
  Renderer(GLFWwindow* window);

  ~Renderer();

  // creates a new thread, which takes the OpenGL context and begins rendering
  void Start();

  // notifies the rendering thread to stop, and waits for it to exit
  void Stop();

  // Checks to see if the Renderer is currently rendering
  //
  // @return True if we're currently rendering.
  //         False otherwise.
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels
  // @width  Width, in pixels, of the image
  // @height Height, in pixels, of the image
  // @pixels Image data, ordered [r,g,b,r,g,b,...]
  void NotifyNewVideoFrame(unsigned int width, unsigned int height, unsigned char* pixels); // TODO: determine if any other pixel formats need to be supported

  // Updates camera parameters with the given values
  // @position New camera position
  // @forward  Vector pointing in the direction the camera is facing
  // @up       Vector pointing "up" relative to the camera's viewpoint (usually orthogonal to forward)
  void SetCameraPose(glm::vec3 position, glm::vec3 forward, glm::vec3 up);

  // Adds a new mesh to the scene
  // @mesh     The mesh to add
  // @material The material to apply to the mesh
  //
  // @return   An <ar::mesh_handle> for <mesh>
  unsigned int Add3DMesh(const Mesh3D& mesh, SharedPtr<Material> material);

  // Adds a new pointcloud to the scene
  // @pointData Pointcloud vertex data
  // @numPoints Number of points in <pointData>
  // @color     A constant color to apply to the cloud
  //
  // @return    An <ar::mesh_handle> for the new <PointCloud>
  unsigned int AddPointCloud(const void* pointData, size_t numPoints, Color color);

  // Updates an existing <LineMesh>
  // @handle   Handle referencing the mesh to update
  // @mesh     New mesh data to replace the old mesh with
  // @material <Material> to apply to the new mesh
  unsigned int AddLineMesh(const LineMesh& mesh, SharedPtr<Material> material);

  // Updates an existing <PointCloud>
  // @handle    Handle referencing the cloud to update
  // @pointData New vertex data to replace the existing points with
  // @numPoints Number of points in <pointData>
  // @color     New color to apply to the cloud
  void UpdatePointCloud(unsigned int handle, const void* pointData, size_t numPoints, Color color);

  // Updates an existing <Mesh3D>
  // @handle   Handle referencing the mesh to update
  // @mesh     New mesh data to replace the old mesh with
  // @material <Material> to apply to the new mesh
  void UpdateMesh(unsigned int handle, const Mesh3D& mesh, SharedPtr<Material> material);

  // Updates an existing <LineMesh>
  // @handle   Handle referencing the mesh to update
  // @mesh     New mesh data to replace the old mesh with
  // @material <Material> to apply to the new mesh
  void UpdateLineMesh(unsigned int handle, const LineMesh& mesh, SharedPtr<Material> material);

  // Transforms an existing mesh object
  // @handle    Handle referencing the object to transform
  // @transform Transformation matrix to apply to the object
  // @absolute  If False, transformation is applied relative to the object's current transformation.
  //            If True, transformation replaces the object's current transformation.
  void UpdateTransform(unsigned int handle, const glm::mat4& transform, bool absolute);

  // Removes an existing mesh from the scene
  // @handle Handle referencing the mesh to remove
  void RemoveMesh(unsigned int handle);

  // Removes all objects previously added to the scene
  void RemoveAllMeshes();

  void DrawVoxels(const Voxel* voxels, size_t numVoxels);

  // Gets the View matrix
  glm::mat4 GetViewMatrix() const
  {
    return _camera.GetViewMatrix();
  }

  // Gets the projection matrix
  glm::mat4 GetProjectionMatrix() const
  {
    return _camera.GetProjectionMatrix();
  }

  // Sets intrinsic parameters to adjust projection based on camera calibration data
  // @camera_matrix 3x3 Camera Intrinsic matrix
  void SetCameraIntrinsics(double camera_matrix[3][3])
  {
    _camera.SetIntrinsics(camera_matrix);
  }

  Delegate<void()> _renderGUIDelegate;

  inline void EnqueueRenderCommand(RenderCommand* command)
  {
    _renderCommandQueue.Enqueue(command);
  }

private:

  CommandQueue _renderCommandQueue;

  std::atomic_bool _running {false};
  double _lastFrameTime = 0;

  // used to synchronize between all active rendering threads to work around IMGUI not playing nice with threads
  static std::mutex _renderGUILock;

  GLFWWindowEvents _windowEvents;
  GLFWwindow* _window;
  int _windowWidth, _windowHeight;

  MeshRenderer<Vertex3D> _meshRenderer;
  LineRenderer _lineRenderer;
  VideoRenderer _videoRenderer;
  PointCloudRenderer _pointCloudRenderer;
  VoxelRenderer _voxelRenderer;
  ImguiRenderer _imguiRenderer;
  Camera _camera;

  std::thread _renderThread;

  GLuint _renderType = GL_TRIANGLES;

  glm::vec3 light_dir = glm::vec3(-1.0f, 1.0f, 0.0f);

  ShaderProgram _defaultShader;



  // Generates a unique ID used to reference meshes from external components
  unsigned int GenerateMeshHandle();

  // ! Call from _renderThread only
  // Updates projection as needed when the window changes size
  void OnWindowResized(int newWidth, int newHeight);

  // ! Call from _renderThread only
  // Updates the GL viewport when the framebuffer changes size
  void OnFramebufferResized(int newWidth, int newHeight);

  // ! Call from _renderThread only
  // Sets up the OpenGL context & initializes data needed for rendering
  void Init();

  // ! Call from _renderThread only
  // handles OpenGL context binding and default rendering settings
  void Init_GL();

  // ! Call from _renderThread only
  // Prepares OpenGL state for rendering with the given parameters
  void EnableRenderPass(RenderPassParams pass);

  // ! Call from _renderThread only
  // the main render loop
  void Render();

  // ! Call from _renderThread only
  // checks for new mesh data, video data, meshes which should be deleted, etc.
  void Update();

  // ! Call from _renderThread only
  // renders just one frame
  void RenderOneFrame();

  // ! Call from _renderThread only
  // renders the GUI
  void RenderGUI();

  // ! Call from _renderThread only
  // Final cleanup which needs to be done (from the render thread) when we stop rendering
  void Shutdown();

};


} // namespace ar

#endif // _ARRENDERER_H
