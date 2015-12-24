#include "windowevents.hpp"

namespace ar
{

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

WindowEvents::event_handle WindowEvents::NextHandle()
{
  static event_handle next = 0;
  return ++next;
}

} // namespace ar
