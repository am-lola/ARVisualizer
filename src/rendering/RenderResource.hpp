#ifndef _ARRENDER_RESOURCE_HPP
#define _ARRENDER_RESOURCE_HPP

namespace ar
{

// Render resource interface (buffers, textures etc.)
class RenderResource
{
public:
  virtual ~RenderResource() { }

  virtual void Init() = 0;
  virtual void Release() = 0;
};

}

#endif // _ARRENDER_RESOURCE_HPP
