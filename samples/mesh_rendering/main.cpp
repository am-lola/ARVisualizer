#include <iostream>
#include <vector>
#include "arvisualizer.hpp"

/*
  A small sample application to show basic usage of ARVisualizer

  This sample just uses ARVisualizer to create and render several
  simple 3D meshes.
*/

int main(void)
{
  ar::ARVisualizer* visualizer = new ar::ARVisualizer();

  // Start the visualizer before doing anything else with it!
  // This opens the window used for rendering, creates the OpenGL context, etc.
  visualizer->Start(1024, 768);

  // make some shapes
  double quadCenter1[3] = {-0.5, 0.0, 2.0 };
  double quadNormal1[3] = { 1.0, 0.0,-1.0 };
  double quadColor1[4]  = { 0.5, 0.5, 0.25, 1.0 };
  double quadWidth1 = 1.0;
  double quadHeight1 = 1.5;

  double quadCenter2[3] = { 0.5, 0.0, 2.0 };
  double quadNormal2[3] = { 0.0, 1.0,-1.0 };
  double quadColor2[4]  = { 0.25, 0.0, 0.25, 0.5 };
  double quadWidth2 = 0.5;
  double quadHeight2 = 0.5;

  double triVertices1[3][3] = {
    { 0.0, 0.5, 3.0 },
    { 0.5, 0.25, 2.0 },
    { 0.0, 0.0, 1.0 }
  };
  double triColor1[4] = { 0.0, 0.5, 0.25, 0.75 };

  double triVertices2[3][3] = {
    { -0.5, 0.0, 1.0 },
    { -0.5, 0.0, 2.0 },
    { 0.5, -0.5, 2.0 }
  };
  double triColor2[4] = { 1.0, 0.5, 0.25, 0.25 };

  // tell the visualizer about our fancy new shapes
  visualizer->AddQuad(quadCenter1, quadNormal1, quadWidth1, quadHeight1, quadColor1);
  visualizer->AddQuad(quadCenter2, quadNormal2, quadWidth2, quadHeight2, quadColor2);
  visualizer->AddTriangle(triVertices1, triColor1);
  visualizer->AddTriangle(triVertices2, triColor2);

  // Keep rendering until the user wants to quit
  std::cout << "Press enter to exit..." << std::endl;
  std::cin.get();
  std::cout << "Shutting down..." << std::endl;

  // cleanup
  visualizer->Stop();
  delete(visualizer);
}
