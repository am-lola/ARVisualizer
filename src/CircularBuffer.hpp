#ifndef _AR_CIRCULARBUFFER_HPP
#define _AR_CIRCULARBUFFER_HPP

#include <vector>
#include <stdexcept>

namespace ar
{

/*
 * A circular buffer which follows the STL naming conventions, but is incomplete (no iterators etc.).
 * This buffer overwrites elements once it is full.
 * It can be used as a dequeue as it can be accessed from the front and the back.
 * Once the buffer is full, pushing to the front will erase elements at the back, pushing to the back will erase elements at the front.
 */
template <typename T>
class CircularBuffer
{
private:
  // array holding the elements
  std::vector<T> _vec;
  // index of the first element
  size_t _start;
  // index of the next free element
  size_t _end;
  // size of the allocated array
  size_t _size;

  // decrease an index handle and wrap if below zero
  size_t dec(size_t idx) const
  {
    return idx == 0 ? _size - 1 : idx - 1;
  }

  // increase an index handle and wrap around
  size_t inc(size_t idx) const
  {
    return (idx + 1) % _size;
  }

  void init(size_t size)
  {
    _size = size + 1;
    _vec.resize(_size);
    _start = _end = 0;
  }

public:

  explicit CircularBuffer(size_t size)
  {
    init(size);
  }

#if __cplusplus >= 201103L

  CircularBuffer(const CircularBuffer& rhs) = default;
  CircularBuffer& operator=(const CircularBuffer& rhs) = default;

  CircularBuffer(CircularBuffer&& rhs) = default;
  CircularBuffer& operator=(CircularBuffer&& rhs) = default;

#endif

  void swap(CircularBuffer& rhs)
  {
    std::swap(_vec, rhs._vec);
    std::swap(_start, rhs._start);
    std::swap(_end, rhs._end);
    std::swap(_size, rhs._size);
  }

  friend void swap(CircularBuffer& a, CircularBuffer& b) { a.swap(b); }

  void push_back(const T& elem)
  {
    _vec[_end] = elem;

    _end = inc(_end);

    // let _end push _start forward (start overwriting the oldest elements)
    if (_end == _start)
      _start = inc(_start);
  }

  void push_front(const T& elem)
  {
    _start = dec(_start);
    _vec[_start] = elem;

    // let _start push _end backwards (start overwriting the newest elements)
    if (_end == _start)
      _end = dec(_end);
  }

  void pop_back()
  {
    if (size() == 0)
      return;

    _end = dec(_end);
  }

  void pop_front()
  {
    if (size() == 0)
      return;

    _start = inc(_start);
  }

  T& back() { return _vec[dec(_end)]; }
  const T& back() const { return _vec[dec(_end)]; }

  T& front() { return _vec[_start]; }
  const T& front() const { return _vec[_start]; }

  // TODO: This accepts all indices. Add a check if the index is valid.
  T& operator[](size_t idx) { return _vec[(_start + idx) % _size]; }
  const T& operator[] (size_t idx) const { return _vec[(_start + idx) % _size]; }

  size_t size() const
  {
    return _start <= _end ? _end - _start : _size - _start + _end;
  }

  bool empty() const { return size() == 0; }
  void clear() { _start = _end = 0; }

  void toVector(std::vector<T>& vector) const
  {
    vector.clear();

    if (_start < _end)
    {
      vector.insert(vector.begin(), _vec.begin() + _start, _vec.begin() + _end);
    }
    else if (_start > _end)
    {
      // insert from _start to _vec.end() and then from 0 to _end
      vector.insert(vector.begin(), _vec.begin() + _start, _vec.end());
      vector.insert(vector.end(), _vec.begin(), _vec.begin() + _end);
    }
  }

  void resize(size_t newSize)
  {
    if (newSize == _size - 1)
      return; // nothing changes

    std::vector<T> newVec;
    newVec.resize(newSize + 1);

    toVector(newVec);

    std::swap(_vec, newVec);

    _size = newSize + 1;
    _start = 0;
    _end = _size - 1;
  }

  // Does the same as clearing the buffer and pushing the elements from first to last one at a time
  void assign(const T* first, const T* last)
  {
    if (first > last)
      throw new std::invalid_argument("first > last");

    size_t dist = std::min(_size - 1, static_cast<size_t>(last - first));
    _vec.assign(last - dist, last);
    _start = 0;
    _end = dist;
  }
};

}

#endif // _AR_CIRCULARBUFFER_HPP
