#include "Renderer.hpp"
#include "rendering/SceneInfo.hpp"
#include "mesh/MeshFactory.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace ar
{

template <typename T>
class Renderer::RenderCommandAddMesh : public RenderCommand
{
public:

  RenderCommandAddMesh(Renderer* renderer, unsigned int handle, const Mesh<T>& _mesh, SharedPtr<Material> _material)
    : _renderer(renderer), _handle(handle), _mesh(new Mesh<T>(_mesh)), _material(_material)
  { }

  virtual void execute() override
  {
    _mesh->SetMaterial(_material);
    _mesh->SetID(_handle);
    _renderer->_meshRenderer.AddMesh(_mesh);
  }

  Renderer* _renderer;
  unsigned int _handle;
  Mesh<T>* _mesh;
  SharedPtr<Material> _material;
};

class Renderer::RenderCommandAddLineMesh : public RenderCommand
{
public:

  RenderCommandAddLineMesh(Renderer* renderer, unsigned int handle, const LineMesh& _mesh, SharedPtr<Material> _material)
    : _renderer(renderer), _handle(handle), _mesh(new LineMesh(_mesh)), _material(_material)
  { }

  virtual void execute() override
  {
    _mesh->SetMaterial(_material);
    _mesh->SetID(_handle);
    _renderer->_lineRenderer.AddMesh(_mesh);
  }

  Renderer* _renderer;
  unsigned int _handle;
  LineMesh* _mesh;
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
    _renderer->_pointCloudRenderer.SetPointCloudTransform(_handle, _transform, _absolute);
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
    : _renderer(renderer), _handle(handle), _mesh(new Mesh3D(mesh)), _material(material)
  { }

  virtual void execute() override
  {
    _mesh->SetMaterial(_material);
    _mesh->SetID(_handle);
    _renderer->_meshRenderer.UpdateMesh(_handle, _mesh);
  }

  Renderer* _renderer;
  unsigned int _handle;
  Mesh3D* _mesh;
  SharedPtr<Material> _material;
};

class Renderer::RenderCommandUpdateLineMesh : public RenderCommand
{
public:
  RenderCommandUpdateLineMesh(Renderer* renderer, unsigned int handle, const LineMesh& mesh, SharedPtr<Material> material)
    : _renderer(renderer), _handle(handle), _mesh(new LineMesh(mesh)), _material(material)
  { }

  virtual void execute() override
  {
    _mesh->SetMaterial(_material);
    _mesh->SetID(_handle);
    _renderer->_lineRenderer.UpdateMesh(_handle, _mesh);
  }

  Renderer* _renderer;
  unsigned int _handle;
  LineMesh* _mesh;
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
    auto it = _renderer->_visibilityMap.find(_handle);
    if (it != _renderer->_visibilityMap.end())
      _renderer->_visibilityMap.erase(it);

    _renderer->_meshRenderer.RemoveMesh(_handle);
    _renderer->_lineRenderer.RemoveMesh(_handle);
    _renderer->_pointCloudRenderer.RemovePointCloud(_handle);
  }

  Renderer* _renderer;
  unsigned int _handle;
};

class Renderer::RenderCommandRemoveAll : public RenderCommand
{
public:
  RenderCommandRemoveAll(Renderer* renderer, bool removeMeshes, bool removeVoxels)
    : _renderer(renderer), _removeMeshes(removeMeshes), _removeVoxels(removeVoxels)
  { }

  virtual void execute() override
  {
    if (_removeMeshes)
    {
      _renderer->_visibilityMap.clear();
      _renderer->_meshRenderer.RemoveAllMeshes();
      _renderer->_lineRenderer.RemoveAllMeshes();
      //_renderer->_pointCloudRenderer.RemoveAllPointClouds();
    }
    if (_removeVoxels)
    {
      _renderer->_voxelRenderer.ClearVoxels();
    }
  }

  Renderer* _renderer;
  bool _removeMeshes;
  bool _removeVoxels;
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

  RenderCommandNotifyNewVideoFrame(Renderer* renderer, unsigned int width, unsigned int height, unsigned char* pixels, int largefactor)
    : _renderer(renderer), _width(width), _height(height), _largefactor(largefactor)
  {
    _pixels = UniquePtr<unsigned char[]>(new unsigned char[width * largefactor * height * 3]);

    // TODO: Memcpy
    for (size_t i = 0; i < width * height * 3 * (_largefactor - 1) / 2; i++)
    {
      _pixels[i] = 50;
    }
    size_t start = width * height * 3 * (_largefactor - 1) / 2;
    for (size_t i = start; i < start + width * height * 3; i++)
    {
      _pixels[i] = pixels[i-start];
    }
    start = start + width * height * 3;
    for (size_t i = start; i < start + width * height * 3 * (_largefactor - 1) / 2; i++)
    {
      _pixels[i] = 50;
    }
  }

