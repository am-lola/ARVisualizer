#include "arvisualizer.hpp"
#include "renderer.hpp"

#include <iostream>
#include <stdlib.h>

namespace ar
{

ARVisualizer::ARVisualizer()
{
  _renderer = new Renderer();
}

ARVisualizer::ARVisualizer(int windowWidth, int windowHeight)
{
  _renderer = new Renderer(windowWidth, windowHeight);
}

ARVisualizer::~ARVisualizer()
{
  if (_renderer->IsRunning())
  {
    Stop();
  }
}

void ARVisualizer::Start()
{
  _renderer->Start();
}

void ARVisualizer::Stop()
{
  _renderer->Stop();
}

bool ARVisualizer::IsRunning()
{
  return _renderer->IsRunning();
}

void ARVisualizer::NotifyNewVideoFrame(int width, int height, unsigned char* pixels)
{
  _renderer->NotifyNewVideoFrame(width, height, pixels);
}

} // namespace ar
