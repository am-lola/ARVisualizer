#include "renderer.hpp"
#include "mesh/vertex.hpp"
#include "mesh/mesh.hpp"
#include "mesh/meshfactory.hpp"
#include "windowmanager/windowmanager.hpp"
#include "common.hpp"

#include <imgui.h>

namespace ar
{

Renderer::Renderer(GLFWwindow* window)
  : _windowEvents(window), _imguiRenderer(window), _camera(_windowEvents)
{
  _window = window;
  glfwGetWindowSize(window, &_windowWidth, &_windowHeight);

  // Ensure we're notified when the window size changes
  _windowEvents.SubscribeEvent(WindowEvents::WindowResized, (std::function<void(int,int)>)(
    [this] (int w, int h)
    {
      this->onWindowResized(w,h);
    }));

  // Ensure we're notified when the Framebuffer size changes
  _windowEvents.SubscribeEvent(WindowEvents::FrameBufferResized, (std::function<void(int,int)>)(
    [this] (int w, int h)
    {
      this->onFramebufferResized(w,h);
    }));

  // Listen for keyboard input
  _windowEvents.SubscribeEvent(WindowEvents::KeyboardKey,
    [this](int k, int scan, int action, int mods)
    {
      if (action == GLFW_PRESS && k == GLFW_KEY_1)
      {
        this->_renderType = GL_TRIANGLES;
      }
      else if (action == GLFW_PRESS && k == GLFW_KEY_2)
      {
        this->_renderType = GL_LINE_LOOP;
      }
      else if (action == GLFW_PRESS && k == GLFW_KEY_3)
      {
        this->_renderType = GL_POINTS;
      }

      this->_imguiRenderer.OnKeyPress(k, scan, action, mods);
    });

  _windowEvents.SubscribeEvent(WindowEvents::KeyboardChar, (std::function<void(unsigned int)>)(
    [this](unsigned int codepoint)
    {
      this->_imguiRenderer.OnKeyChar(codepoint);
    }));

  _windowEvents.SubscribeEvent(WindowEvents::MouseButton,
    [this](int button, int action, int mods)
    {
      this->_imguiRenderer.OnMouseButton(button, action, mods);
    });

  _windowEvents.SubscribeEvent(WindowEvents::Scroll, (std::function<void(double, double)>)(
    [this](double xoffset, double yoffset)
    {
      this->_imguiRenderer.OnScroll(xoffset, yoffset);
    }));

  // set a default projection matrix
  UpdateProjection();
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
  init_complete.wait(lock, [this]{return (bool)this->_running;});
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
  // TODO: Is this necessary?
  std::lock_guard<std::mutex> guard(_mutex);
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
  if (width != _videoWidth || height != _videoHeight)
  {
    _currentVideoFrame.reset();
    _currentVideoFrame = std::unique_ptr<unsigned char[]>(new unsigned char[width * height * 3]());
    _videoWidth = width;
    _videoHeight = height;
  }

  for (size_t i = 0; i < width * height * 3; i++)
  {
    _currentVideoFrame[i] = pixels[i];
  }

  _newVideoFrame = true;
}

void Renderer::SetCameraPose(glm::vec3 position, glm::vec3 forward, glm::vec3 up)
{
  _camera.SetPosition(position);
  _camera.SetForwardAndUp(glm::normalize(forward), glm::normalize(up));
}

unsigned int Renderer::Add3DMesh(Mesh3D mesh, std::shared_ptr<Material> material)
{
  std::lock_guard<std::mutex> guard(_mutex);
  unsigned int handle = GenerateMeshHandle(mesh);
  mesh.SetMaterial(material);
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

void Renderer::RemoveAllMeshes()
{
  for (auto& m : _3DMeshes)
  {
    m.MarkForDeletion();
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

void Renderer::UpdateProjection()
{
  if (_useCameraIntrinsics)
  {
    _projectionMatrix = glm::mat4(
      _cameraMatrix[0][0] / _cameraMatrix[0][2], 0, 0, 0,
      0, _cameraMatrix[1][1] / _cameraMatrix[1][2], 0, 0,
      0, 0, -(_camera_params.farClip + _camera_params.nearClip) / (_camera_params.farClip - _camera_params.nearClip), -1.0,
      0, 0, (-2.0 * _camera_params.farClip * _camera_params.nearClip) / (_camera_params.farClip - _camera_params.nearClip), 0
    );
  }
  else
  {
    _projectionMatrix = glm::perspective(
       _camera_params.fov,
       (float)_windowWidth / (float)_windowHeight,
       _camera_params.nearClip,
       _camera_params.farClip
     );
  }
}

void Renderer::init()
{
  std::lock_guard<std::mutex> guard(_mutex);

  // setup OpenGL context and open a window for rendering
  init_GL();

  // init buffers
  _2DMeshBuffer = std::unique_ptr<VertexBuffer<Vertex2D> >(new VertexBuffer<Vertex2D>());
  _3DMeshBuffer = std::unique_ptr<VertexBuffer<Vertex3D> >(new VertexBuffer<Vertex3D>());

  // load shaders
  _defaultShader.loadAndLink(ShaderSources::sh_simpleNormal_vert, ShaderSources::sh_simpleLit_frag);
  _videoShader.loadAndLink(ShaderSources::sh_2D_passthru_vert, ShaderSources::sh_simpleTexture_frag);

  // setup default goemetry needed for rendering and send it to OpenGL
  init_geometry();

  // generate texture handles and allocate space for default textures
  init_textures();

  _imguiRenderer.Init();

  // notify initialization is complete
  _running = true;
}

void Renderer::init_GL()
{
  // bind GL context to this thread
  glfwMakeContextCurrent(_window);

  glfwSwapInterval(1);
  glEnable(GL_CULL_FACE);
  glBlendEquation(GL_FUNC_ADD);
  glClearColor(0, 0, 0, 0);
  glPointSize(5.0);
}

void Renderer::init_geometry()
{
  // prepare video pane
  std::vector<Vertex2D> videoVertices = {
    //   Position        UV Coords
     { {-1.0f, -1.0f}, {0.0f, 1.0f} },
     { { 1.0f, -1.0f}, {1.0f, 1.0f} },
     { {-1.0f,  1.0f}, {0.0f, 0.0f} },
     { { 1.0f,  1.0f}, {1.0f, 0.0f} }
  };

  std::vector<GLuint> videoIndices = {
      0, 1, 3, // first triangle
      0, 3, 2  // second triangle
  };

  _2DMeshes.push_back(TexturedQuad(videoVertices, videoIndices, &_videoShader));
}

void Renderer::init_textures()
{

    // initialize video texture to be the same size as our window
    _videoWidth = _windowWidth; _videoHeight = _windowHeight;
    _currentVideoFrame = std::unique_ptr<unsigned char[]>(new unsigned char[_videoWidth * _videoHeight * 3]());
    for (size_t i = 0; i < _videoWidth * _videoHeight * 3; i++)
    {
      _currentVideoFrame[i] = 50;
    }

    GLuint videoTexture;
    glGenTextures(1, &videoTexture);
    bufferTexture(_videoWidth, _videoHeight, videoTexture, _currentVideoFrame.get());

    _2DMeshes[0].SetTexture(videoTexture); /// TODO: Don't do this...
}

void Renderer::onWindowResized(int newWidth, int newHeight)
{
  _windowWidth = newWidth;
  _windowHeight = newHeight;
}

void Renderer::onFramebufferResized(int newWidth, int newHeight)
{
  glViewport(0, 0, newWidth, newHeight);
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

void Renderer::enableRenderPass(RenderPassParams pass)
{
  // Enable Depth if set, otherwise ensure it's disabled
  if (pass & EnableDepth)
  {
    glEnable(GL_DEPTH_TEST);
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
  }

  if (pass & Blend_Add)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }

  if (pass & Blend_Mul)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
  }

  if (pass & Blend_Alpha)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  if (pass & Blend_None)
  {
    glDisable(GL_BLEND);
  }
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
    bufferTexture(_videoWidth, _videoHeight, _2DMeshes[0].GetTexture(), _currentVideoFrame.get());
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

  // TODO: Pass deltaTime
  _camera.Update(0.016);
}

void Renderer::renderOneFrame()
{  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*************
  * First pass:
  *   render all 2D textured shapes
  *************/
  enableRenderPass(Blend_None);
  for (auto& m : _2DMeshes)
  {
    m.GetShader()->enable();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.GetTexture());
    glUniform1i(m.GetShader()->getUniform("tex"), 0);

    _2DMeshBuffer->Draw(_renderType, m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
  }

  /*************
  * Second pass:
  *   render all 3D objects on top of the previous 2D shapes
  *************/
  enableRenderPass(Blend_Add);
  for (auto& m : _3DMeshes)
  {
    m.GetShader()->enable();

    // uniforms global to all objects
    glUniformMatrix4fv(m.GetShader()->getUniform("M"), 1, GL_FALSE, &(m.GetTransform()[0][0]));
    glUniformMatrix4fv(m.GetShader()->getUniform("V"), 1, GL_FALSE, &(GetViewMatrix()[0][0]));
    glUniform3fv(m.GetShader()->getUniform("light_dir"), 1, &(light_dir[0]));

    // object-specific uniforms
    glm::mat4 mvp = GetProjectionMatrix() * GetViewMatrix() * m.GetTransform();
    glUniformMatrix4fv(m.GetShader()->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
    m.GetMaterial()->Apply();

    _3DMeshBuffer->Draw(_renderType, m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
  }

  // cleanup
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);

  // Render GUI last
  renderGUI();
}

static std::mutex _renderGUILock;

void Renderer::renderGUI()
{
  // Still need to lock here to prevent a crash when switching focus between windows.
  MutexLockGuard guard(_renderGUILock);

  // Only draw in focused windows.
  // Imgui has global state, so multi-threaded rendering in different windows mutually stomps previous states
  // which prevents user interaction. Locking here doesn't help.
  if (!glfwGetWindowAttrib(_window, GLFW_FOCUSED))
    return;

  _imguiRenderer.NewFrame();
  _camera.RenderGUI();
  ImGui::ShowTestWindow();
  ImGui::Render();
  _imguiRenderer.RenderDrawLists(ImGui::GetDrawData());
}

void Renderer::shutdown()
{
  // Clean up any textures still in use
  for (auto m : _2DMeshes)
  {
    GLuint tex = m.GetTexture();
    glDeleteTextures(1, &tex);
  }

  _2DMeshBuffer.reset();;
  _3DMeshBuffer.reset();
  _currentVideoFrame.reset();

  _imguiRenderer.Shutdown();

  glfwMakeContextCurrent(NULL); // unbind OpenGL context from this thread
}

} // namespace ar
