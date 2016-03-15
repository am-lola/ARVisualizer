#include "Renderer.hpp"
#include "rendering/SceneInfo.hpp"

namespace ar
{

template <typename T>
class Renderer::RenderCommandAddMesh : public RenderCommand
{
public:

  RenderCommandAddMesh(Renderer* renderer, unsigned int handle, const Mesh<T>& _mesh, SharedPtr<Material> _material)
    : _renderer(renderer), _handle(handle), _mesh(_mesh), _material(_material)
  { }

  virtual void execute() override
  {
    _mesh.SetMaterial(_material);
    _mesh.SetID(_handle);
    _renderer->_meshRenderer.AddMesh(_mesh);
  }

  Renderer* _renderer;
  unsigned int _handle;
  Mesh<T> _mesh;
  SharedPtr<Material> _material;
};

class Renderer::RenderCommandUpdateTransform : public RenderCommand
{
public:
  RenderCommandUpdateTransform(Renderer* renderer, unsigned int handle, const glm::mat4& transform, bool absolute)
    : _renderer(renderer), _handle(handle), _transform(transform), _absolute(absolute)
  { }

  virtual void execute() override
  {
    _renderer->_meshRenderer.SetMeshTransform(_handle, _transform, _absolute);
  }

  Renderer* _renderer;
  unsigned int _handle;
  glm::mat4 _transform;
  bool _absolute;
};

class Renderer::RenderCommandUpdateMesh : public RenderCommand
{
public:
  RenderCommandUpdateMesh(Renderer* renderer, unsigned int handle, const Mesh3D& mesh, SharedPtr<Material> material)
    : _renderer(renderer), _handle(handle), _mesh(mesh), _material(material)
  { }

  virtual void execute() override
  {
    _mesh.SetMaterial(_material);
    _renderer->_meshRenderer.UpdateMesh(_handle, _mesh);
  }

  Renderer* _renderer;
  unsigned int _handle;
  Mesh3D _mesh;
  SharedPtr<Material> _material;
};

class Renderer::RenderCommandRemoveMesh : public RenderCommand
{
public:
  RenderCommandRemoveMesh(Renderer* renderer, unsigned int handle)
    : _renderer(renderer), _handle(handle)
  { }

  virtual void execute() override
  {
    _renderer->_meshRenderer.RemoveMesh(_handle);
  }

  Renderer* _renderer;
  unsigned int _handle;
};

class Renderer::RenderCommandNotifyNewVideoFrame : public RenderCommand
{
public:

  RenderCommandNotifyNewVideoFrame(Renderer* renderer, unsigned int width, unsigned int height, unsigned char* pixels)
    : _renderer(renderer), _width(width), _height(height)
  {
    _pixels = UniquePtr<unsigned char[]>(new unsigned char[width * height * 3]);

    // TODO: Memcpy
    for (size_t i = 0; i < width * height * 3; i++)
    {
      _pixels[i] = pixels[i];
    }
  }

  virtual void execute() override
  {
    _renderer->_videoRenderer.SetNewFrame(_width, _height, _pixels.get());
  }

  Renderer* _renderer;
  unsigned int _width;
  unsigned int _height;

  UniquePtr<unsigned char[]> _pixels;
};

class Renderer::RenderCommandAddPointCloud : public RenderCommand
{
public:
  RenderCommandAddPointCloud(Renderer* renderer, unsigned int handle, const void* pointData, size_t numPoints, Color color)
    : _renderer(renderer), _handle(handle), _color(color)
  {
    _pointCloud.reset(new PointCloud<VertexP4>);

    const PointCloud<VertexP4>::VertexType* verts = reinterpret_cast<const PointCloud<VertexP4>::VertexType*>(pointData);

    _pointCloud->SetPoints(verts, numPoints);
    _pointCloud->SetID(handle);
  }

  virtual void execute() override
  {
    _renderer->_pointCloudRenderer.AddPointCloud(std::move(_pointCloud), _color);
  }

  Renderer* _renderer;
  unsigned int _handle;
  UniquePtr<PointCloud<VertexP4>> _pointCloud;
  Color _color;
};

// TODO: If we get multiple of these in the command queue we only want to take the last one
class Renderer::RenderCommandUpdatePointCloud : public RenderCommand
{
public:
  RenderCommandUpdatePointCloud(Renderer* renderer, unsigned int handle, const void* pointData, size_t numPoints, Color color)
    : _renderer(renderer), _handle(handle), _color(color)
  {

    const PointCloud<VertexP4>::VertexType* verts = reinterpret_cast<const PointCloud<VertexP4>::VertexType*>(pointData);
    _points.assign(verts, verts + numPoints);
  }

  virtual void execute() override
  {
    _renderer->_pointCloudRenderer.UpdatePointCloud(_handle, _points, _color);
  }