  virtual void execute() override
  {
    _renderer->_videoRenderer.SetNewFrame(_width, _largefactor * _height, _pixels.get());
  }

  Renderer* _renderer;
  unsigned int _width;
  unsigned int _height;
  int _largefactor = 1;

  UniquePtr<unsigned char[]> _pixels;
};

class Renderer::RenderCommandAddPointCloud : public RenderCommand
{
public:
  RenderCommandAddPointCloud(Renderer* renderer, unsigned int handle, const void* pointData, size_t numPoints, bool colored, Color color)
    : _renderer(renderer), _handle(handle), _colored(colored), _color(color)
  {
    if (!colored)
    {
      const auto verts = reinterpret_cast<const VertexP4*>(pointData);
      auto pc = new PointCloud<VertexP4>;
      pc->SetPoints(verts, numPoints);
      pc->SetID(handle);
      _pointCloud.reset(pc);
    }
    else
    {
      const auto verts = reinterpret_cast<const Vertex_PCL_PointXYZRGBA*>(pointData);
      auto pc = new PointCloud<Vertex_PCL_PointXYZRGBA>;
      pc->SetPoints(verts, numPoints);
      pc->SetID(handle);
      _pointCloud.reset(pc);
    }
  }

  virtual void execute() override
  {
    _renderer->_pointCloudRenderer.AddPointCloud(std::move(_pointCloud), _colored, _color);
  }

  Renderer* _renderer;
  unsigned int _handle;
  UniquePtr<BasePointCloud> _pointCloud;
  bool _colored;
  Color _color;
};

// TODO: If we get multiple of these in the command queue we only want to take the last one
class Renderer::RenderCommandUpdatePointCloud : public RenderCommand
{
public:
  RenderCommandUpdatePointCloud(Renderer* renderer, unsigned int handle, const void* pointData, size_t numPoints, bool colored, Color color)
    : _renderer(renderer), _handle(handle), _colored(colored), _color(color)
  {
    if (!colored)
    {
      const auto* verts = reinterpret_cast<const VertexP4*>(pointData);
      _points.assign(verts, verts + numPoints);
    }
    else
    {
      const auto* verts = reinterpret_cast<const Vertex_PCL_PointXYZRGBA*>(pointData);
      _pointsColored.assign(verts, verts + numPoints);
    }
  }

  virtual void execute() override
  {
    if (!_colored)
      _renderer->_pointCloudRenderer.UpdatePointCloud(_handle, _points, _color);
    else
      _renderer->_pointCloudRenderer.UpdatePointCloud(_handle, _pointsColored);
  }

  Renderer* _renderer;
  unsigned int _handle;
  Vector<VertexP4> _points;
  Vector<Vertex_PCL_PointXYZRGBA> _pointsColored;
  bool _colored;
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

class Renderer::RenderCommandSetVisibility : public RenderCommand
{
public:
  RenderCommandSetVisibility(Renderer* renderer, unsigned int handle, bool visible)
    : _renderer(renderer), _handle(handle), _visible(visible)
  {
  }

  virtual void execute() override
  {
    _renderer->_visibilityMap[_handle] = _visible;
  }

