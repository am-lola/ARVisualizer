#ifndef _ARVIDEO_RENDERING_HPP
#define _ARVIDEO_RENDERING_HPP

#include "RenderingCommon.hpp"
#include "ShaderProgram.hpp"
#include "mesh/Vertex.hpp"
#include "mesh/Mesh.hpp"

namespace ar
{

class VideoRenderer : public RenderComponent
{
public:

  VideoRenderer();

  void SetBackgroundColor(unsigned int);
  virtual void Init() override;
  virtual void Release() override;
  virtual void Update() override;
  virtual void RenderPass(const class SceneInfo& sceneInfo) override;

  void SetNewFrame(unsigned int width, unsigned int height, unsigned char* pixels);

private:

  static void BufferTexture(int width, int height, GLuint tex, unsigned char* pixels);

  ShaderProgram _shader;

  UniquePtr<unsigned char[]> _currentVideoFrame;
  unsigned int _videoWidth, _videoHeight;
  bool _newVideoFrame = false;


  GenericVertexBuffer<VertexP2T2> _vertexBuffer;
  GenericIndexBuffer _indexBuffer;
  TexturedMesh<VertexP2T2> _quadMesh;
  unsigned int _background = 0;
  enum BackgroundColor //TODO: unify with enum in Renderer.hpp
  {
    GREY = 0,
    BLACK = 1,
    WHITE = 2,
    SKYBLUE = 3
  };
};

}

#endif // _ARVIDEO_RENDERING_HPP
