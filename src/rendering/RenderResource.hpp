#ifndef _ARRENDER_RESOURCE_HPP
#define _ARRENDER_RESOURCE_HPP

#include <iostream>

namespace ar
{

// Render resource interface (buffers, textures etc.)
class RenderResource
{
public:
  virtual ~RenderResource()
  {
    if (_initialized)
    {
      std::cerr << "A RenderResource was not released!" << std::endl;
      Release();
    }
  };

  void Init()
  {
    if (_initialized)
      throw new std::runtime_error("Init was called but the resource was already initialized.");

    InitResource();

    _initialized = true;
  };

  void Release()
  {
    if (!_initialized)
      throw new std::runtime_error("Release was called but the resource was not initialized.");

    ReleaseResource();

    _initialized = false;
  }

protected:

  virtual void InitResource() = 0;
  virtual void ReleaseResource() = 0;

  bool _initialized = false;
};

}

#endif // _ARRENDER_RESOURCE_HPP
