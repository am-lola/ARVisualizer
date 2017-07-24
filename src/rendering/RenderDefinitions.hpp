#ifndef _ARRENDER_DEFINITIONS_HPP
#define _ARRENDER_DEFINITIONS_HPP

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

namespace ar
{

// OpenGL buffer usages
enum class BufferUsage
{
  Static,
  Dynamic,
  Stream
};

static GLenum GetGLUsage(BufferUsage usage)
{
  return usage == BufferUsage::Stream ? GL_STREAM_DRAW : (usage == BufferUsage::Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

} // namespace ar

#endif // _ARRENDER_DEFINITIONS_HPP