  Renderer* _renderer;
  unsigned int _handle;
  bool _visible;
};

// used to synchronize between all active rendering threads to work around IMGUI not playing nice with threads
std::mutex Renderer::_renderGUILock;

Renderer::Renderer(GLFWwindow* window)
  : _running(false), _windowEvents(window), _imguiRenderer(window), _camera(_windowEvents)
{
  _window = window;
  glfwGetWindowSize(window, &_windowWidth, &_windowHeight);

  _meshRenderPassParams = Blend_Alpha | EnableDepth;
  _lightAlpha = false;

  _windowEvents.GetFrameBufferResizedDelegate() += [this](int w, int h)
  {
    this->OnFramebufferResized(w, h);
  };

  _windowEvents.GetWindowResizedDelegate() += [this](int w, int h)
  {
    this->OnWindowResized(w, h);
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

  strcpy(_screenshotPrefix, "img");
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

void Renderer::NotifyNewVideoFrame(unsigned int width, unsigned int height, unsigned char* pixels, int largefactor)
{
  RenderCommandNotifyNewVideoFrame* command = new RenderCommandNotifyNewVideoFrame(this, width, height, pixels, largefactor);
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

unsigned int Renderer::AddPointCloud(const void* pointData, size_t numPoints, bool colored, Color color)
{
  const unsigned int handle = GenerateMeshHandle();
  RenderCommandAddPointCloud* command = new RenderCommandAddPointCloud(this, handle, pointData, numPoints, colored, color);
  EnqueueRenderCommand(command);
  return handle;
}

unsigned int Renderer::AddLineMesh(const LineMesh& mesh, SharedPtr<Material> material)
{
  const unsigned int handle = GenerateMeshHandle();

  RenderCommandAddLineMesh* command = new RenderCommandAddLineMesh(this, handle, mesh, material);
  EnqueueRenderCommand(command);

  return handle;
}

void Renderer::UpdatePointCloud(unsigned int handle, const void* pointData, size_t numPoints, bool colored, Color color)
{
  RenderCommandUpdatePointCloud* command = new RenderCommandUpdatePointCloud(this, handle, pointData, numPoints, colored, color);
  EnqueueRenderCommand(command);
}

void Renderer::UpdateMesh(unsigned int handle, const Mesh3D& mesh, SharedPtr<Material> material)
{
  if (handle == 0) { return; }
  RenderCommandUpdateMesh* command = new RenderCommandUpdateMesh(this, handle, mesh, material);
  EnqueueRenderCommand(command);
}

void Renderer::UpdateLineMesh(unsigned int handle, const LineMesh& mesh, SharedPtr<Material> material)
{
  if (handle == 0) { return; }
  RenderCommandUpdateLineMesh* command = new RenderCommandUpdateLineMesh(this, handle, mesh, material);
  EnqueueRenderCommand(command);
}

void Renderer::UpdateTransform(unsigned int handle, const glm::mat4& transform, bool absolute)
{
  if (handle == 0) { return; }
  RenderCommandUpdateTransform* command = new RenderCommandUpdateTransform(this, handle, transform, absolute);
  EnqueueRenderCommand(command);
}

void Renderer::SetVisibility(unsigned int handle, bool visible)
{
  if (handle == 0) { return; }
  RenderCommandSetVisibility* command = new RenderCommandSetVisibility(this, handle, visible);
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
  RenderCommandRemoveAll* command = new RenderCommandRemoveAll(this, true, false);
  EnqueueRenderCommand(command);
}

void Renderer::RemoveAllVoxels()
{
  RenderCommandRemoveAll* command = new RenderCommandRemoveAll(this, false, true);
  EnqueueRenderCommand(command);
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

  InitGUI();

  // load shaders
  _defaultShader.loadAndLink(ShaderSources::sh_simpleNormal_vert, ShaderSources::sh_simpleLit_frag);

  _meshRenderer.Init();
  _meshRenderer.SetDefaultShader(&_defaultShader);
  _videoRenderer.Init();
  _pointCloudRenderer.Init();
  _voxelRenderer.Init();
  _lineRenderer.Init();

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

void Renderer::InitGUI()
{
  // Set theme (adapted from https://github.com/simongeilfus/Cinder-ImGui)
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding            = ImVec2(10, 10);
  style.WindowMinSize            = ImVec2(160, 20);
  style.FramePadding             = ImVec2(4, 3);
  style.ItemSpacing              = ImVec2(8, 4);
  style.ItemInnerSpacing         = ImVec2(6, 4);
  style.Alpha			         = 0.95f;
  style.WindowFillAlphaDefault   = 1.0f;
  style.WindowRounding           = 4.0f;
  style.FrameRounding            = 2.0f;
  style.IndentSpacing            = 6.0f;
  style.ItemInnerSpacing		 = ImVec2(2, 4);
  style.ColumnsMinSpacing        = 50.0f;
  style.GrabMinSize		         = 14.0f;
  style.GrabRounding		     = 16.0f;
  style.ScrollbarSize		     = 12.0f;
  style.ScrollbarRounding	     = 16.0f;
  style.Colors[ImGuiCol_Text]                  = ImVec4(0.86f, 0.93f, 0.89f, 0.88f);
  style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
  style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.06f, 0.09f, 0.12f, 0.90f);
  style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.20f, 0.22f, 0.27f, 0.58f);
  style.Colors[ImGuiCol_Border]                = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
  style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
  style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.24f, 0.26f, 0.31f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
  style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.47f, 0.77f, 0.83f, 0.21f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
  style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_Button]                = ImVec4(0.22f, 0.24f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
  style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_Header]                = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
  style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
  style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_Column]                = ImVec4(0.47f, 0.77f, 0.83f, 0.32f);
  style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
  style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.86f, 0.93f, 0.89f, 0.16f);
  style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.86f, 0.93f, 0.89f, 0.39f);
  style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
  style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
  style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.25f, 0.53f, 0.56f, 1.00f);
  style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
}

