#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

namespace ar
{

#define SPHERE_SUBDIV_LEVELS 2

class Renderer;

typedef unsigned int mesh_handle;

/*
  The main interface to the AR Visualization library
*/

class ARVisualizer
{
public:
  ARVisualizer();
  ~ARVisualizer();

  // Begins rendering
  void Start(int width, int height);

  // Stops all rendering activity
  void Stop();

  // True if the visualizer has been started
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

  mesh_handle AddTriangle(double vertexPositions[3][3], double color[4]);

  mesh_handle AddQuad(double center[3], double normal[3], double width, double height, double color[4]);

  mesh_handle AddSphere(double center[3], double radius, double color[4]);

private:
  Renderer* _renderer;
};

} // namespace ar

#endif // _ARVISUALIZER_H
