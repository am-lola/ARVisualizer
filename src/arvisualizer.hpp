#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

namespace ar
{

class Renderer;

/*
  The main interface to the AR Visualization library
*/

class ARVisualizer
{
public:
  ARVisualizer();
  ARVisualizer(int windowWidth, int windowHeight);

  ~ARVisualizer();

  // Begins rendering
  void Start();

  // Stops all rendering activity
  void Stop();

  // True if the visualizer has been started
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

private:
  Renderer* _renderer;
};

} // namespace ar

#endif // _ARVISUALIZER_H