void Renderer::OnWindowResized(int newWidth, int newHeight)
{
  _windowWidth = newWidth;
  _windowHeight = newHeight;
}

void Renderer::OnFramebufferResized(int newWidth, int newHeight)
{
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

    HandleScreenshot();
  }

  // if we've stopped rendering, cleanup
  Shutdown();
}

std::string Renderer::MakeScreenshotFileName() const
{
  char fileName[128];
  MakeScreenshotFileName(fileName, sizeof(fileName));
  return std::string(fileName);
}

void Renderer::MakeScreenshotFileName(char* fileName, size_t len) const
{
  snprintf(fileName, len, "%s_%d.%s", _screenshotPrefix, _screenshotNr,
           _screenshotFileExt[static_cast<int>(_screenshotFormat)]);
}

void Renderer::HandleScreenshot()
{
  if (_doScreenshot && (!_screenshotHideGUI || !_guiIsVisible))
  {
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    const int width = vp[2];
    const int height = vp[3];

    const int size = width*height;
    uint32_t* buffer = new uint32_t[size];
    uint32_t* mirrored = new uint32_t[size];

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    const uint32_t alpha = _screenshotTransparentBG ? 0 : 0xFF000000;

    for (int y = 0; y < height; y++)
      for (int x = 0; x < width; x++)
        mirrored[y * width + x] = buffer[(height - y - 1) * width + x] | alpha;

    const std::string fileName = MakeScreenshotFileName();

    switch (_screenshotFormat)
    {
      case PNG:
        stbi_write_png(fileName.c_str(), width, height, 4, mirrored, width * 4);
        break;
      case BMP:
        stbi_write_bmp(fileName.c_str(), width, height, 4, mirrored);
        break;
      case TGA:
        stbi_write_tga(fileName.c_str(), width, height, 4, mirrored);
        break;
    }

    _doScreenshot = false;
    _hideGUI = false;
    _screenshotNr++;
    delete[] mirrored;
    delete[] buffer;
  }
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
  _lineRenderer.Update();
}

