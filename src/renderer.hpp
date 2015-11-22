#ifndef _ARRENDERER_H
#define _ARRENDERER_H

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <vector>
#include <thread>
#include <mutex>
#include "ShaderProgram.hpp"
#include "mesh/vertexbuffer.hpp"
#include "mesh/mesh.hpp"


namespace ar
{

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

private:
  bool _running = false;

  std::mutex _mutex;

  GLFWwindow* _window;
  int _width, _height;

  std::thread _renderThread;

    /// TODO: move this to its own class
  unsigned char* _currentVideoFrame;
  ShaderProgram _videoShader;
  bool _newVideoFrame = false;

  std::vector<TexturedQuad> _2DQuads;
  VertexBuffer<VertexP2T2>* _quadsBuffer;

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
