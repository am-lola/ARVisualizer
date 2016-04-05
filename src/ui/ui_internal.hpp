#ifndef _UI_INTERNAL_H
#define _UI_INTERNAL_H

#include "ui.hpp"
#include "common.hpp"

namespace ar
{

class Renderer;

IUIWindow* createUIWindow(const char* name, float initialWidth, float initialHeight, Renderer* renderer);

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

  IUIWindow* AddWindow(Renderer* renderer, const char* name, float initialWidth = 0.0f, float initialHeight = 0.0f)
  {
    IUIWindow* window = createUIWindow(name, initialWidth, initialHeight, renderer);
    _windows.push_back(window);
    return window;
  }

  void RemoveWindow(IUIWindow* window)
  {
    // TODO: Could be more optimized
    _windows.erase(std::find(_windows.begin(), _windows.end(), window));
    delete window;
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
