#ifndef _ARCOLOR_H
#define _ARCOLOR_H

namespace ar
{
  // A simple structure for passing colors around
  struct Color
  {
    float r;
    float g;
    float b;
    float a;

    Color() : r(1.0), g(1.0), b(1.0), a(1.0) {};
    Color(float R, float G, float B) : r(R), g(G), b(B), a(1.0f) {};
    Color(float R, float G, float B, float A) : r(R), g(G), b(B), a(A) {};
  };

} // namespace ar

#endif // _ARCOLOR_H