void Renderer::RenderOneFrame()
{
  SceneInfo sceneInfo;
  sceneInfo.renderType = _renderType;
  sceneInfo.viewMatrix = GetViewMatrix();
  sceneInfo.projectionMatrix = GetProjectionMatrix();
  sceneInfo.lightDir = light_dir;
  sceneInfo.nearClip = _camera._nearClip;
  sceneInfo.farClip = _camera._farClip;
  sceneInfo.aspect = _camera._aspect;
  sceneInfo.visibilityMap = &_visibilityMap;
  sceneInfo.lightAlpha = _lightAlpha;

  glViewport(0, 0, _windowWidth, _windowHeight);
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
  EnableRenderPass(Blend_None | EnableDepth);
  _pointCloudRenderer.RenderPass(sceneInfo);

  // Render voxels
  EnableRenderPass(Blend_None | EnableDepth);
  _voxelRenderer.RenderPass(sceneInfo);

  /*************
  * Third pass:
  *   render all 3D objects on top of the previous 2D shapes
  *************/
  EnableRenderPass(_meshRenderPassParams);
  _meshRenderer.RenderPass(sceneInfo);

  EnableRenderPass(Blend_None | EnableDepth);
  _lineRenderer.RenderPass(sceneInfo);

  // cleanup
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);

  if (!_hideGUI)
  {
    // Render GUI last
    RenderGUI();
    _guiIsVisible = true;
  }
  else
  {
    _guiIsVisible = false;
  }
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
  _lineRenderer.RenderGUI();

  static bool showScreenshotWindow = false;
  static bool showRendererStats = false;

  if (ImGui::Begin("Camera"))
  {
    ImGui::Separator();
    static int currentItem;
    ImGui::Combo("Blend mode", &currentItem, "Add\0Mul\0Alpha\0None\0");

    switch(currentItem)
    {
      case 0:
        _meshRenderPassParams = Blend_Add;
        break;
      case 1:
        _meshRenderPassParams = Blend_Mul;
        break;
      case 2:
        _meshRenderPassParams = Blend_Alpha;
        break;
      case 3:
        _meshRenderPassParams = Blend_None;
        break;
    }

    static bool enableDepth;
    ImGui::Checkbox("Enable depth", &enableDepth);
    ImGui::Checkbox("Light alpha", &_lightAlpha);

    ImGui::Separator();
    ImGui::Text("Windows:");

    if (enableDepth)
      _meshRenderPassParams |= EnableDepth;

    ImGui::Checkbox("Screenshot", &showScreenshotWindow);
    ImGui::Checkbox("Renderer Stats", &showRendererStats);
  }

  ImGui::End();

  if (showScreenshotWindow)
  {
    if (ImGui::Begin("Screenshot", &showScreenshotWindow, ImVec2(280, 150)))
    {
      ImGui::Columns(2);
      ImGui::PushItemWidth(-40);
      ImGui::InputText("Prefix", _screenshotPrefix, sizeof(_screenshotPrefix));
      int format = static_cast<int>(_screenshotFormat);
      ImGui::Combo("", &format, "png\0bmp\0tga\0");
      _screenshotFormat = static_cast<ScreenshotFormat>(format);
      if (ImGui::Button("Screenshot"))
      {
        _doScreenshot = true;
        if (_screenshotHideGUI)
          _hideGUI = true;
      }
      ImGui::Checkbox("Hide GUI", &_screenshotHideGUI);
      ImGui::Checkbox("Transparent BG", &_screenshotTransparentBG);
      ImGui::NextColumn();
      ImGui::PushItemWidth(-40);
      ImGui::DragInt("Nr", &_screenshotNr, 0.5f, 0, std::numeric_limits<int>::max());
      ImGui::SameLine();
      ImGui::TextDisabled("(?)");
      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Auto increments after a screenshot is taken.");

      char fileName[128];
      MakeScreenshotFileName(fileName, sizeof(fileName));
      ImGui::Text("%s", "Filename:");
      ImGui::Text("%s", fileName);
    }

    ImGui::End();
  }

  RenderStatsGUI(showRendererStats);
  _renderGUIDelegate();

  ImGui::Render();
  _imguiRenderer.RenderDrawLists(ImGui::GetDrawData());
}

void Renderer::RenderStatsGUI(bool& opened)
{
  static constexpr int bufferSize = 128;
  static float values[bufferSize] = { 0.0f };
  static int offset = 0;
  static float lastTime;

  const float timeDiff = ImGui::GetTime() - lastTime;
  lastTime = ImGui::GetTime();
  values[offset] = timeDiff;
  offset = (offset + 1) % bufferSize;

  if (opened)
  {
    if (ImGui::Begin("Renderer Stats", &opened))
    {
      ImGui::PushItemWidth(-100);
      ImGui::PlotLines("Frame time", values, bufferSize, offset, nullptr, 0.0f, 0.1f, ImVec2(0, 60));
    }
    ImGui::End();
  }
}

void Renderer::Shutdown()
{
  _videoRenderer.Release();
  _voxelRenderer.Release();
  _meshRenderer.Release();
  _pointCloudRenderer.Release();
  _lineRenderer.Release();

  _imguiRenderer.Shutdown();

  _defaultShader.destroy();

  glfwMakeContextCurrent(nullptr); // unbind OpenGL context from this thread
}

void Renderer::DrawVoxels(const Voxel* voxels, size_t numVoxels)
{
  RenderCommandDrawVoxels* command = new RenderCommandDrawVoxels(this, voxels, numVoxels);
  EnqueueRenderCommand(command);
}

bool Renderer::ProjectPointToNDC(const glm::vec3& point, glm::vec4& outProjected) const
{
  outProjected = GetProjectionMatrix() * GetViewMatrix() * glm::vec4(point, 1.0f);
  const bool behind = outProjected.w < _camera._nearClip;
  if (!behind)
    outProjected /= outProjected.w;
  return !behind;
}

bool Renderer::ProjectPointToPixel(const glm::vec3& point, glm::vec2& outPixel) const
{
  glm::vec4 pointNDC;
  if (!ProjectPointToNDC(point, pointNDC))
    return false;

  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  const int width = vp[2];
  const int height = vp[3];

  outPixel.x = ( pointNDC.x * 0.5f + 0.5f) * static_cast<float>(width);
  outPixel.y = (-pointNDC.y * 0.5f + 0.5f) * static_cast<float>(height);
  return true;
}

} // namespace ar
