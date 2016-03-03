#ifndef _WMCOMMAND_H
#define _WMCOMMAND_H

namespace ar
{

// The commands WindowManger is able to process are defined here


class Renderer;

enum WMCommandType
{
  OPEN_WINDOW,
  CLOSE_WINDOW,
  SHUTDOWN
};

struct IWMCommand
{
  IWMCommand(WMCommandType type) : commandType(type) {}
  WMCommandType commandType;
};

struct CreateRendererParams : IWMCommand
{
  CreateRendererParams() : IWMCommand(OPEN_WINDOW) {}

  int windowWidth;
  int windowHeight;
  std::string windowName;
  Renderer** resultLocation; // where to store the resulting renderer
};

struct DestroyRendererParams : IWMCommand
{
  DestroyRendererParams() : IWMCommand(CLOSE_WINDOW) {}

  Renderer* renderer; // Renderer to shut down & close
  GLFWwindow* window; // Window corresponding to the renderer
};

} // namespace ar

#endif // _WMCOMMAND_H
