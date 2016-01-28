#ifndef _WINDOWMANGER_H
#define _WINDOWMANGER_H

#include <queue>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <condition_variable>
#include <atomic>
#include "renderer.hpp"
#include "wmcommand.hpp"

namespace ar
{

// The WindowManager class manages contact with GLFW for the purposes of creating
//  and destroying windows and OpenGL contexts, which must always be done from the
//  same thread that calls glfwInit.
// To ensure we only ever have one thread calling into these sensitive parts of
//  GLFW, WindowManager is implemented as a singleton which owns a background thread
//  that makes all of the calls to GLFW. This ensures that client applications
//  don't need to worry about which thread they call into ARVisualizer from.
class WindowManager
{
public:
  static WindowManager& Instance()
  {
    static WindowManager instance; // static initializer is magically thread-safe in C++11
    return instance;               //   This will NOT work correctly if compiled differently!
  };

  // Creates a new Window & OpenGL context, then hands it off to a new Renderer instance
  Renderer* NewRenderer(int windowWidth, int windowHeight, std::string windowName);

  // Destroys the given renderer and its corresponding window
  void DeleteRenderer(Renderer* renderer, GLFWwindow* window);

private:
  WindowManager();
  ~WindowManager();

  // disable copy constructor
  WindowManager(const WindowManager& that) = delete;
  // disable assignment operator
  WindowManager& operator=(const WindowManager&) = delete;

  std::atomic_bool _shuttingDown {false}; // True if background thread should stop

  // Main Manager thread. This thread handles creation of OpenGL contexts, handoff
  // to the render threads, initialization and shutdown of GLFW, etc.
  // This is the "main thread" the GLFW docs refer to.
  std::thread _mgrThread;

  // Renderers. Every created window is given to a Renderer for drawing.
  std::vector<Renderer*> _renderers;

  // Queue of requests for the window manager thread to handle
  std::queue<IWMCommand*> _commandQueue;
  std::mutex _commandLock;
  std::condition_variable _commandAvailable;
  std::condition_variable _commandComplete;

  // Adds the given command to the queue and notifies the manager thread that
  //  there is new work to do.
  void SendCommand(IWMCommand* command);


  /*
    The following should ONLY be called from _mgrThread!!
    -----------------------------------------------------
  */

  // Initializes GLFW so we can create windows later
  void mgrInit();

  // Main loop for _mgrThread. Waits for commands from the queue and then executes them
  void mgrWork();

  // Handles a request for a new Renderer/Window. Creates a GLFWwindow, gives it
  //  to a new Renderer, and saves a pointer to the Renderer.
  void mgrCreateRenderer(CreateRendererParams* params);

  // Creates a new window with the specified parameters
  GLFWwindow* mgrMakeWindow(CreateRendererParams* params);

  // Handles a request to close a window.
  void mgrDestroyRenderer(DestroyRendererParams* params);

  // Destroys the given window.
  void mgrCloseWindow(GLFWwindow* window);
};

} // namespace ar

#endif // _WINDOWMANGER_H
