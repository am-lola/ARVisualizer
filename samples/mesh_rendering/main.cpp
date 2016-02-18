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

  /******************
    Make some shapes
   *******************/

   // Quads are built from a position, a normal, and width/height
   float quadCenter1[3] = {-0.5, 0.0, 2.0 };
   float quadNormal1[3] = { 1.0, 0.0,-1.0 };

   ar::Quad quad1 = ar::Quad(quadCenter1, quadNormal1, 1.0f, 1.5f, ar::Color( 0.5, 0.5, 0.25, 1.0 ));

  float quadCenter2[3] = { 0.5, 0.0, 2.0 };
  float quadNormal2[3] = { 0.0, 1.0,-1.0 };

  ar::Quad quad2 = ar::Quad(quadCenter2, quadNormal2, 0.5f, 0.5f, ar::Color( 0.25, 0.0, 0.25, 0.5 ));


   // Triangles are specified by three points in space
  float triVertices1[9] = {
      0.0, 0.5,  3.0,
      0.5, 0.25, 2.0,
      0.0, 0.0,  1.0
  };

  ar::Triangle tri1 = ar::Triangle(triVertices1, ar::Color( 0.0, 0.5, 0.25, 0.75 ));

  double triVertices2[3][3] = {
    { -0.5, 0.0, 1.0 },
    { -0.5, 0.0, 2.0 },
    { 0.5, -0.5, 2.0 }
  };

  ar::Triangle tri2 = ar::Triangle(triVertices2[0], triVertices2[1], triVertices2[2], ar::Color( 1.0, 0.5, 0.25, 0.25 ));

   // A sphere is specified with just a position and a radius
  float sphereCenter[3] = { 0.0, 0.0, 4.0 };

  ar::Sphere sphere = ar::Sphere(sphereCenter, 2.0, ar::Color( 0.6, 0.35, 0.2, 0.7 ));

   // Capsules need two positions (one for each end) and a radius
  double capsuleCenter1[3] = { 2.5, 2.0, 6.0 };
  double capsuleCenter2[3] = { 2.0,-1.0, 5.0 };
  double capsuleRadius = 1.0;
  ar::Color capsuleColor = ar::Color( 0.1, 0.8, 0.3, 1.0 );

  // tell the visualizer about our fancy new shapes
  visualizer->AddCapsule(capsuleCenter1, capsuleCenter2, capsuleRadius, capsuleColor);
  visualizer->Add(sphere);
  visualizer->Add(quad1);
  visualizer->Add(quad2);
  visualizer->Add(tri1);
  visualizer->Add(tri2);
  // Keep rendering until the user wants to quit
  std::cout << "Press enter to exit..." << std::endl;
  std::cin.get();
  std::cout << "Shutting down..." << std::endl;

  // cleanup
  visualizer->Stop();
  delete(visualizer);
}
