#ifndef _ARRENDERCOMPONENT_HPP
#define _ARRENDERCOMPONENT_HPP

namespace ar
{

class RenderComponent
{
public:

  virtual ~RenderComponent() { }

  virtual void Init() = 0;
  virtual void Release() = 0;
  virtual void Update() = 0;
  virtual void RenderPass(const class SceneInfo& sceneInfo) = 0;
  virtual void RenderGUI() { }
};

}

#endif // _ARRENDERCOMPONENT_HPP
