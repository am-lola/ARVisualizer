#ifndef _ALIASES_H
#define _ALIASES_H

#include <memory>
#include <vector>
#include <mutex>

namespace ar
{
  template <typename T>
  using DefaultDelete = typename std::default_delete<T>;
  template <typename T, typename TD = DefaultDelete<T>>
  using UniquePtr = std::unique_ptr<T, TD>;
  template <typename T>
  using SharedPtr = std::shared_ptr<T>;
  template <typename T>
  using Vector = std::vector<T>;

  typedef std::lock_guard<std::mutex> MutexLockGuard;
}

#endif // _ALIASES_H
