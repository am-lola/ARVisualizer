#ifndef _IMGUI_RENDERER_H
#define _IMGUI_RENDERER_H

struct GLFWwindow;
struct ImDrawData;

namespace ar
{


class ImguiRenderer
{
public:

  ImguiRenderer(GLFWwindow* window);

  // Initializes imgui (thread safe)
  void Init();
  // Shuts down imgui, if this is the last instance of this class (thread safe)
  void Shutdown();
  // Begin a new frame
  void NewFrame();

  // Use if you want to reset your rendering device without losing ImGui state.
  void InvalidateDeviceObjects();
  void CreateDeviceObjects();
  void RenderDrawLists(ImDrawData* draw_data);

  // Feed glfw callbacks here for user input
  void OnKeyPress(int key, int, int action, int mods);
  void OnKeyChar(unsigned int codepoint);
  void OnScroll(double xoffset, double yoffset);
  void OnMouseButton(int button, int action, int mods);

private:

  void CreateFontsTexture();

  GLFWwindow* _window;

  double _time = 0.0;
  unsigned int _fontTexture = 0;
  int _shaderHandle = 0;
  int _vertHandle = 0;
  int _fragHandle = 0;
  int _attribLocationTex = 0;
  int _attribLocationProjMtx = 0;
  int _attribLocationPosition = 0;
  int _attribLocationUV = 0;
  int _attribLocationColor = 0;
  unsigned int _vboHandle = 0;
  unsigned int _vaoHandle = 0;
  unsigned int _elementsHandle = 0;
  bool _mousePressed[3] = { false, false, false };
  double _mouseWheel = 0.0;

};

}

#endif // _IMGUI_RENDERER_H
