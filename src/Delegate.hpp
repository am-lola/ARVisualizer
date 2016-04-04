#ifndef _DELEGATE_H
#define _DELEGATE_H

#include <functional>
#include <vector>

namespace ar
{

template <typename>
class Delegate;

template <typename TReturn, typename... TArgs>
class Delegate<TReturn(TArgs...)>
{
private:
  using FuncType = std::function<TReturn(TArgs...)>;
  std::vector<FuncType> _funcs;

public:

  template <typename T>
  inline Delegate& operator+=(const T& func)
  {
    _funcs.emplace_back(func);
    return *this;
  }

  std::vector<TReturn> operator()(TArgs... args)
  {
    std::vector<TReturn> result;
    result.reserve(_funcs.size());
    for (const auto& fn : _funcs)
    {
      result.emplace_back(fn(args...));
    }

    return result;
  }

  inline void clear() noexcept { _funcs.clear(); }
  inline bool isEmpty() noexcept { return _funcs.empty(); }
};

// Template specialization for void return types
template <typename... TArgs>
class Delegate<void(TArgs...)>
{
private:
  using FuncType = std::function<void(TArgs...)>;
  std::vector<FuncType> _funcs;

public:

  template <typename T>
  inline Delegate& operator+=(const T& func)
  {
    _funcs.emplace_back(func);
    return *this;
  }

  inline void operator()(TArgs... args)
  {
    for (const auto& fn : _funcs)
    {
      fn(args...);
    }
  }

  inline void clear() noexcept { _funcs.clear(); }
  inline bool isEmpty() noexcept { return _funcs.empty(); }
};

}

#endif // _DELEGATE_H
