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
  _quadsBuffer = new VertexBuffer<VertexP2T2>();

  // load video shaders
  _videoShader.loadAndLink("shaders/video.vert", "shaders/video.frag");
  _videoShader.addUniform("videotex");

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
  glClearColor(0, 0, 0.5, 1);
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

  _2DQuads.push_back(TexturedQuad(videoVertices, videoIndices, &_videoShader));
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

    _2DQuads[0].SetTexture(videoTexture); /// TODO: Don't do this...
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
      bufferTexture(_width, _height, _2DQuads[0].GetTexture(), _currentVideoFrame);
      _newVideoFrame = false;
    }

    // if we have any new mesh data, send it to the GPU
    for (auto& m : _2DQuads)
    {
      if (m.Dirty())
      {
        m.SetOffset(_quadsBuffer->AddVertices(m.GetVertices()));
        _quadsBuffer->AddIndices(m.GetIndices());
        m.ClearDirty();
      }
    }
    if (_quadsBuffer->Dirty())
    {
      _quadsBuffer->BufferData();
    }

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

  _videoShader.enable();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _2DQuads[0].GetTexture());
  glUniform1i(_videoShader.getUniform("videotex"), 0);

  for (auto& m : _2DQuads)
  {
    _quadsBuffer->Draw(m.IndexCount(), m.GetOffset());
  }

  // cleanup
  glBindTexture(GL_TEXTURE_2D, 0);
  _videoShader.disable();
}

void Renderer::shutdown()
{
  delete(_quadsBuffer);
  glfwDestroyWindow(_window);
}

} // namespace ar
