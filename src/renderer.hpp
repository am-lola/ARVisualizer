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

#include <vector>
#include <thread>
#include <mutex>
#include "ShaderProgram.hpp"
#include "mesh/vertexbuffer.hpp"
#include "mesh/mesh.hpp"


namespace ar
{

// define vertex format for 2D & 3D shapes here for convenience
typedef VertexP3C3 Vertex3D;
typedef VertexP2T2 Vertex2D;
typedef Mesh<Vertex3D> Mesh3D;
typedef TexturedQuad Mesh2D;

class Renderer
{
public:
  Renderer() : Renderer(1024, 768) {};
  Renderer(int windowWidth, int windowHeight);

  ~Renderer();

  // creates a new thread, which makes the OpenGL context and begins rendering
  void Start();

  // notifies the rendering thread to stop, and waits for it to exit
  void Stop();

  // returns True if we're currently rendering
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

  unsigned int Add3DMesh(Mesh3D mesh);

  // Gets the View matrix
  glm::mat4 GetViewMatrix()
  {
    return glm::lookAt( // should probably cache this and only update it when it changes
      _camera.position,
      _camera.forward,
      _camera.up
    );
  }

  glm::mat4 GetProjectionMatrix()
  {
    return glm::perspective( // this one, too
      _camera.fov,
      (float)_width / (float)_height,
      _camera.nearClip,
      _camera.farClip
    );
  }

private:
  bool _running = false;

  std::mutex _mutex;

  GLFWwindow* _window;
  int _width, _height;

  std::thread _renderThread;

  struct camera_parms {
      glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);  // where the camera is
      glm::vec3 forward  = glm::vec3(0.0f, 0.0f, 1.0f);  // where the camera is looking
      glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);  // "up" from camera's perspective (orthogonal to forward)
      float fov          = 45.0f;  // field of view, in degrees
      float nearClip     = 0.1f;   // distance to near clipping plane
      float farClip      = 100.0f; // distance to far clipping plane
  } _camera;

    /// TODO: move this to its own class
  unsigned char* _currentVideoFrame;
  ShaderProgram _videoShader;
  bool _newVideoFrame = false;

  ShaderProgram _defaultShader;
  std::vector<TexturedQuad> _2DMeshes;
  VertexBuffer<Vertex2D>* _2DMeshBuffer;

  std::vector<Mesh<VertexP3C3> > _3DMeshes;
  VertexBuffer<Vertex3D>* _3DMeshBuffer;

  // Sets up the OpenGL context & initializes data needed for rendering
  void init(int windowWidth, int windowHeight);

  // handles OpenGL context creation and window creation
  void init_GL(int windowWidth, int windowHeight);

  // sets up vertex data for objects we know we need (e.g. a plane to render the video on)
  void init_geometry();   /// TODO: Move this to another class

  // allocates and initializes default textures
  void init_textures();   /// TODO: Move this to another class

  // sends the data in pixels to the texture unit on the GPU referenced by tex
  void bufferTexture(int width, int height, GLuint tex, unsigned char* pixels);

  // the main render loop
  void render();

  // renders just one frame
  void renderOneFrame();

  // Final cleanup which needs to be done (from the render thread) when we stop rendering
  void shutdown();
};

} // namespace ar

#endif // _ARRENDERER_H
