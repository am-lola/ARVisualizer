#include "windowevents.hpp"

namespace ar
{

WindowEvents::WindowEvents(GLFWwindow* window)
{
  // give GLFW a reference to ourself so we can get it in the callbacks
  glfwSetWindowUserPointer(window, this);

  // register our callbacks with GLFW
  glfwSetMouseButtonCallback(window, glfw_mousebutton_callback);
  glfwSetCursorEnterCallback(window, glfw_mouseenter_callback);
  glfwSetCursorPosCallback(window, glfw_mousemove_callback);
  glfwSetKeyCallback(window, glfw_keyboard_callback);
  glfwSetCharCallback(window, glfw_char_callback);
  glfwSetScrollCallback(window, glfw_scroll_callback);
  glfwSetWindowSizeCallback(window, glfw_windowsize_callback);
  glfwSetFramebufferSizeCallback(window, glfw_framebuffersize_callback);
}

void WindowEvents::NotifyMouseButtonCallback(int button, int action, int mods)
{
  for (const auto& cb : _mouseButtonCallbacks)
  {
    cb.second(button, action, mods);
  }
}

void WindowEvents::NotifyMouseMoveCallback(double xpos, double ypos)
{
  for (const auto& cb : _mouseMoveCallbacks)
  {
    cb.second(xpos, ypos);
  }
}

void WindowEvents::NotifyMouseEnterExitCallback(int entered)
{
  for (const auto& cb : _mouseEnterExitCallbacks)
  {
    cb.second(entered);
  }
}

void WindowEvents::NotifyScrollCallback(double xoffset, double yoffset)
{
  for (const auto& cb : _scrollCallbacks)
  {
    cb.second(xoffset, yoffset);
  }
}

void WindowEvents::NotifyKeyboardKeyCallback(int key, int scancode, int action, int mods)
{
  for (const auto& cb : _keyCallbacks)
  {
    cb.second(key, scancode, action, mods);
  }
}

void WindowEvents::NotifyKeyboardCharCallback(unsigned int codepoint)
{
  for (const auto& cb : _charCallbacks)
  {
    cb.second(codepoint);
  }
}

void WindowEvents::NotifyWindowSizeCallback(int width, int height)
{
  for (const auto& cb : _windowSizeCallbacks)
  {
    cb.second(width, height);
  }
}

void WindowEvents::NotifyFrameBufferSizeCallback(int width, int height)
{
  for (const auto& cb : _frameBufferSizeCallbacks)
  {
    cb.second(width, height);
  }
}

WindowEvents::event_handle WindowEvents::SubscribeEvent(Event e, std::function<void(int)> cb)
{
  event_handle eh = NextHandle();
  switch (e)
  {
    case Event::MouseEnterExit:
    {
      _mouseEnterExitCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }

  return eh;
}

WindowEvents::event_handle WindowEvents::SubscribeEvent(Event e, std::function<void(int,int)> cb)
{
  event_handle eh = NextHandle();
  switch (e)
  {
    case Event::WindowSizeChanged:
    {
      _windowSizeCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    case Event::FrameBufferSizeChanged:
    {
      _frameBufferSizeCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }

  return eh;
}

WindowEvents::event_handle WindowEvents::SubscribeEvent(Event e, std::function<void(int,int,int)> cb)
{
  event_handle eh = NextHandle();
  switch (e)
  {
    case Event::MouseButtonStateChanged:
    {
      _mouseButtonCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }

  return eh;
}

WindowEvents::event_handle WindowEvents::SubscribeEvent(Event e, std::function<void(int,int,int,int)> cb)
{
  event_handle eh = NextHandle();
  switch (e)
  {
    case Event::KeyboardStateChanged:
    {
      _keyCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }

  return eh;
}

WindowEvents::event_handle WindowEvents::SubscribeEvent(Event e, std::function<void(double,double)> cb)
{
  event_handle eh = NextHandle();
  switch (e)
  {
    case Event::MouseMove:
    {
      _mouseMoveCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    case Event::Scroll:
    {
      _scrollCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }

  return eh;
}

WindowEvents::event_handle WindowEvents::SubscribeEvent(Event e, std::function<void(unsigned int)> cb)
{
  event_handle eh = NextHandle();
  switch (e)
  {
    case Event::CharacterInput:
    {
      _charCallbacks.insert(std::make_pair(eh, cb));
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }

  return eh;
}

void WindowEvents::UnsubscribeEvent(Event e, event_handle eh)
{
  switch (e)
  {
    case Event::WindowSizeChanged:
    {
      _windowSizeCallbacks.erase(eh);
      break;
    }
    case Event::FrameBufferSizeChanged:
    {
      _frameBufferSizeCallbacks.erase(eh);
      break;
    }
    case Event::MouseButtonStateChanged:
    {
      _mouseButtonCallbacks.erase(eh);
      break;
    }
    case Event::MouseEnterExit:
    {
      _mouseEnterExitCallbacks.erase(eh);
      break;
    }
    case Event::MouseMove:
    {
      _mouseMoveCallbacks.erase(eh);
      break;
    }
    case Event::KeyboardStateChanged:
    {
      _keyCallbacks.erase(eh);
      break;
    }
    case Event::CharacterInput:
    {
      _charCallbacks.erase(eh);
      break;
    }
    case Event::Scroll:
    {
      _scrollCallbacks.erase(eh);
      break;
    }
    default:
    {
      throw UnknownWindowEventException("Event: " + (int)e);
    }
  }
}

void WindowEvents::glfw_mousebutton_callback( GLFWwindow *window, int button, int action, int mods )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyMouseButtonCallback(button, action, mods);
  }
}

void WindowEvents::glfw_mousemove_callback( GLFWwindow *window, double xpos, double ypos )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyMouseMoveCallback(xpos, ypos);
  }
}

void WindowEvents::glfw_mouseenter_callback( GLFWwindow *window, int entered )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyMouseEnterExitCallback(entered);
  }
}

void WindowEvents::glfw_scroll_callback( GLFWwindow *window, double xoffset, double yoffset )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyScrollCallback(xoffset, yoffset);
  }
}

void WindowEvents::glfw_keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyKeyboardKeyCallback(key, scancode, action, mods);
  }
}

void WindowEvents::glfw_char_callback( GLFWwindow *window, unsigned int codepoint )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyKeyboardCharCallback(codepoint);
  }
}

void WindowEvents::glfw_windowsize_callback( GLFWwindow *window, int width, int height )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyWindowSizeCallback(width, height);
  }
}

void WindowEvents::glfw_framebuffersize_callback( GLFWwindow *window, int width, int height )
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != NULL)
  {
    winEvents->NotifyFrameBufferSizeCallback(width, height);
  }
}

WindowEvents::event_handle WindowEvents::NextHandle()
{
  static event_handle next = 0;
  return ++next;
}

} // namespace ar
