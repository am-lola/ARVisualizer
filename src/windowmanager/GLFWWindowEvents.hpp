#ifndef _GLFWWINDOWEVENTS_H
#define _GLFWWINDOWEVENTS_H

#include "WindowEvents.hpp"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

namespace ar
{

// Wraps GLFW-specific calls to handle window events
class GLFWWindowEvents : public WindowEvents
{
public:
  GLFWWindowEvents( GLFWwindow *window );

private:
  // callbacks registered with GLFW to notify us when events happen
  static void glfw_mousebutton_callback( GLFWwindow *window, int button, int action, int mods );
  static void glfw_mousemove_callback( GLFWwindow *window, double xpos, double ypos );
  static void glfw_mouseenter_callback( GLFWwindow *window, int entered );
  static void glfw_scroll_callback( GLFWwindow *window, double xoffset, double yoffset );
  static void glfw_keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods );
  static void glfw_char_callback( GLFWwindow *window, unsigned int codepoint );
  static void glfw_windowsize_callback( GLFWwindow *window, int width, int height );
  static void glfw_framebuffersize_callback( GLFWwindow *window, int width, int height );
  static void glfw_windowclose_callback( GLFWwindow *window );
};

} // namespace ar

#endif // _GLFWWINDOWEVENTS_H
