#include "Renderer.hpp"
#include "mesh/MeshFactory.hpp"
#include "common.hpp"

namespace ar
{

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

  std::condition_variable init_complete;
  _renderThread = std::thread([this, &init_complete]()
  {
    this->Init();
    init_complete.notify_all();
    this->Render();
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
  return _running;
}

void Renderer::NotifyNewVideoFrame(int width, int height, unsigned char* pixels)
{
  if (!_running)
  {
    throw std::runtime_error("Renderer was not started before being sent data!");
  }

  MutexLockGuard guard(_mutex);

  // if the incoming image is a different size from the last frame we saw,
  // create a new array to match the new frame size
  if (width != _videoWidth || height != _videoHeight)
  {
    _currentVideoFrame.reset();
    _currentVideoFrame = UniquePtr<unsigned char[]>(new unsigned char[width * height * 3]());
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

unsigned int Renderer::Add3DMesh(Mesh3D mesh, SharedPtr<Material> material)
{
  MutexLockGuard guard(_mutex);
  unsigned int handle = GenerateMeshHandle();
  mesh.SetMaterial(material);
  mesh.SetShader(&_defaultShader);
  mesh.SetID(handle);

  _new3DMeshes.push_back(mesh);
  return handle;
}

unsigned int Renderer::AddPointCloud(const void* pointData, size_t numPoints, Color color)
{
  if (!_running)
  {
    throw std::runtime_error("Renderer was not started before being sent data!");
  }

  MutexLockGuard guard(_mutex);

  unsigned int handle = GenerateMeshHandle();
  const PointCloud<VertexP4>::VertexType* verts = reinterpret_cast<const PointCloud<VertexP4>::VertexType*>(pointData);

  PointCloud<VertexP4> pointcloud;

  pointcloud.SetPoints(verts, numPoints);
  pointcloud.SetID(handle);
  pointcloud.SetShader(&_pointCloudShader);
  pointcloud.SetMaterial(std::make_shared<FlatColorMaterial>(color));

  _pointClouds.push_back(std::move(pointcloud));

  return handle;
}

void Renderer::UpdatePointCloud(unsigned int handle, const void* pointData, size_t numPoints, Color color)
{
  const PointCloud<VertexP4>::VertexType* verts = reinterpret_cast<const PointCloud<VertexP4>::VertexType*>(pointData);
  for (auto& cloud : _pointClouds)
  {
    if (cloud.ID() == handle)
    {
      MutexLockGuard guard(_mutex);
      cloud.SetPoints(verts, numPoints); /// TODO: Update pointcloud's color
      break;
    }
  }
}

void Renderer::Update(unsigned int handle, Mesh3D mesh, SharedPtr<Material> material)
{
  MutexLockGuard guard(_mutex);
  RemoveMesh(handle);

  mesh.SetMaterial(material);
  mesh.SetShader(&_defaultShader);
  mesh.SetID(handle);
  _new3DMeshes.push_back(mesh);
}

void Renderer::UpdateTransform(unsigned int handle, glm::mat4 transform, bool absolute)
{
  if (handle == 0) { return; }
  MutexLockGuard guard(_mutex);
  for (auto& m : _3DMeshes)
  {
    if (m.ID() == handle)
    {
      if (absolute)
      {
        m.SetTransform(transform);
      }
      else
      {
        m.SetTransform(transform * m.GetTransform());
      }
    }
  }
  for (auto& m : _new3DMeshes)
  {
    if (m.ID() == handle)
    {
      if (absolute)
      {
        m.SetTransform(transform);
      }
      else
      {
        m.SetTransform(transform * m.GetTransform());
      }
    }
  }
  for (auto& c : _pointClouds)
  {
    if (c.ID() == handle)
    {
      if (absolute)
      {
        c.SetTransform(transform);
      }
      else
      {
        c.SetTransform(transform * c.GetTransform());
      }
    }
  }
}

void Renderer::RemoveMesh(unsigned int handle)
{
  if (handle == 0) { return; } // zero is reserved as a non-unique default ID

  for (auto& m : _3DMeshes)
  {
    if (m.ID() == handle)
    {
      m.MarkForDeletion();
    }
  }
  for (auto& m : _new3DMeshes)
  {
    if (m.ID() == handle)
    {
      m.MarkForDeletion();
    }
  }
  for (auto& c : _pointClouds)
  {
    if (c.ID() == handle)
    {
      c.MarkForDeletion();
    }
  }
}

void Renderer::RemoveAllMeshes()
{
  for (auto& m : _3DMeshes)
  {
    m.MarkForDeletion();
  }
  for (auto& m : _new3DMeshes)
  {
    m.MarkForDeletion();
  }
  for (auto& c : _pointClouds)
  {
    c.MarkForDeletion();
  }
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
  MutexLockGuard guard(_mutex);

  // setup OpenGL context and open a window for rendering
  Init_GL();

  // init buffers
  _2DMeshBuffer = UniquePtr<VertexBuffer<Vertex2D>>(new VertexBuffer<Vertex2D>());
  _3DMeshBuffer = UniquePtr<VertexBuffer<Vertex3D>>(new VertexBuffer<Vertex3D>());
  _voxelInstancedVertexBuffer = UniquePtr<InstancedVertexBuffer<VertexP3N3, VertexP3C4S>>(new InstancedVertexBuffer<VertexP3N3, VertexP3C4S>());

  // load shaders
  _defaultShader.loadAndLink(ShaderSources::sh_simpleNormal_vert, ShaderSources::sh_simpleLit_frag);
  _videoShader.loadAndLink(ShaderSources::sh_2D_passthru_vert, ShaderSources::sh_simpleTexture_frag);
  _pointCloudShader.loadAndLink(ShaderSources::sh_pointCloud_vert, ShaderSources::sh_flatShaded_frag);
  _voxelShader.loadAndLink(ShaderSources::sh_voxel_vert, ShaderSources::sh_voxel_frag);

  // setup default goemetry needed for rendering and send it to OpenGL
  Init_geometry();

  // generate texture handles and allocate space for default textures
  Init_textures();

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

void Renderer::Init_geometry()
{
  Mesh2D videoPane = MeshFactory::MakeQuad<Mesh2D>(glm::vec2(0, 0), 2.0, 2.0); // vertices range from -1..1
  videoPane.SetShader(&_videoShader);
  _2DMeshes.push_back(videoPane); //TexturedQuad(videoVertices, videoIndices, &_videoShader));

  auto voxel_base_mesh = MeshFactory::MakeCube<Mesh3D>(glm::vec3(0, 0, 0), 1.0);
  _voxelInstancedVertexBuffer->SetVertices(voxel_base_mesh.GetVertices());
  _voxelInstancedVertexBuffer->SetIndices(voxel_base_mesh.GetIndices());
}

void Renderer::Init_textures()
{
    // initialize video texture to be the same size as our window
    _videoWidth = _windowWidth; _videoHeight = _windowHeight;
    _currentVideoFrame = UniquePtr<unsigned char[]>(new unsigned char[_videoWidth * _videoHeight * 3]());
    for (size_t i = 0; i < _videoWidth * _videoHeight * 3; i++)
    {
      _currentVideoFrame[i] = 50;
    }

    GLuint videoTexture;
    glGenTextures(1, &videoTexture);
    BufferTexture(_videoWidth, _videoHeight, videoTexture, _currentVideoFrame.get());

    _2DMeshes[0].SetTexture(videoTexture); /// TODO: Don't do this...
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

void Renderer::BufferTexture(int width, int height, GLuint tex, unsigned char* pixels)
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

  // if we've received a new video frame, send it to the GPU
  if (_newVideoFrame)
  {
    MutexLockGuard guard(_mutex);
    BufferTexture(_videoWidth, _videoHeight, _2DMeshes[0].GetTexture(), _currentVideoFrame.get());
    _newVideoFrame = false;
  }

  // if one or more meshes was marked for deletion, delete it and regenerate VBO/VEOs
  // The implementation here causes us to trash and rebuild any VertexBuffer which contained
  // a deleted mesh. It may be faster to just delete the range of vertices covered by that
  // mesh, but we'd still have to go through all the other meshes to update their offsets
  // and re-send the modified buffer to the GPU, so the gains may not be terribly meaningful.
  bool mustRegenerateVBO_2D = false;
  for (auto it = _2DMeshes.begin(); it != _2DMeshes.end(); )
  {
    if (it->PendingDelete())
    {
      it = _2DMeshes.erase(it);
      _2DMeshBuffer->ClearAll();   // empty vertex buffer so it can be rebuilt
      mustRegenerateVBO_2D = true;
    }
    else
    {
      ++it;
    }
  }
  bool mustRegenerateVBO_3D = false;
  for (auto it = _3DMeshes.begin(); it != _3DMeshes.end(); )
  {
    if (it->PendingDelete())
    {
      it = _3DMeshes.erase(it);
      _3DMeshBuffer->ClearAll();
      mustRegenerateVBO_3D = true;
    }
    else
    {
      ++it;
    }
  }

  for (auto it = _pointClouds.begin(); it != _pointClouds.end(); )
  {
    if (it->PendingDelete())
    {
      it = _pointClouds.erase(it);
    }
    else
    {
      ++it;
    }
  }

  // if we have any new mesh data, update buffers and send it to the GPU
  _mutex.lock();

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
  for (auto& m : _new3DMeshes)
  {
    if (!m.PendingDelete()) // if a mesh was already deleted, don't bother adding it
      _3DMeshes.push_back(m);
  }
  _new3DMeshes.clear(); // empty new list since we've extracted all new meshes
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

  for (auto& cloud : _pointClouds)
  {
    if (cloud.Dirty())
    {
      cloud.UpdateBuffer();
    }

    if (cloud.GetVertexBuffer()->Dirty())
    {
      cloud.GetVertexBuffer()->BufferData();
    }
  }

  _voxelInstancedVertexBuffer->BufferData();

  _mutex.unlock();
}

void Renderer::RenderOneFrame()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*************
  * First pass:
  *   render all 2D textured shapes
  *************/
  EnableRenderPass(Blend_None);
  for (auto& m : _2DMeshes)
  {
    const auto& shader = m.GetShader();
    shader->enable();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.GetTexture());
    glUniform1i(shader->getUniform("tex"), 0);

    _2DMeshBuffer->Draw(_renderType, m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
  }

  /*************
  * Second pass:
  *   render point cloud
  *************/
  EnableRenderPass(Blend_Add);
  for (auto& cloud : _pointClouds)
  {
    if (cloud.ShouldDraw())
    {
      const auto& shader = cloud.GetShader();
      shader->enable();
      cloud.GetMaterial()->Apply();

      glm::mat4 mvp = GetProjectionMatrix() * GetViewMatrix() * cloud.GetTransform();
      glUniformMatrix4fv(shader->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
      glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, &cloud.GetTransform()[0][0]);
      glUniform1f(shader->getUniform("fadeDepth"), cloud._fadeDepth);


      GLfloat storedPointSize;
      glGetFloatv(GL_POINT_SIZE, &storedPointSize);
      glPointSize(cloud._pointSize);

      cloud.GetVertexBuffer()->Draw();

      glPointSize(storedPointSize);
    }
  }

  if (_voxelInstancedVertexBuffer->InstanceCount() > 0)
  {
    ShaderProgram& shader = _voxelShader;
    EnableRenderPass(Blend_None | EnableDepth);
    shader.enable();

    glm::mat4 modelTransform = glm::mat4(1.0f);

    // set uniforms
    glm::mat4 mv = GetViewMatrix() * modelTransform;
    glm::mat4 mvp = GetProjectionMatrix() * mv;
    glm::vec4 lightDirWorldSpace = GetViewMatrix() * glm::vec4(light_dir.x, light_dir.y, light_dir.z ,0);
    glm::vec3 ldir = glm::vec3(lightDirWorldSpace);
    glUniformMatrix4fv(shader.getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
    glUniformMatrix4fv(shader.getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(shader.getUniform("light_dir"), 1, &(ldir[0]));

    _voxelInstancedVertexBuffer->Draw(_renderType);
  }

  /*************
  * Third pass:
  *   render all 3D objects on top of the previous 2D shapes
  *************/
  EnableRenderPass(Blend_Add);
  for (auto& m : _3DMeshes)
  {
    const auto& shader = m.GetShader();
    shader->enable();

    // uniforms global to all objects
    glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, &(m.GetTransform()[0][0]));
    glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, &(GetViewMatrix()[0][0]));
    glUniform3fv(shader->getUniform("light_dir"), 1, &(light_dir[0]));

    // object-specific uniforms
    glm::mat4 mvp = GetProjectionMatrix() * GetViewMatrix() * m.GetTransform();
    glUniformMatrix4fv(shader->getUniform("MVP"), 1, GL_FALSE, &mvp[0][0]);
    m.GetMaterial()->Apply();

    _3DMeshBuffer->Draw(_renderType, m.IndexCount(), m.GetVertexOffset(), m.GetIndexOffset());
  }

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

  _renderGUIDelegate();

  for (auto& cloud : _pointClouds)
  {
    cloud.RenderGUI();
  }

  ImGui::Render();
  _imguiRenderer.RenderDrawLists(ImGui::GetDrawData());
}

void Renderer::Shutdown()
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

void Renderer::DrawVoxels(const Voxel* voxels, size_t numVoxels)
{
  if (!_running)
  {
    throw std::runtime_error("Renderer was not started before being sent data!");
  }

  MutexLockGuard guard(_mutex);

  const VertexP3C4S* vertices = reinterpret_cast<const VertexP3C4S*>(voxels);
  _voxelInstancedVertexBuffer->SetInstances(vertices, numVoxels);
}

} // namespace ar
