#ifndef _COMMON_H
#define _COMMON_H

#include "aliases.hpp"

namespace ar
{

template<typename T>
T clamp(const T& val, const T& min, const T& max)
{
  return std::max(min, std::min(max, val));
}

}

#endif // _COMMON_H
