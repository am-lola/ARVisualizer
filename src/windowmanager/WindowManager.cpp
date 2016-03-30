#include "WindowManager.hpp"

namespace ar
{

WindowManager::WindowManager()
{
  _mgrThread = std::thread([this] ()
  {
    this->mgrInit();
    this->mgrWork();
  });
}

WindowManager::~WindowManager()
{
  // ensure manager thread knows we want to shutdown
  if (!_shuttingDown)
  {
    // ensure all renderers are stopped before their windows are destroyed
    for (auto& r : _renderers)
    {
      r->Stop();
    }

    SendCommand(new IWMCommand(SHUTDOWN));
  }

  // Wait for manager thread to complete
  _mgrThread.join();
}

Renderer* WindowManager::NewRenderer(int windowWidth, int windowHeight, std::string windowName)
{
  std::unique_lock<std::mutex> cmdLock(_commandLock);
  Renderer* renderer = NULL;

  CreateRendererParams* rendererParams = new CreateRendererParams();
  rendererParams->windowHeight = windowHeight;
  rendererParams->windowWidth = windowWidth;
  rendererParams->windowName = windowName;
  rendererParams->resultLocation = &renderer;

  SendCommand(rendererParams);

  // Wait for the renderer to be created to ensure the caller never gets a bad pointer
  _commandComplete.wait(cmdLock, [rendererPtr=&renderer]{return *rendererPtr != NULL;});

  return renderer;
}

// TODO: If there are no renderers left, shutdown, clean up GLFW, etc.
void WindowManager::DeleteRenderer(Renderer* renderer, GLFWwindow* window)
{
  std::unique_lock<std::mutex> cmdLock(_commandLock);
  // remove renderer from tracked list
  _renderers.erase(std::remove(_renderers.begin(), _renderers.end(), renderer), _renderers.end());

  // send command to manager thread to destroy the renderer's window
  DestroyRendererParams* destroyCmd = new DestroyRendererParams();
  destroyCmd->renderer = renderer;
  destroyCmd->window = window;

  SendCommand(destroyCmd);
}

void WindowManager::mgrInit()
{
  if (!glfwInit())
  {
    throw std::runtime_error("Failed to initialize GLFW!");
  }
}

void WindowManager::mgrWork()
{
  while (!_shuttingDown)
  {
    if (_commandQueue.size() > 0)
    {
      IWMCommand* newCommand = _commandQueue.front();

      switch (newCommand->commandType)
      {
        case OPEN_WINDOW:
        {
          CreateRendererParams* openWindowCmd = static_cast<CreateRendererParams*>(newCommand);
          mgrCreateRenderer(openWindowCmd);
          break;
        }
        case CLOSE_WINDOW:
        {
          DestroyRendererParams* closeWindowCmd = static_cast<DestroyRendererParams*>(newCommand);
          mgrDestroyRenderer(closeWindowCmd);
          break;
        }
        case SHUTDOWN:
        {
          _shuttingDown = true;
          break;
        }
        default:
        {
          throw std::runtime_error("Unknown Command type '" + std::to_string(newCommand->commandType) + "' given to Window Manager");
          break;
        }
      }

      // take a lock on the queue before messing with it
      std::unique_lock<std::mutex> cmdLock(_commandLock);
      _commandQueue.pop();
      delete(newCommand);
      cmdLock.unlock();

      // let waiting threads know a command is done & they should check their predicates
      _commandComplete.notify_all();
    }
    else
    {
      // wait for a new command
      std::unique_lock<std::mutex> cmdLock(_commandLock);
      _commandAvailable.wait(cmdLock, [this]{return this->_commandQueue.size() > 0;});
    }
  }

  // make sure all GLFW stuff is cleaned up after we're done
  glfwTerminate();
}

// _commandLock should be acquired *before* calling this!
void WindowManager::SendCommand(IWMCommand* command)
{
  _commandQueue.push(command);
  _commandAvailable.notify_one();
}

void WindowManager::mgrCreateRenderer(CreateRendererParams* params)
{
  // create GL context and renderer
  GLFWwindow* win = mgrMakeWindow(params);
  Renderer* newRenderer = new Renderer(win);
  newRenderer->Start();
  // store pointer to new renderer for requesting thread to pick up
  *(params->resultLocation) = newRenderer;

  MutexLockGuard cmdLock(_commandLock);
  _renderers.push_back(newRenderer); // keep track of Renderers we create
}

GLFWwindow* WindowManager::mgrMakeWindow(CreateRendererParams* params)
{
  // tell GLFW what we want from OpenGL
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // OpenGL 3.3 core
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open window & create OpenGL context
  GLFWwindow* win = glfwCreateWindow(params->windowWidth, params->windowHeight, params->windowName.c_str(), NULL, NULL);
  if (!win)
  {
    glfwTerminate();
    throw std::runtime_error("Failed to open GLFW Window!");
  }

  return win;
}

void WindowManager::mgrDestroyRenderer(DestroyRendererParams* params)
{
  params->renderer->Stop();
  mgrCloseWindow(params->window);
  delete(params->renderer);
}

void WindowManager::mgrCloseWindow(GLFWwindow* window)
{
  glfwDestroyWindow(window);
}

} // namespace ar
