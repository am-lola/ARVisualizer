#include <iostream>
#include <vector>
#include "arvisualizer.hpp"

struct quad
{
  double center[3];
  double normal[3];
  double color[3];
  double width;
  double height;
};

struct tri
{
  double vertices[3][3];
  double color[3];
};

int main(void)
{
  ar::ARVisualizer* visualizer = new ar::ARVisualizer(1024, 768);

  // start the visualizer before doing anything else with it!
  visualizer->Start();

  // make some shapes
  double quadCenter1[3] = {-0.5, 0.0, 2.0 };
  double quadNormal1[3] = { 1.0, 0.0,-1.0 };
  double quadColor1[3]  = { 0.5, 0.5, 0.25};
  double quadWidth1 = 1.0;
  double quadHeight1 = 1.5;

  double quadCenter2[3] = { 0.5, 0.0, 2.0 };
  double quadNormal2[3] = { 0.0, 1.0,-1.0 };
  double quadColor2[3]  = { 0.25, 0.0, 0.25};
  double quadWidth2 = 0.5;
  double quadHeight2 = 0.5;

  double triVertices1[3][3] = {
    { 0.0, 0.5, 3.0 },
    { 0.5, 0.25, 2.0 },
    { 0.0, 0.0, 1.0 }
  };
  double triColor1[] = { 0.0, 0.5, 0.25 };

  double triVertices2[3][3] = {
    { -0.5, 0.0, 1.0 },
    { -0.5, 0.0, 2.0 },
    { 0.5, -0.5, 2.0 }
  };
  double triColor2[] = { 1.0, 0.5, 0.25 };

  // tell the visualizer about our fancy new shapes
  visualizer->AddTriangle(triVertices1, triColor1);
  visualizer->AddQuad(quadCenter1, quadNormal1, quadWidth1, quadHeight1, quadColor1);
  visualizer->AddTriangle(triVertices2, triColor2);
  visualizer->AddQuad(quadCenter2, quadNormal2, quadWidth2, quadHeight2, quadColor2);


  std::cout << "Pres enter to exit..." << std::endl;
  std::cin.get();
  std::cout << "Shutting down..." << std::endl;

  // cleanup
  visualizer->Stop();
  delete(visualizer);
}
