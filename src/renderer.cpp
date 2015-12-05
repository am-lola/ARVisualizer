#include "renderer.hpp"
#include "mesh/vertex.hpp"
#include "mesh/mesh.hpp"
#include "mesh/meshfactory.hpp"
#include "windowmanager/windowmanager.hpp"

namespace ar
{

Renderer::Renderer(GLFWwindow* window)
{
  _window = window;
  glfwGetWindowSize(window, &_width, &_height);
}

Renderer::~Renderer()
{
  if (_running)
  {
    Stop();
  }
}

void Renderer::Start()
{
  if (_running)
  {
    return;
  }

  std::condition_variable init_complete;
  _renderThread = std::thread([this, &init_complete]()
  {
    this->init();
    init_complete.notify_all();
    this->render();
  });

  // wait for initialization to finish before returning
  std::unique_lock<std::mutex> lock(_mutex);
  init_complete.wait(lock, [this]{return this->_running;});
}

void Renderer::Stop()
{
  if (!_running)
  {
    return;
  }
  _running = false;
  _renderThread.join();
  WindowManager::Instance().DeleteRenderer(this, _window);
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

unsigned int Renderer::Add3DMesh(Mesh3D mesh)
{
  unsigned int handle = GenerateMeshHandle(mesh);
  mesh.SetShader(&_defaultShader);
  mesh.SetID(handle);
  _3DMeshes.push_back(mesh);
  return handle;
}

void Renderer::RemoveMesh(unsigned int handle)
{
  if (handle == 0) { return; } // zero is reserved as a non-unique default ID

  for (auto& m : _3DMeshes)
  {
    if (m.ID() == handle)
    {
      m.MarkForDeletion();
      break;
    }
  }
}

/// TODO: make this more robust
/// Each mesh should get a unique ID we can use later to find it, even if
/// the renderer has resorted the vector containing it.
unsigned int Renderer::GenerateMeshHandle(Mesh3D mesh)
{
  static unsigned int nextID = 0;
  return ++nextID;
}

void Renderer::init()
{
  std::lock_guard<std::mutex> guard(_mutex);

  // setup OpenGL context and open a window for rendering
  init_GL();

  // init buffers
  _2DMeshBuffer = new VertexBuffer<Vertex2D>();
  _3DMeshBuffer = new VertexBuffer<Vertex3D>();

  // load video shaders
  _defaultShader.loadAndLink("shaders/default.vert", "shaders/default.frag");
  _defaultShader.addUniform("MVP");
  _videoShader.loadAndLink("shaders/2D_passthru.vert", "shaders/simpleTexture.frag");
  _videoShader.addUniform("tex");

  // setup default goemetry needed for rendering and send it to OpenGL
  init_geometry();

  // generate texture handles and allocate space for default textures
  init_textures();

  // notify initialization is complete
  _running = true;
}

void Renderer::init_GL()
{
  // bind GL context to this thread
  glfwMakeContextCurrent(_window);

  glfwSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0, 0, 0.15, 1);
}

void Renderer::init_geometry()
{
  // prepare video pane
  std::vector<Vertex2D> videoVertices = {
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
    // check for new meshes, video data, etc.
    update();

    // Render!
    renderOneFrame();

    glfwSwapBuffers(_window);
    glfwPollEvents();
  }

  // if we've stopped rendering, cleanup
  shutdown();
}

void Renderer::update()
{
  // if we've received a new video frame, send it to the GPU
  if (_newVideoFrame)
  {
    std::lock_guard<std::mutex> guard(_mutex);
    bufferTexture(_width, _height, _2DMeshes[0].GetTexture(), _currentVideoFrame);
    _newVideoFrame = false;
  }

  // if one or more meshes was marked for deletion, delete it and regenerate VBO/VEOs
  // The implementation here causes us to trash and rebuild any VertexBuffer which contained
  // a deleted mesh. It may be faster to just delete the range of vertices covered by that
  // mesh, but we'd still have to go through all the other meshes to update their offsets
  // and re-send the modified buffer to the GPU, so the gains may not be terribly meaningful.
  bool mustRegenerateVBO_2D = false;
  for (int i = 0; i < _2DMeshes.size(); i++)
  {
    if (_2DMeshes[i].PendingDelete())
    {
      _2DMeshes.erase(_2DMeshes.begin() + i); // remove mesh
      _2DMeshBuffer->ClearAll();   // empty vertex buffer so it can be rebuilt
      mustRegenerateVBO_2D = true;
    }
  }
  bool mustRegenerateVBO_3D = false;
  for (int i = 0; i < _3DMeshes.size(); i++)
  {
    if (_3DMeshes[i].PendingDelete())
    {
      _3DMeshes.erase(_3DMeshes.begin() + i);
      _3DMeshBuffer->ClearAll();
      mustRegenerateVBO_3D = true;
    }
  }

  // if we have any new mesh data, update buffers and send it to the GPU
  for (auto& m : _2DMeshes)
  {
    if (m.Dirty() || mustRegenerateVBO_2D)
    {
      m.SetVertexOffset(_2DMeshBuffer->AddVertices(m.GetVertices()));
      m.SetIndexOffset(_2DMeshBuffer->AddIndices(m.GetIndices()));
      m.ClearDirty();
    }
  }
  if (_2DMeshBuffer->Dirty())
  {
    _2DMeshBuffer->BufferData();
  }
  for (auto& m : _3DMeshes)
  {
    if (m.Dirty() || mustRegenerateVBO_3D)
    {
      m.SetVertexOffset(_3DMeshBuffer->AddVertices(m.GetVertices()));
      m.SetIndexOffset(_3DMeshBuffer->AddIndices(m.GetIndices()));
      m.ClearDirty();
    }
  }
  if (_3DMeshBuffer->Dirty())
  {
    _3DMeshBuffer->BufferData();
  }
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

    _2DMeshBuffer->Draw(m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
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

    _3DMeshBuffer->Draw(m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
  }

  // cleanup
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

void Renderer::shutdown()
{
  // Clean up any textures still in use
  for (auto m : _2DMeshes)
  {
    GLuint tex = m.GetTexture();
    glDeleteTextures(1, &tex);
  }

  delete(_2DMeshBuffer);
  delete(_3DMeshBuffer);
  delete(_currentVideoFrame);
  glfwMakeContextCurrent(NULL); // unbind OpenGL context from this thread
}

} // namespace ar
