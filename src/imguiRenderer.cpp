// ImGui GLFW3 binding with OpenGL3 + shaders. Adapted from imgui_impl_glfw_gl3.cpp.
// https://github.com/ocornut/imgui

#include "imguiRenderer.hpp"
#include "common.hpp"
#include <mutex>

#include <imgui.h>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

namespace ar
{

static int _openInstances = 0;
static std::mutex _mutex;

ImguiRenderer::ImguiRenderer(GLFWwindow* window)
  : _window(window)
{
}

void ImguiRenderer::Init()
{
  MutexLockGuard guard(_mutex);

  _openInstances++;
  if (_openInstances > 1)
    return;

  ImGuiIO& io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  io.RenderDrawListsFn = nullptr;

  #ifdef _WIN32
  io.ImeWindowHandle = glfwGetWin32Window(window);
  #endif
}

void ImguiRenderer::Shutdown()
{
  MutexLockGuard guard(_mutex);

  if (_openInstances == 1)
  {
    InvalidateDeviceObjects();
    ImGui::Shutdown();
  }

  _openInstances--;
}

void ImguiRenderer::RenderDrawLists(ImDrawData* draw_data)
{
  // Backup GL state
  GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
  GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
  GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
  GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
  GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
  GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
  GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glActiveTexture(GL_TEXTURE0);

  // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
  ImGuiIO& io = ImGui::GetIO();
  int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  // Setup viewport, orthographic projection matrix
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  const float ortho_projection[4][4] =
  {
    { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
    { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
    { 0.0f,                  0.0f,                  -1.0f, 0.0f },
    {-1.0f,                  1.0f,                   0.0f, 1.0f },
  };
  glUseProgram(_shaderHandle);
  glUniform1i(_attribLocationTex, 0);
  glUniformMatrix4fv(_attribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
  glBindVertexArray(_vaoHandle);

  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = 0;

    glBindBuffer(GL_ARRAY_BUFFER, _vboHandle);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

    for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
    {
      if (pcmd->UserCallback)
      {
        pcmd->UserCallback(cmd_list, pcmd);
      }
      else
      {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state
  glUseProgram(last_program);
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
  glBindVertexArray(last_vertex_array);
  glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  glBlendFunc(last_blend_src, last_blend_dst);
  if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void ImguiRenderer::NewFrame()
{
  if (!_fontTexture)
    CreateDeviceObjects();

  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(_window, &w, &h);
  glfwGetFramebufferSize(_window, &display_w, &display_h);
  io.DisplaySize = ImVec2((float)w, (float)h);
  io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

  // Setup time step
  double current_time =  glfwGetTime();
  io.DeltaTime = _time > 0.0 ? (float)(current_time - _time) : (float)(1.0f/60.0f);
  _time = current_time;

  // Setup inputs
  // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
  if (glfwGetWindowAttrib(_window, GLFW_FOCUSED))
  {
    double mouse_x, mouse_y;
    glfwGetCursorPos(_window, &mouse_x, &mouse_y);
    io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
  }
  else
  {
    io.MousePos = ImVec2(-1,-1);
  }

  for (int i = 0; i < 3; i++)
  {
    io.MouseDown[i] = _mousePressed[i] || glfwGetMouseButton(_window, i) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    _mousePressed[i] = false;
  }

  io.MouseWheel = _mouseWheel;
  _mouseWheel = 0.0;

  // Hide OS mouse cursor if ImGui is drawing it
  glfwSetInputMode(_window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

  // Start the frame
  ImGui::NewFrame();
}

void ImguiRenderer::InvalidateDeviceObjects()
{
  if (_vaoHandle) glDeleteVertexArrays(1, &_vaoHandle);
  if (_vboHandle) glDeleteBuffers(1, &_vboHandle);
  if (_elementsHandle) glDeleteBuffers(1, &_elementsHandle);
  _vaoHandle = _vboHandle = _elementsHandle = 0;

  glDetachShader(_shaderHandle, _vertHandle);
  glDeleteShader(_vertHandle);
  _vertHandle = 0;

  glDetachShader(_shaderHandle, _fragHandle);
  glDeleteShader(_fragHandle);
  _fragHandle = 0;

  glDeleteProgram(_shaderHandle);
  _shaderHandle = 0;

  if (_fontTexture)
  {
    glDeleteTextures(1, &_fontTexture);
    ImGui::GetIO().Fonts->TexID = 0;
    _fontTexture = 0;
  }
}

void ImguiRenderer::CreateDeviceObjects()
{
  // Backup GL state
  GLint last_texture, last_array_buffer, last_vertex_array;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

  const GLchar *vertex_shader =
      "#version 330\n"
      "uniform mat4 ProjMtx;\n"
      "in vec2 Position;\n"
      "in vec2 UV;\n"
      "in vec4 Color;\n"
      "out vec2 Frag_UV;\n"
      "out vec4 Frag_Color;\n"
      "void main()\n"
      "{\n"
      " Frag_UV = UV;\n"
      " Frag_Color = Color;\n"
      " gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
      "}\n";

  const GLchar* fragment_shader =
      "#version 330\n"
      "uniform sampler2D Texture;\n"
      "in vec2 Frag_UV;\n"
      "in vec4 Frag_Color;\n"
      "out vec4 Out_Color;\n"
      "void main()\n"
      "{\n"
      " Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
      "}\n";

  _shaderHandle = glCreateProgram();
  _vertHandle = glCreateShader(GL_VERTEX_SHADER);
  _fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(_vertHandle, 1, &vertex_shader, 0);
  glShaderSource(_fragHandle, 1, &fragment_shader, 0);
  glCompileShader(_vertHandle);
  glCompileShader(_fragHandle);
  glAttachShader(_shaderHandle, _vertHandle);
  glAttachShader(_shaderHandle, _fragHandle);
  glLinkProgram(_shaderHandle);

  _attribLocationTex = glGetUniformLocation(_shaderHandle, "Texture");
  _attribLocationProjMtx = glGetUniformLocation(_shaderHandle, "ProjMtx");
  _attribLocationPosition = glGetAttribLocation(_shaderHandle, "Position");
  _attribLocationUV = glGetAttribLocation(_shaderHandle, "UV");
  _attribLocationColor = glGetAttribLocation(_shaderHandle, "Color");

  glGenBuffers(1, &_vboHandle);
  glGenBuffers(1, &_elementsHandle);

  glGenVertexArrays(1, &_vaoHandle);
  glBindVertexArray(_vaoHandle);
  glBindBuffer(GL_ARRAY_BUFFER, _vboHandle);
  glEnableVertexAttribArray(_attribLocationPosition);
  glEnableVertexAttribArray(_attribLocationUV);
  glEnableVertexAttribArray(_attribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
  glVertexAttribPointer(_attribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
  glVertexAttribPointer(_attribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
  glVertexAttribPointer(_attribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

  CreateFontsTexture();

  // Restore modified GL state
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindVertexArray(last_vertex_array);
}

void ImguiRenderer::CreateFontsTexture()
{
  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

  // Upload texture to graphics system
  GLint last_texture;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  glGenTextures(1, &_fontTexture);
  glBindTexture(GL_TEXTURE_2D, _fontTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)_fontTexture;

  // Restore state
  glBindTexture(GL_TEXTURE_2D, last_texture);
}

void ImguiRenderer::OnKeyPress(int key, int, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    (void)mods; // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
}

void ImguiRenderer::OnKeyChar(unsigned int codepoint)
{
  ImGuiIO& io = ImGui::GetIO();
  if (codepoint > 0 && codepoint < 0x10000)
      io.AddInputCharacter((unsigned short)codepoint);
}

void ImguiRenderer::OnMouseButton(int button, int action, int mods)
{
  if (action == GLFW_PRESS && button >= 0 && button < 3)
    _mousePressed[button] = true;
}

void ImguiRenderer::OnScroll(double /*xoffset*/, double yoffset)
{
  _mouseWheel += yoffset;
}

}
