#ifndef _WINDOWEVENTS_H
#define _WINDOWEVENTS_H

#include "Delegate.hpp"

namespace ar
{

// Provides an abstraction between incoming window input events and
// objects which need to consume them
class WindowEvents
{
public:

  // Calls all subscribed handlers for the corresponding events
  void NotifyMouseButtonCallback(int button, int action, int mods);
  void NotifyMouseMoveCallback(double xpos, double ypos);
  void NotifyMouseEnterExitCallback(int entered);
  void NotifyScrollCallback(double xoffset, double yoffset);
  void NotifyKeyboardKeyCallback(int key, int scancode, int action, int mods);
  void NotifyKeyboardCharCallback(unsigned int codepoint);
  void NotifyWindowSizeCallback(int width, int height);
  void NotifyFrameBufferSizeCallback(int width, int height);

  // Don't know if it's useful to keep these getters around...
  Delegate<void(int,int,int)>& GetMouseButtonDelegate() { return _mouseButtonDelegate; }
  Delegate<void(int)>& GetMouseEnterExitDelegate() { return _mouseEnterExitDelegate; }
  Delegate<void(double,double)>& GetMouseMoveDelegate() { return _mouseMoveDelegate; }
  Delegate<void(int,int)>& GetWindowResizedDelegate() { return _windowResizedDelegate; }
  Delegate<void(int,int)>& GetFrameBufferResizedDelegate() { return _frameBufferResizedDelegate; }
  Delegate<void(int,int,int,int)>& GetKeyboardKeyDelegate() { return _keyboardKeyDelegate; }
  Delegate<void(unsigned int)>& GetKeyboardCharDelegate() { return _keyboardCharDelegate; }
  Delegate<void(double,double)>& GetScrollDelegate() { return _scrollDelegate; }

private:
  Delegate<void(int,int,int)> _mouseButtonDelegate;
  Delegate<void(int)> _mouseEnterExitDelegate;
  Delegate<void(double,double)> _mouseMoveDelegate;
  Delegate<void(int,int)> _windowResizedDelegate;
  Delegate<void(int,int)> _frameBufferResizedDelegate;
  Delegate<void(int,int,int,int)> _keyboardKeyDelegate;
  Delegate<void(unsigned int)> _keyboardCharDelegate;
  Delegate<void(double,double)> _scrollDelegate;
};

} // namespace ar

#endif // _WINDOWEVENTS_H