  Renderer* _renderer;
  unsigned int _handle;
  Vector<VertexP4> _points;
  Color _color;
};

class Renderer::RenderCommandDrawVoxels : public RenderCommand
{
public:
  RenderCommandDrawVoxels(Renderer* renderer, const Voxel* voxels, size_t numVoxels)
    : _renderer(renderer)
  {
    _voxels.assign(voxels, voxels + numVoxels);
  }

  virtual void execute() override
  {
    _renderer->_voxelRenderer.SetVoxels(_voxels);
  }

  Renderer* _renderer;
  Vector<Voxel> _voxels;
};

// used to synchronize between all active rendering threads to work around IMGUI not playing nice with threads
std::mutex Renderer::_renderGUILock;

Renderer::Renderer(GLFWwindow* window)
  : _windowEvents(window), _imguiRenderer(window), _camera(_windowEvents)
{
  _window = window;
  glfwGetWindowSize(window, &_windowWidth, &_windowHeight);

  _windowEvents.GetFrameBufferResizedDelegate() += [this](int w, int h)
  {
    this->OnWindowResized(w, h);
  };

  _windowEvents.GetWindowResizedDelegate() += [this](int w, int h)
  {
    this->OnFramebufferResized(w, h);
  };

  // Listen for keyboard input
  _windowEvents.GetKeyboardKeyDelegate() += [this](int k, int scan, int action, int mods)
  {
    if (!ImGui::IsAnyItemActive())
    {
      if (action == GLFW_PRESS && k == GLFW_KEY_1)
      {
        this->_renderType = GL_TRIANGLES;
      }
      else if (action == GLFW_PRESS && k == GLFW_KEY_2)
      {
        // TODO: This is not working correctly. Could use glPolygonMode( GL_FRONT_AND_BACK, GL_LINE )
        this->_renderType = GL_LINE_LOOP;
      }
      else if (action == GLFW_PRESS && k == GLFW_KEY_3)
      {
        this->_renderType = GL_POINTS;
      }
    }

    this->_imguiRenderer.OnKeyPress(k, scan, action, mods);
  };

  _windowEvents.GetKeyboardCharDelegate() += [this](unsigned int codepoint)
  {
    this->_imguiRenderer.OnKeyChar(codepoint);
  };

  _windowEvents.GetMouseButtonDelegate() += [this](int button, int action, int mods)
  {
    this->_imguiRenderer.OnMouseButton(button, action, mods);
  };

  _windowEvents.GetScrollDelegate() += [this](double xoffset, double yoffset)
  {
    this->_imguiRenderer.OnScroll(xoffset, yoffset);
  };

  // set a default projection matrix
  _camera.SetAspectRatio((float)_windowWidth / (float)_windowHeight);
  _camera.UpdateProjection();
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

  _renderThread = std::thread([this]()
  {
    this->Init();
    this->Render();
  });

  // we don't need to wait for render initialization - the command queue will buffer everything
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

void Renderer::NotifyNewVideoFrame(unsigned int width, unsigned int height, unsigned char* pixels)
{
  RenderCommandNotifyNewVideoFrame* command = new RenderCommandNotifyNewVideoFrame(this, width, height, pixels);
  EnqueueRenderCommand(command);
}

void Renderer::SetCameraPose(glm::vec3 position, glm::vec3 forward, glm::vec3 up)
{
  _camera.SetPosition(position);
  _camera.SetForwardAndUp(glm::normalize(forward), glm::normalize(up));
}

unsigned int Renderer::Add3DMesh(const Mesh3D& mesh, SharedPtr<Material> material)
{
  const unsigned int handle = GenerateMeshHandle();

  RenderCommandAddMesh<Vertex3D>* command = new RenderCommandAddMesh<Vertex3D>(this, handle, mesh, material);
  EnqueueRenderCommand(command);

  return handle;
}

unsigned int Renderer::AddPointCloud(const void* pointData, size_t numPoints, Color color)
{
  const unsigned int handle = GenerateMeshHandle();
  RenderCommandAddPointCloud* command = new RenderCommandAddPointCloud(this, handle, pointData, numPoints, color);
  EnqueueRenderCommand(command);
  return handle;
}

void Renderer::UpdatePointCloud(unsigned int handle, const void* pointData, size_t numPoints, Color color)
{
  RenderCommandUpdatePointCloud* command = new RenderCommandUpdatePointCloud(this, handle, pointData, numPoints, color);
  EnqueueRenderCommand(command);
}

void Renderer::Update(unsigned int handle, const Mesh3D& mesh, SharedPtr<Material> material)
{
  if (handle == 0) { return; }
  RenderCommandUpdateMesh* command = new RenderCommandUpdateMesh(this, handle, mesh, material);
  EnqueueRenderCommand(command);
}

void Renderer::UpdateTransform(unsigned int handle, const glm::mat4& transform, bool absolute)
{
  if (handle == 0) { return; }
  RenderCommandUpdateTransform* command = new RenderCommandUpdateTransform(this, handle, transform, absolute);
  EnqueueRenderCommand(command);
}

void Renderer::RemoveMesh(unsigned int handle)
{
  if (handle == 0) { return; } // zero is reserved as a non-unique default ID
  RenderCommandRemoveMesh* command = new RenderCommandRemoveMesh(this, handle);
  EnqueueRenderCommand(command);
}

void Renderer::RemoveAllMeshes()
{
}

/// TODO: make this more robust
/// Each mesh should get a unique ID we can use later to find it, even if
/// the renderer has resorted the vector containing it.
unsigned int Renderer::GenerateMeshHandle()
{
  static unsigned int nextID = 0;
  return ++nextID;
}

void Renderer::Init()
{
  // setup OpenGL context and open a window for rendering
  Init_GL();

  // load shaders
  _defaultShader.loadAndLink(ShaderSources::sh_simpleNormal_vert, ShaderSources::sh_simpleLit_frag);

  _meshRenderer.Init();
  _meshRenderer.SetDefaultShader(&_defaultShader);
  _videoRenderer.Init();
  _pointCloudRenderer.Init();
  _voxelRenderer.Init();

  _imguiRenderer.Init();

  // notify initialization is complete
  _running = true;
}

void Renderer::Init_GL()
{
  // bind GL context to this thread
  glfwMakeContextCurrent(_window);

  glfwSwapInterval(1);
  glEnable(GL_CULL_FACE);
  glBlendEquation(GL_FUNC_ADD);
  glClearColor(0, 0, 0, 0);
  glPointSize(5.0f);
}

void Renderer::OnWindowResized(int newWidth, int newHeight)
{
  _windowWidth = newWidth;
  _windowHeight = newHeight;
}

void Renderer::OnFramebufferResized(int newWidth, int newHeight)
{
  glViewport(0, 0, newWidth, newHeight);
  _camera.SetAspectRatio((float)newWidth / (float)newHeight);
}

void Renderer::EnableRenderPass(RenderPassParams pass)
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

void Renderer::Render()
{
  while (_running)
  {
    // check for new meshes, video data, etc.
    Update();

    // Render!
    RenderOneFrame();

    glfwSwapBuffers(_window);
    glfwPollEvents();
  }

  // if we've stopped rendering, cleanup
  Shutdown();
}

void Renderer::Update()
{
  // get time since last frame
  double currentTime = glfwGetTime();
  double deltaTime = currentTime - _lastFrameTime;
  _lastFrameTime = currentTime;

  _camera.Update(deltaTime);

  // Execute all available render commands
  const size_t numCommands = _renderCommandQueue.NumEnqueuedCommands();
  for (size_t i = 0; i < numCommands; i++)
  {
    auto command = _renderCommandQueue.Dequeue();
    command->execute();
    delete command;
  }

  _videoRenderer.Update();
  _pointCloudRenderer.Update();
  _meshRenderer.Update();
  _voxelRenderer.Update();
}

void Renderer::RenderOneFrame()
{
  SceneInfo sceneInfo;
  sceneInfo.renderType = _renderType;
  sceneInfo.viewMatrix = GetViewMatrix();
  sceneInfo.projectionMatrix = GetProjectionMatrix();
  sceneInfo.lightDir = light_dir;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*************
  * First pass:
  *   render all 2D textured shapes
  *************/
  EnableRenderPass(Blend_None);
  _videoRenderer.RenderPass(sceneInfo);

  /*************
  * Second pass:
  *   render point cloud
  *************/
  EnableRenderPass(Blend_Add);
  _pointCloudRenderer.RenderPass(sceneInfo);

  EnableRenderPass(Blend_None | EnableDepth);
  _voxelRenderer.RenderPass(sceneInfo);

  /*************
  * Third pass:
  *   render all 3D objects on top of the previous 2D shapes
  *************/
  EnableRenderPass(Blend_Alpha);
  _meshRenderer.RenderPass(sceneInfo);

  // cleanup
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);

  // Render GUI last
  RenderGUI();
}

void Renderer::RenderGUI()
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

  _pointCloudRenderer.RenderGUI();
  _meshRenderer.RenderGUI();
  _videoRenderer.RenderGUI();
  _voxelRenderer.RenderGUI();

  _renderGUIDelegate();

  ImGui::Render();
  _imguiRenderer.RenderDrawLists(ImGui::GetDrawData());
}

void Renderer::Shutdown()
{
  _videoRenderer.Release();
  _voxelRenderer.Release();
  _meshRenderer.Release();
  _pointCloudRenderer.Release();

  _imguiRenderer.Shutdown();

  glfwMakeContextCurrent(NULL); // unbind OpenGL context from this thread
}

void Renderer::DrawVoxels(const Voxel* voxels, size_t numVoxels)
{
  RenderCommandDrawVoxels* command = new RenderCommandDrawVoxels(this, voxels, numVoxels);
  EnqueueRenderCommand(command);
}

} // namespace ar
