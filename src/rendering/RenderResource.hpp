#ifndef _ARRENDER_RESOURCE_HPP
#define _ARRENDER_RESOURCE_HPP

#include <iostream>

namespace ar
{

/*
 * Base class for render resources. Mainly used to wrap around OpenGL resources (buffers, textures etc.) but can also
 * be used for other classes that need initialization and releasing of render resources.
 */
class RenderResource
{
public:
  virtual ~RenderResource()
  {
    if (_initialized)
    {
      std::cerr << "A RenderResource was not released!" << std::endl;
      // Try to manually release - this may throw an exception.
      Release();
    }
  };

  // Initializes the resource (e.g. allocates buffers or generates vertex arrays).
  // Needs to be called on the render thread with a valid OpenGL context before first use!
  void Init()
  {
    if (_initialized)
      throw new std::runtime_error("Init was called but the resource was already initialized.");

    // gl* calls will all fail if a GL context doesn't exist
    if (glfwGetCurrentContext() == nullptr)
      throw std::runtime_error("The resource cannot be created without an OpenGL Context.");

    InitResource();

    _initialized = true;
  };

  // Releases the resource and frees all used memory. Needs to be called with a valid OpenGL context.
  void Release()
  {
    if (!_initialized)
      throw new std::runtime_error("Release was called but the resource was not initialized.");

    // gl* calls will all fail if a GL context doesn't exist
    if (glfwGetCurrentContext() == nullptr)
      throw std::runtime_error("The resource cannot be created without an OpenGL Context.");

    ReleaseResource();

    _initialized = false;
  }

protected:

  // Hooks for overriding classes
  virtual void InitResource() = 0;
  virtual void ReleaseResource() = 0;

  bool _initialized = false;
};

}

#endif // _ARRENDER_RESOURCE_HPP
