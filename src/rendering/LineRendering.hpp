#ifndef _ARLINE_RENDERING_HPP
#define _ARLINE_RENDERING_HPP

#include "RenderingCommon.hpp"
#include "MeshRendering.hpp"

namespace ar
{

class LineRenderer : public MeshRenderer<VertexLine>
{
  typedef MeshRenderer<VertexLine> Base;

public:

  LineRenderer();

  virtual void Init() override;
  virtual void RenderPass(const SceneInfo& sceneInfo) override;

private:

  ShaderProgram _shader;
};

} // namespace ar

#endif // _ARLINE_RENDERING_HPP
