#ifndef _WMCOMMAND_H
#define _WMCOMMAND_H

namespace ar
{

class Renderer;

// The commands <WindowManager> is able to process
enum WMCommandType
{
  OPEN_WINDOW,
  CLOSE_WINDOW,
  SHUTDOWN
};

// Interface for commands submitted to <WindowManager>
struct IWMCommand
{
  // Constructor
  // @type Which <WMCommandType> this command represents
  IWMCommand(WMCommandType type) : commandType(type) {}

  // Flag for which kind of command this is
  WMCommandType commandType;
};

// Command for creating a new <Renderer>
struct CreateRendererParams : IWMCommand
{
  CreateRendererParams() : IWMCommand(OPEN_WINDOW) {}

  int windowWidth;
  int windowHeight;
  std::string windowName;
  // where to store the resulting renderer after creating it
  Renderer** resultLocation;
};

// Command for destroying an existing <Renderer>
struct DestroyRendererParams : IWMCommand
{
  DestroyRendererParams() : IWMCommand(CLOSE_WINDOW) {}

  // Renderer to shut down & close
  Renderer* renderer;
  // Window corresponding to the renderer
  GLFWwindow* window;
};

} // namespace ar

#endif // _WMCOMMAND_H
