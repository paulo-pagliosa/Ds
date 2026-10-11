//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2026 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: PointArray.h
// ========
// Class definition for point array.
//
// Author: Paulo Pagliosa
// Last revision: 11/10/2026

#ifndef __PointArray_h
#define __PointArray_h

#include "core/Array.h"
#include "core/Rearrange.h"
#include "core/SoA.h"
#include "geometry/IndexList.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointArray: point array class
// ==========
template <typename Allocator, typename index_t, typename P, typename... Args>
class PointArray
{
public:
  ASSERT_SIGNED(index_t, "PointArray: signed integral type expected");

  using type = PointArray<Allocator, index_t, P, Args...>;
  using Data = SoA<Allocator, index_t, P, Args...>;
  using PointId = index_t;

  PointArray() = default;

  PointArray(index_t capacity):
    _data{capacity},
    _flag{capacity}
  {
    // do nothing
  }

  PointArray(const PointArray&) = delete;
  PointArray& operator =(const PointArray&) = delete;

  PointArray(PointArray&& other) noexcept:
    _data{std::move(other._data)},
    _flag{std::move(other._flag)},
    _size{std::exchange(other._size, 0)},
    _activeCount{std::exchange(other._activeCount, 0)},
    _freeList{std::exchange(other._freeList, eol)}
  {
    // do nothing
  }
 
  PointArray& operator =(PointArray&& other) noexcept
  {
    if (this != &other)
    {
      _data = std::move(other._data);
      _flag = std::move(other._flag);
      _size = std::exchange(other._size, 0);
      _activeCount = std::exchange(other._activeCount, 0);
      _freeList = std::exchange(other._freeList, eol);
    }
    return *this;
  }
 
  [[nodiscard]] auto capacity() const
  {
    return _data.size();
  }

  [[nodiscard]] auto size() const
  {
    return _size;
  }

  [[nodiscard]] auto activeCount() const
  {
    return _activeCount;
  }

  void reallocate(index_t capacity)
  {
    clear();
    try
    {
      _data.reallocate(capacity);
      _flag.reallocate(capacity);
    }
    catch (...)
    {
      // Keep both arrays with the same capacity
      _data.reallocate(0);
      _flag.reallocate(0);
      throw;
    }
  }

  void clear()
  {
    _size = _activeCount = 0;
    _freeList = eol;
  }

  PointId add(const P& p, const Args&... args);

  [[nodiscard]] bool isActive(PointId i) const
  {
    assert(i >= 0 && i < _size);
    return _flag.template get<0>(i) == activeFlag;
  }

  bool remove(PointId i)
  {
    return isActive(i) ? deactivate(i), true : false;
  }

  /// Rearranges the points according to the given order and then
  /// compacts this array. Afterwards, the active point with id k
  /// is the one whose original id was order[k], and the inactive
  /// points are discarded.
  template <IsIntegral I>
  void rearrange(std::span<I> order)
  {
    assert(order.size() == size_t(size()));
    rearrangeBySwap(order, [&](I i, I j)
    {
      swap(index_t(i), index_t(j));
    });
    compact();
  }

  /// Discards the inactive points, which MUST BE after the active
  /// ones: the size becomes activeCount() and the free list empty.
  index_t compact();

  template <size_t I>
  [[nodiscard]] auto& get(PointId i)
  {
    assert(isActive(i));
    return _data.template get<I>(i);
  }

  template <size_t I>
  [[nodiscard]] const auto& get(PointId i) const
  {
    return const_cast<PointArray*>(this)->template get<I>(i);
  }

  void set(PointId i, const P& p, const Args&... args)
  {
    assert(isActive(i));
    _data.set(i, p, args...);
  }

  [[nodiscard]] const auto& position(PointId i) const
  {
    return this->template get<0>(i);
  }

  [[nodiscard]] auto& position(PointId i)
  {
    return this->template get<0>(i);
  }

  void setPosition(PointId i, const P& p)
  {
    position(i) = p;
  }

  [[nodiscard]] const auto& operator [](PointId i) const
  {
    return position(i);
  }

  [[nodiscard]] auto& operator [](PointId i)
  {
    return position(i);
  }

  [[nodiscard]] auto cbegin() const
  {
    return _data.cbegin();
  }

  [[nodiscard]] auto cend() const
  {
    return typename Data::const_iterator{&_data, _size};
  }

  [[nodiscard]] auto begin()
  {
    return _data.begin();
  }

  [[nodiscard]] auto end()
  {
    return typename Data::iterator{&_data, _size};
  }

protected:
  void deactivate(PointId i)
  {
    if (!isActive(i))
      return;
    if (--_activeCount == 0)
    {
      _size = 0;
      _freeList = eol;
    }
    else
    {
      _flag.set(i, _freeList);
      _freeList = i;
    }
  }

private:
  using Flag = SoA<Allocator, index_t, index_t>;

  static constexpr index_t eol{-1};
  static constexpr index_t activeFlag{-2};

  Data _data;
  Flag _flag;
  index_t _size{};
  index_t _activeCount{};
  index_t _freeList{eol};

  // Exchanges the points i and j (either may be inactive).
  // The free list is not updated: used only by compact()
  // and rearrange(), which rebuild it.
  void swap(PointId i, PointId j)
  {
    _data.swap(i, j);
    _flag.swap(i, j);
  }

}; // PointArray

template <typename Allocator, typename index_t, typename P, typename... Args>
auto
PointArray<Allocator, index_t, P, Args...>::add(const P& p,
  const Args&... args) -> PointId
{
  PointId i;

  if (_freeList != eol)
    i = _freeList;
  else if (_size < capacity())
    i = _size;
  else
    return eol;
  // Store the point before changing any state: if an assignment
  // throws, the array is left unchanged
  _data.set(i, p, args...);
  if (i == _freeList)
    _freeList = _flag.template get<0>(i);
  else
    ++_size;
  _flag.set(i, activeFlag);
  _activeCount++;
  return i;
}

template <typename Allocator, typename index_t, typename P, typename... Args>
index_t
PointArray<Allocator, index_t, P, Args...>::compact()
{
  index_t c{};

  for (index_t i = 0; i < _size && c < _activeCount; ++i)
    if (isActive(i))
    {
      if (i != c)
        swap(c, i); // c is a hole: the active point moves down
      ++c;
    }
  _size = _activeCount;
  _freeList = eol;
  return c;
}

template <IsIntegral I,
  typename Allocator,
  typename index_t,
  typename P,
  typename... Args>
inline void
rearrange(PointArray<Allocator, index_t, P, Args...>& points,
  std::span<I> order)
{
  points.rearrange(order);
}

template <IsIntegral I,
  typename Allocator,
  typename index_t,
  typename P,
  typename... Args>
[[nodiscard]] inline bool
isPointActive(const PointArray<Allocator, index_t, P, Args...>& points, I i)
{
  return points.isActive(index_t(i));
}

} // end namespace cg

#endif // __PointArray_h
