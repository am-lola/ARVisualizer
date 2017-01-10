#include <vtkDataSetToDataObjectFilter.h>
#include "VideoRendering.hpp"
#include "ShaderSources.g.hpp"
#include "mesh/MeshFactory.hpp"

namespace ar {

  VideoRenderer::VideoRenderer() {
  }

void VideoRenderer::SetBackgroundColor(unsigned char r, unsigned char g, unsigned char b)
{
  if((_background[0] == r) && (_background[1] == g) && (_background[2] == b))
    return;

  _newColor = true;
  _background[0] = r;
  _background[1] = g;
  _background[2] = b;
  _backgroundpixels = UniquePtr<unsigned char[]>(new unsigned char[_videoWidth * _videoHeight * 3]);

  for (size_t i = 0; i < _videoWidth * _videoHeight; i++) {
    _backgroundpixels[3*i] = _background[0];
    _backgroundpixels[3*i+1] = _background[1];
    _backgroundpixels[3*i+2] = _background[2];
  }

}

void VideoRenderer::Init()
{
  _vertexBuffer.Init();
  _indexBuffer.Init();
  _shader.loadAndLink(ShaderSources::sh_2D_passthru_vert, ShaderSources::sh_simpleTexture_frag);

  _videoWidth = 64; _videoHeight = 64;
  _currentVideoFrame = UniquePtr<unsigned char[]>(new unsigned char[_videoWidth * _videoHeight * 3]);

  for (size_t i = 0; i < _videoWidth * _videoHeight; i++) {
    _currentVideoFrame[3*i] = _background[0];
    _currentVideoFrame[3*i+1] = _background[1];
    _currentVideoFrame[3*i+2] = _background[2];
  }


  TexturedMesh<VertexP2T2> videoPane = MeshFactory::MakeQuad<TexturedMesh<VertexP2T2>>(glm::vec2(0, 0), 2.0, 2.0); // vertices range from -1..1
  videoPane.SetShader(&_shader);
  _quadMesh = videoPane;

  GLuint videoTexture;
  glGenTextures(1, &videoTexture);
  BufferTexture(_videoWidth, _videoHeight, videoTexture, _currentVideoFrame.get());

  _quadMesh.SetTexture(videoTexture);
}

void VideoRenderer::Release()
{
  _vertexBuffer.Release();
  _indexBuffer.Release();

  _currentVideoFrame.reset();

  GLuint tex = _quadMesh.GetTexture();
  glDeleteTextures(1, &tex);
}

void VideoRenderer::Update()
{
  // if we've received a new video frame, send it to the GPU
  if (_newVideoFrame)
  {
    BufferTexture(_videoWidth, _videoHeight, _quadMesh.GetTexture(), _currentVideoFrame.get());
    _newVideoFrame = false;
  }
  else if (_newColor)
  {
    BufferTexture(_videoWidth, _videoHeight, _quadMesh.GetTexture(), _backgroundpixels.get());
    _newColor = false;
  }

  if (_quadMesh.Dirty())
  {
    _quadMesh.SetVertexOffset(_vertexBuffer.AddVertices(_quadMesh.GetVertices()));
    _quadMesh.SetIndexOffset(_indexBuffer.AddIndices(_quadMesh.GetIndices()));
    _quadMesh.ClearDirty();
  }

  _vertexBuffer.BufferData();
  _indexBuffer.BufferData();
}
void VideoRenderer::RenderPass(const SceneInfo& sceneInfo)
{
  const auto& shader = _quadMesh.GetShader();
  shader->enable();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _quadMesh.GetTexture());
  glUniform1i(shader->getUniform("tex"), 0);

  glBindVertexArray(_vertexBuffer._vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer._vio);

  glDrawElementsBaseVertex(sceneInfo.renderType,
                           _quadMesh.IndexCount(),
                           GL_UNSIGNED_INT,
                           (void*)(_quadMesh.GetIndexOffset() * sizeof(GLuint)),
                           _quadMesh.GetVertexOffset());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void VideoRenderer::BufferTexture(int width, int height, GLuint tex, unsigned char* pixels)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGB,
    width,
    height,
    0,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    pixels
  );

  glBindTexture(GL_TEXTURE_2D, 0);
}

void VideoRenderer::SetNewFrame(unsigned int width, unsigned int height, unsigned char* pixels)
{
  if (width != _videoWidth || height != _videoHeight)
  {
    _currentVideoFrame.reset();
    _currentVideoFrame = UniquePtr<unsigned char[]>(new unsigned char[width * height * 3]());
    _videoWidth = width;
    _videoHeight = height;
  }

  // TODO: Memcpy
  for (size_t i = 0; i < width * height * 3; i++)
  {
    _currentVideoFrame[i] = pixels[i];
  }

  _newVideoFrame = true;
}

}
