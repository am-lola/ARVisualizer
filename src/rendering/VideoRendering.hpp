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

  void SetBackgroundColor(unsigned char, unsigned char, unsigned char);
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
  bool _newColor = true;


  GenericVertexBuffer<VertexP2T2> _vertexBuffer;
  GenericIndexBuffer _indexBuffer;
  TexturedMesh<VertexP2T2> _quadMesh;
  unsigned char _background[3] = {50,50,50};
  UniquePtr<unsigned char[]> _backgroundpixels;
  unsigned int _bgWidth = 64;
  unsigned int _bgHeight = 64;
};

}

#endif // _ARVIDEO_RENDERING_HPP
