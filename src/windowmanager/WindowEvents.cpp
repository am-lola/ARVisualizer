#include "WindowEvents.hpp"

namespace ar
{

void WindowEvents::NotifyMouseButtonCallback(int button, int action, int mods)
{
  _mouseButtonDelegate(button, action, mods);
}

void WindowEvents::NotifyMouseMoveCallback(double xpos, double ypos)
{
  _mouseMoveDelegate(xpos, ypos);
}

void WindowEvents::NotifyMouseEnterExitCallback(int entered)
{
  _mouseEnterExitDelegate(entered);
}

void WindowEvents::NotifyScrollCallback(double xoffset, double yoffset)
{
  _scrollDelegate(xoffset, yoffset);
}

void WindowEvents::NotifyKeyboardKeyCallback(int key, int scancode, int action, int mods)
{
  _keyboardKeyDelegate(key, scancode, action, mods);
}

void WindowEvents::NotifyKeyboardCharCallback(unsigned int codepoint)
{
  _keyboardCharDelegate(codepoint);
}

void WindowEvents::NotifyWindowSizeCallback(int width, int height)
{
  _windowResizedDelegate(width, height);
}

void WindowEvents::NotifyFrameBufferSizeCallback(int width, int height)
{
  _frameBufferResizedDelegate(width, height);
}

void WindowEvents::NotifyWindowCloseCallback()
{
  _windowCloseDelegate();
}

} // namespace ar
