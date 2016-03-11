#ifndef _RENDERPASSPARAMS_H
#define _RENDERPASSPARAMS_H

namespace ar
{

// Flags used to set rendering parameters when drawing objects
typedef enum
{
  None        = 0x00,
  EnableDepth = 0x01,
  Blend_Add   = 0x02,
  Blend_Mul   = 0x04,
  Blend_Alpha = 0x08,
  Blend_None  = 0x10,
} RenderPassParams;

inline RenderPassParams operator|(RenderPassParams a, RenderPassParams b)
{
    return static_cast<RenderPassParams>(static_cast<int>(a) | static_cast<int>(b));
}
inline RenderPassParams& operator|=(RenderPassParams& a, RenderPassParams b)
{
    return a = a | b;
}
inline RenderPassParams operator&(RenderPassParams a, RenderPassParams b)
{
    return static_cast<RenderPassParams>(static_cast<int>(a) & static_cast<int>(b));
}
inline RenderPassParams& operator&=(RenderPassParams& a, RenderPassParams b)
{
    return a = a & b;
}

} // namespace ar

#endif // _RENDERPASSPARAMS_H
