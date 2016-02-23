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
#include <thread>
#include <mutex>
#include <atomic>
#include "RenderPassParams.hpp"
#include "ShaderSources.g.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "mesh/VertexBuffer.hpp"
#include "mesh/InstancedVertexBuffer.hpp"
#include "mesh/Mesh.hpp"
#include "pointcloud/PointCloud.hpp"
#include "windowmanager/GLFWWindowEvents.hpp"
#include "windowmanager/WindowManager.hpp"
#include "ImguiRenderer.hpp"
#include "Camera.hpp"
#include "geometry/Voxel.hpp"

namespace ar
{

// define vertex format for 2D & 3D shapes here for convenience
typedef VertexP3N3 Vertex3D;
typedef VertexP2T2 Vertex2D;
typedef Mesh<Vertex3D> Mesh3D;
typedef TexturedMesh<VertexP2T2> Mesh2D;


class Renderer
{
public:
  Renderer(GLFWwindow* window);

  ~Renderer();

  // creates a new thread, which makes the OpenGL context and begins rendering
  void Start();

  // notifies the rendering thread to stop, and waits for it to exit
  void Stop();

  // returns True if we're currently rendering
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

  // Updates camera parameters with the given values
  void SetCameraPose(glm::vec3 position, glm::vec3 forward, glm::vec3 up);

  unsigned int Add3DMesh(Mesh3D mesh, SharedPtr<Material> material);

  unsigned int AddPointCloud(const void* pointData, size_t numPoints);

  void UpdatePointCloud(unsigned int handle, const void* pointData, size_t numPoints);

  void Update(unsigned int handle, Mesh3D mesh, SharedPtr<Material> material);

  void RemoveMesh(unsigned int handle);

  void RemoveAllMeshes();

  void DrawVoxels(const Voxel* voxels, size_t numVoxels);

  // Gets the View matrix
  glm::mat4 GetViewMatrix() const
  {
    return _camera.GetViewMatrix();
  }

  glm::mat4 GetProjectionMatrix() const
  {
    return _projectionMatrix;
  }

  void SetCameraIntrinsics(double camera_matrix[3][3])
  {
      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          _cameraMatrix[i][j] = camera_matrix[i][j];
        }
      }

      _useCameraIntrinsics = true;
      UpdateProjection();
  }

  // Updates projection as needed when the window changes size
  void onWindowResized(int newWidth, int newHeight);

  // Updates the GL viewport when the framebuffer changes size
  void onFramebufferResized(int newWidth, int newHeight);

private:
  std::atomic_bool _running {false};

  std::mutex _mutex;

  GLFWWindowEvents _windowEvents;
  GLFWwindow* _window;
  int _windowWidth, _windowHeight;

  ImguiRenderer _imguiRenderer;
  Camera _camera;

  std::thread _renderThread;

  struct camera_parms {
      float fov          = 45.0f;  // field of view, in degrees
      float nearClip     = 0.1f;   // distance to near clipping plane
      float farClip      = 10000.0f; // distance to far clipping plane
  } _camera_params;

  GLuint _renderType = GL_TRIANGLES;

  glm::mat3 _cameraMatrix;
  glm::mat4 _projectionMatrix;
  bool _useCameraIntrinsics = false;

    /// TODO: move this to its own class
  UniquePtr<unsigned char[]> _currentVideoFrame;
  int _videoWidth, _videoHeight;
  ShaderProgram _videoShader;
  std::atomic_bool _newVideoFrame {false};
  glm::vec3 light_dir = glm::vec3(-1.0f, 1.0f, 0.0f);

  ShaderProgram _defaultShader;
  Vector<Mesh2D> _2DMeshes;
  UniquePtr<VertexBuffer<Vertex2D> > _2DMeshBuffer;

  Vector<Mesh3D> _3DMeshes;
  Vector<Mesh3D> _new3DMeshes; // TODO: This is a temporary workaround and should be removed later
  UniquePtr<VertexBuffer<Vertex3D> > _3DMeshBuffer;

  Vector<PointCloud<VertexP4>> _pointClouds;
  ShaderProgram _pointCloudShader;

  ShaderProgram _voxelShader;
  UniquePtr<InstancedVertexBuffer<VertexP3N3, VertexP3C4S>> _voxelInstancedVertexBuffer;

  unsigned int GenerateMeshHandle();

  // Regenerates projection matrix
  void UpdateProjection();

  // Sets up the OpenGL context & initializes data needed for rendering
  void init();

  // handles OpenGL context binding and default rendering settings
  void init_GL();

  // sets up vertex data for objects we know we need (e.g. a plane to render the video on)
  void init_geometry();   /// TODO: Move this to another class

  // allocates and initializes default textures
  void init_textures();   /// TODO: Move this to another class

  // sends the data in pixels to the texture unit on the GPU referenced by tex
  void bufferTexture(int width, int height, GLuint tex, unsigned char* pixels);

  // Prepares OpenGL state for rendering with the given parameters
  void enableRenderPass(RenderPassParams pass);

  // the main render loop
  void render();

  // checks for new mesh data, video data, meshes which should be deleted, etc.
  void update();

  // renders just one frame
  void renderOneFrame();

  // renders the GUI
  void renderGUI();

  // Final cleanup which needs to be done (from the render thread) when we stop rendering
  void shutdown();

};


} // namespace ar

#endif // _ARRENDERER_H
