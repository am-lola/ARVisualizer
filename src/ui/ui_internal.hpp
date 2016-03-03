#ifndef _UI_INTERNAL_H
#define _UI_INTERNAL_H

#include "ui.hpp"
#include "common.hpp"

namespace ar
{

IUIWindow* createUIWindow(const char* name, float initialWidth, float initialHeight);

class UserInterface
{
public:

  ~UserInterface()
  {
    for (size_t i = 0; i < _windows.size(); i++)
    {
      delete _windows[i];
    }
  }

  IUIWindow* AddWindow(const char* name, float initialWidth = 0.0f, float initialHeight = 0.0f)
  {
    IUIWindow* window = createUIWindow(name, initialWidth, initialHeight);
    _windows.push_back(window);
    return window;
  }

  void draw()
  {
    for (size_t i = 0; i < _windows.size(); i++)
    {
      _windows[i]->drawElements();
    }
  }

private:

  Vector<IUIWindow*> _windows;
};

} // namespace ar

#endif // _UI_INTERNAL_H
