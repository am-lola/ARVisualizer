#include "renderer.hpp"

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
  glGenVertexArrays(1, &_videoVAO);
  glBindVertexArray(_videoVAO);
  glGenBuffers(1, &_videoVBO);

  GLfloat videoVertices[] = {
  //  X      Y     U     V
    -1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 1.0f
  };

  glBindBuffer(GL_ARRAY_BUFFER, _videoVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*16, videoVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0, // position
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(GL_FLOAT),
    (void*)0
  );

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1, // texcoord
    2,
    GL_FLOAT,
    GL_FALSE,
    4 * sizeof(GL_FLOAT),
    (void*)(2 * sizeof(GL_FLOAT))
  );

  // reset GL state
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer::init_textures()
{
    // initialize video texture to be the same size as our window
    _currentVideoFrame = new unsigned char[_width * _height * 3];
    for (size_t i = 0; i < _width * _height * 3; i++)
    {
      _currentVideoFrame[i] = 50;
    }

    glGenTextures(1, &_videoTexture);
    bufferTexture(_width, _height, _videoTexture, _currentVideoFrame);
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
    if (_newVideoFrame)
    {
      std::lock_guard<std::mutex> guard(_mutex);
      bufferTexture(_width, _height, _videoTexture, _currentVideoFrame);
      _newVideoFrame = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderOneFrame();

    glfwSwapBuffers(_window);
    glfwPollEvents();
  }

  // if we've stopped rendering, close/destroy the window
  glfwDestroyWindow(_window);
}

void Renderer::renderOneFrame()
{
  _videoShader.enable();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _videoTexture);
  glUniform1i(_videoShader.getUniform("videotex"), 0);
  glBindVertexArray(_videoVAO);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  // cleanup
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  _videoShader.disable();
}

} // namespace ar
