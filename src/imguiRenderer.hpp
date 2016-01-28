#ifndef _IMGUI_RENDERER_H
#define _IMGUI_RENDERER_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

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
  GLuint _fontTexture = 0;
  GLuint _shaderHandle = 0;
  GLuint _vertHandle = 0;
  GLuint _fragHandle = 0;
  GLuint _vboHandle = 0;
  GLuint _vaoHandle = 0;
  GLuint _elementsHandle = 0;
  GLint _attribLocationTex = 0;
  GLint _attribLocationProjMtx = 0;
  GLint _attribLocationPosition = 0;
  GLint _attribLocationUV = 0;
  GLint _attribLocationColor = 0;
  bool _mousePressed[3] = { false, false, false };
  double _mouseWheel = 0.0;

};

}

#endif // _IMGUI_RENDERER_H
