#ifndef _WINDOWEVENTS_H
#define _WINDOWEVENTS_H

#include <functional>
#include <map>

namespace ar
{

// Provides an abstraction between incoming window input events and
// objects which need to consume them
class WindowEvents
{
public:
  typedef enum class
  {
    WindowSizeChanged,       // called when the user resizes the window
    FrameBufferSizeChanged,  // called when GL framebuffer is changed (in code, or due to window size changing)

    MouseButtonStateChanged, // called when any mouse button is pressed or released
    MouseEnterExit,          // called when the mouse enters or exits the window
    MouseMove,               // called any time the mouse moves over the window

    KeyboardStateChanged,    // called any time a keyboard key is pressed or released
    CharacterInput,          // called any time a printable character is received from the keyboard
    Scroll,                  // called when the user uses the scroll wheel on their mouse (or scrolls from the keyboard)
  } Event;

  typedef unsigned int event_handle;

  static const Event MouseButton        = Event::MouseButtonStateChanged;
  static const Event MouseEnterExit     = Event::MouseEnterExit;
  static const Event MouseMove          = Event::MouseMove;

  static const Event WindowResized      = Event::WindowSizeChanged;
  static const Event FrameBufferResized = Event::FrameBufferSizeChanged;

  static const Event KeyboardKey        = Event::KeyboardStateChanged;
  static const Event KeyboardChar       = Event::CharacterInput;
  static const Event Scroll             = Event::Scroll;

  // Registers a callback to be invoked when the given Event is detected
  event_handle SubscribeEvent(Event e, std::function<void(int)> cb);
  event_handle SubscribeEvent(Event e, std::function<void(int,int)> cb);
  event_handle SubscribeEvent(Event e, std::function<void(int,int,int)> cb);
  event_handle SubscribeEvent(Event e, std::function<void(int,int,int,int)> cb);
  event_handle SubscribeEvent(Event e, std::function<void(double,double)> cb);
  event_handle SubscribeEvent(Event e, std::function<void(unsigned int)> cb);

  // Ensures a previously-subscribed even handler is not called again in the future
  void UnsubscribeEvent(Event e, event_handle eh);

  // Calls all subscribed handlers for the corresponding events
  void NotifyMouseButtonCallback(int button, int action, int mods);
  void NotifyMouseMoveCallback(double xpos, double ypos);
  void NotifyMouseEnterExitCallback(int entered);
  void NotifyScrollCallback(double xoffset, double yoffset);
  void NotifyKeyboardKeyCallback(int key, int scancode, int action, int mods);
  void NotifyKeyboardCharCallback(unsigned int codepoint);
  void NotifyWindowSizeCallback(int width, int height);
  void NotifyFrameBufferSizeCallback(int width, int height);

  class UnknownWindowEventException : public std::runtime_error
  {
  public:
    UnknownWindowEventException(const std::string& what_arg) : std::runtime_error(what_arg) {}
  };

private:
  // callbacks we use to notify others when events happen
  std::map<event_handle, std::function<void(int)> >             _mouseEnterExitCallbacks;
  std::map<event_handle, std::function<void(int,int)> >         _windowSizeCallbacks;
  std::map<event_handle, std::function<void(int,int)> >         _frameBufferSizeCallbacks;
  std::map<event_handle, std::function<void(int,int,int)> >     _mouseButtonCallbacks;
  std::map<event_handle, std::function<void(int,int,int,int)> > _keyCallbacks;
  std::map<event_handle, std::function<void(double,double)> >   _mouseMoveCallbacks;
  std::map<event_handle, std::function<void(double,double)> >   _scrollCallbacks;
  std::map<event_handle, std::function<void(unsigned int)> >    _charCallbacks;

  // gets a new unique handle to assign to an event
  static event_handle NextHandle();
};

} // namespace ar

#endif // _WINDOWEVENTS_H
