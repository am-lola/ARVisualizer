#include "renderer.hpp"
#include "mesh/vertex.hpp"
#include "mesh/mesh.hpp"

namespace ar
{

Renderer::Renderer(int windowWidth, int windowHeight)
{
  _width = windowWidth;
  _height = windowHeight;
}

Renderer::~Renderer()
{
  if (_running)
  {
    Stop();
  }
  glfwTerminate();
}

void Renderer::Start()
{
  if (_running)
  {
    return;
  }

  _running = true;
  _renderThread = std::thread([&]()
  {
    init(_width, _height);
    render();
  });
}

void Renderer::Stop()
{
  if (!_running)
  {
    return;
  }
  _running = false;
  _renderThread.join();
}

bool Renderer::IsRunning()
{
  return _running;
}

void Renderer::NotifyNewVideoFrame(int width, int height, unsigned char* pixels)
{
  if (!_running)
  {
    throw std::runtime_error("Renderer was not started before being sent data!");
  }

  std::lock_guard<std::mutex> guard(_mutex);

  // if the incoming image is a different size from the last frame we saw,
  // create a new array to match the new frame size
  if (width != _width || height != _height)
  {
    delete(_currentVideoFrame);
    _currentVideoFrame = new unsigned char[width * height * 3];
    _width = width;
    _height = height;
  }

  for (size_t i = 0; i < width * height * 3; i++)
  {
    _currentVideoFrame[i] = pixels[i];
  }

  _newVideoFrame = true;
}

void Renderer::init(int windowWidth, int windowHeight)
{
  std::lock_guard<std::mutex> guard(_mutex);

  // setup OpenGL context and open a window for rendering
  init_GL(windowWidth, windowHeight);

  // init buffers
  _2DMeshBuffer = new VertexBuffer<VertexP2T2>();
  _3DMeshBuffer = new VertexBuffer<VertexP3C3>();

  // load video shaders
  _defaultShader.loadAndLink("shaders/default.vert", "shaders/default.frag");
  _defaultShader.addUniform("MVP");
  _videoShader.loadAndLink("shaders/2D_passthru.vert", "shaders/simpleTexture.frag");
  _videoShader.addUniform("tex");

  // setup default goemetry needed for rendering and send it to OpenGL
  init_geometry();

  // generate texture handles and allocate space for default textures
  init_textures();
}

void Renderer::init_GL(int windowWidth, int windowHeight)
{
  if (!glfwInit())
  {
    throw std::runtime_error("Failed to initialize GLFW!");
  }

  // tell GLFW what we want from OpenGL
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open window & create OpenGL context
  _window = glfwCreateWindow(windowWidth, windowHeight, "AR Visualizer", NULL, NULL);
  if (!_window)
  {
    glfwTerminate();
    throw std::runtime_error("Failed to open GLFW Window!");
  }

  // bind GL context to this thread
  glfwMakeContextCurrent(_window);

  glfwSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0, 0, 0.15, 1);
}

void Renderer::init_geometry()
{
  // prepare video pane
  std::vector<VertexP2T2> videoVertices = {
    //   Position        UV Coords
     { {-1.0f,  1.0f}, {0.0f, 0.0f} },
     { { 1.0f,  1.0f}, {1.0f, 0.0f} },
     { { 1.0f, -1.0f}, {1.0f, 1.0f} },
     { {-1.0f, -1.0f}, {0.0f, 1.0f} }
  };

  std::vector<GLuint> videoIndices = {
      0, 1, 2, // first triangle
      0, 2, 3  // second triangle
  };

  _2DMeshes.push_back(TexturedQuad(videoVertices, videoIndices, &_videoShader));
}

void Renderer::init_textures()
{
    // initialize video texture to be the same size as our window
    _currentVideoFrame = new unsigned char[_width * _height * 3];
    for (size_t i = 0; i < _width * _height * 3; i++)
    {
      _currentVideoFrame[i] = 50;
    }

    GLuint videoTexture;
    glGenTextures(1, &videoTexture);
    bufferTexture(_width, _height, videoTexture, _currentVideoFrame);

    _2DMeshes[0].SetTexture(videoTexture); /// TODO: Don't do this...
}

void Renderer::bufferTexture(int width, int height, GLuint tex, unsigned char* pixels)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RGB,
          width,
          height,
          0,
          GL_RGB,
          GL_UNSIGNED_BYTE,
          pixels
      );

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::render()
{
  while (_running)
  {
    // if we've received a new video frame, send it to the GPU
    if (_newVideoFrame)
    {
      std::lock_guard<std::mutex> guard(_mutex);
      bufferTexture(_width, _height, _2DMeshes[0].GetTexture(), _currentVideoFrame);
      _newVideoFrame = false;
    }

    // if we have any new mesh data, update buffers and send it to the GPU
    for (auto& m : _2DMeshes)
    {
      if (m.Dirty())
      {
        m.SetOffset(_2DMeshBuffer->AddVertices(m.GetVertices()));
        _2DMeshBuffer->AddIndices(m.GetIndices());
        m.ClearDirty();
      }
    }
    if (_2DMeshBuffer->Dirty())
    {
      _2DMeshBuffer->BufferData();
    }
    for (auto& m : _3DMeshes)
    {
      if (m.Dirty())
      {
        m.SetOffset(_3DMeshBuffer->AddVertices(m.GetVertices()));
        _3DMeshBuffer->AddIndices(m.GetIndices());
        m.ClearDirty();
      }
    }
    if (_3DMeshBuffer->Dirty())
    {
      _3DMeshBuffer->BufferData();
    }

    // Render!
    renderOneFrame();

    glfwSwapBuffers(_window);
    glfwPollEvents();
  }

  // if we've stopped rendering, close/destroy the window & cleanup
  shutdown();
}

void Renderer::renderOneFrame()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*************
  * First pass:
  *   render all 2D textured shapes
  *************/
  glDisable(GL_DEPTH_TEST);
  for (auto& m : _2DMeshes)
  {
    m.GetShader()->enable();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.GetTexture());
    glUniform1i(m.GetShader()->getUniform("tex"), 0);

    _2DMeshBuffer->Draw(m.IndexCount(), m.GetOffset());
  }

  /*************
  * Second pass:
  *   render all 3D objects on top of the previous 2D shapes
  *************/
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  for (auto& m : _3DMeshes)
  {
    m.GetShader()->enable();
    glm::mat4 mvp = GetProjectionMatrix() * GetViewMatrix() * m.GetTransform();
    glUniformMatrix4fv(m.GetShader()->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);

    _3DMeshBuffer->Draw(m.IndexCount(), m.GetOffset());
  }

  // cleanup
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

void Renderer::shutdown()
{
  delete(_2DMeshBuffer);
  delete(_3DMeshBuffer);
  glfwDestroyWindow(_window);
}

} // namespace ar
