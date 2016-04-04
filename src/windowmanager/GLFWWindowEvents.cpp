#include "GLFWWindowEvents.hpp"

namespace ar
{

GLFWWindowEvents::GLFWWindowEvents(GLFWwindow* window)
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
  glfwSetWindowCloseCallback(window, glfw_windowclose_callback);
}

void GLFWWindowEvents::glfw_mousebutton_callback(GLFWwindow *window, int button, int action, int mods)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyMouseButtonCallback(button, action, mods);
  }
}

void GLFWWindowEvents::glfw_mousemove_callback(GLFWwindow *window, double xpos, double ypos)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyMouseMoveCallback(xpos, ypos);
  }
}

void GLFWWindowEvents::glfw_mouseenter_callback(GLFWwindow *window, int entered)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyMouseEnterExitCallback(entered);
  }
}

void GLFWWindowEvents::glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyScrollCallback(xoffset, yoffset);
  }
}

void GLFWWindowEvents::glfw_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyKeyboardKeyCallback(key, scancode, action, mods);
  }
}

void GLFWWindowEvents::glfw_char_callback(GLFWwindow *window, unsigned int codepoint)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyKeyboardCharCallback(codepoint);
  }
}

void GLFWWindowEvents::glfw_windowsize_callback(GLFWwindow *window, int width, int height)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyWindowSizeCallback(width, height);
  }
}

void GLFWWindowEvents::glfw_framebuffersize_callback(GLFWwindow *window, int width, int height)
{
  // get a 'this' reference
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyFrameBufferSizeCallback(width, height);
  }
}

void GLFWWindowEvents::glfw_windowclose_callback(GLFWwindow *window)
{
  WindowEvents* winEvents = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
  if (winEvents != nullptr)
  {
    winEvents->NotifyWindowCloseCallback();
  }
}

} // namespace ar
