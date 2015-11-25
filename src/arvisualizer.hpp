#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

namespace ar
{

class Renderer;

typedef unsigned int mesh_handle;

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

  mesh_handle AddTriangle(double vertexPositions[3][3], double color[3]);

  mesh_handle AddQuad(double* center, double* normal, double width, double height, double* color);

private:
  Renderer* _renderer;
};

} // namespace ar

#endif // _ARVISUALIZER_H
