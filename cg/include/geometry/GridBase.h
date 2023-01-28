//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GridBase.h
// ========
// Class definition for grid base.
//
// Author: Paulo Pagliosa
// Last revision: 27/01/2023

#ifndef __GridBase_h
#define __GridBase_h

#include "core/ContentHolder.h"
#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include "geometry/Index3.h"
#include <cassert>
#include <stdexcept>

namespace cg
{ // begin namespace cg

//
// Forward definitions
//
template <int D, typename T> class Grid;
template <int D, typename T> class GridData;


/////////////////////////////////////////////////////////////////////
//
// GridConstIterator: grid const iterator class
// =================
template <int D, typename T>
class GridConstIterator
{
public:
  using grid_type = Grid<D, T>;
  using id_type = typename grid_type::id_type;
  using const_iterator = GridConstIterator<D, T>;
  using value_type = const T;
  using pointer = value_type*;
  using reference = value_type&;

  GridConstIterator() = default;

  GridConstIterator(id_type id, const grid_type* grid):
    _grid{grid},
    _id{id}
  {
    // do nothing
  }

  reference operator *() const
  {
    return (*_grid)[_id];
  }

  const_iterator& operator ++()
  {
    ++_id;
    return *this;
  }

  const_iterator operator ++(int)
  {
    const_iterator temp{*this};

    _id++;
    return temp;
  }

  const_iterator& operator --()
  {
    --_id;
    return *this;
  }

  const_iterator operator --(int)
  {
    const_iterator temp{*this};

    _id--;
    return temp;
  }

  pointer operator ->() const
  {
    return &(operator *());
  }

  bool operator ==(const const_iterator& other) const
  {
    return _grid == other._grid && _id == other._id;
  }

  bool operator !=(const const_iterator& other) const
  {
    return !operator ==(other);
  }

  auto index() const
  {
    return _grid->index(_id);
  }

  auto id() const
  {
    return _id;
  }

private:
  const grid_type* _grid{};
  id_type _id{};

}; // GridConstIterator


/////////////////////////////////////////////////////////////////////
//
// GridIterator: grid iterator class
// ============
template <int D, typename T>
class GridIterator: public GridConstIterator<D, T>
{
public:
  using grid_type = Grid<D, T>;
  using id_type = typename grid_type::id_type;
  using const_iterator = GridConstIterator<D, T>;
  using iterator = GridIterator<D, T>;
  using value_type = T;
  using pointer = value_type*;
  using reference = value_type&;

  using const_iterator::GridConstIterator;

  reference operator *()
  {
    return (reference)**((const_iterator*)this);
  }

  iterator& operator ++()
  {
    ++*((const_iterator*)this);
    return *this;
  }

  iterator operator ++(int)
  {
    iterator temp{*this};

    ++*this;
    return temp;
  }

  iterator& operator --()
  {
    --*((const_iterator*)this);
    return *this;
  }

  iterator operator --(int)
  {
    iterator temp{*this};

    --*this;
    return temp;
  }

  pointer operator ->()
  {
    return &(operator *());
  }

}; // GridIterator


/////////////////////////////////////////////////////////////////////
//
// Grid: generic grid class
// ====
template <int D, typename T>
class Grid: public SharedObject
{
public:
  static_assert(D == 2 || D == 3, "Grid: bad dimension");

  using grid_type = Grid<D, T>;
  using id_type = int64_t;
  using index_type = Index<D, id_type>;
  using const_iterator = GridConstIterator<D, T>;
  using iterator = GridIterator<D, T>;
  using value_type = T;

  static constexpr auto dim()
  {
    return D;
  }

  Grid(const index_type& size):
    _data{size}
  {
    // do nothing
  }

  explicit Grid(id_type n):
    _data{index_type{n}}
  {
    // do nothing
  }

  Grid(const grid_type&) = delete;
  grid_type& operator =(const grid_type&) = delete;

  Grid(grid_type&& other):
    _data{std::move(other._data)}
  {
    // do nothing
  }

  const auto& size() const
  {
    return _data.size();
  }

  auto length() const
  {
    return _data.length();
  }

  auto id(const index_type& index) const
  {
    return _data.id(index);
  }

  auto index(id_type id) const
  {
    return _data.index(id);
  }

  const auto& operator [](id_type id) const
  {
    return _data[id];
  }

  auto& operator [](id_type id)
  {
    return _data[id];
  }

  const auto& operator [](const index_type& index) const
  {
    return (*this)[id(index)];
  }

  auto& operator [](const index_type& index)
  {
    return (*this)[id(index)];
  }

  /// Returns a const iterator to the beginning of this object.
  const_iterator cbegin() const
  {
    return const_iterator(0, this);
  }

  /// Returns a const iterator to the end of this object.
  const_iterator cend() const
  {
    return const_iterator(length(), this);
  }

  /// Returns an iterator to the beginning of this object.
  iterator begin() const
  {
    return iterator(0, this);
  }

  /// Returns an iterator to the end of this object.
  iterator end() const
  {
    return iterator(length(), this);
  }

protected:
  Grid() = default;

  const auto& data() const
  {
    return _data;
  }

  auto& data()
  {
    return _data;
  }

  void resize(const index_type& size)
  {
    _data.resize(size);
  }

private:
  GridData<D, T> _data;

}; // Grid


/////////////////////////////////////////////////////////////////////
//
// RegionGrid: region grid class
// ==========
template <int D, typename real, typename T>
class RegionGrid: public Grid<D, T>
{
public:
  ASSERT_REAL(real, "RegionGrid: floating point type expected");

  using Base = Grid<D, T>;
  using id_type = typename Base::id_type;
  using index_type = typename Base::index_type;
  using grid_type = RegionGrid<D, real, T>;
  using bounds_type = Bounds<real, D>;
  using vec_type = Vector<real, D>;

  using Base::index;
  using Base::id;
  using Base::operator [];

  static constexpr auto dflFatFactor = (real)1.01;

  static auto fatFactor()
  {
    return _fatFactor;
  }

  static void setFatFactor(real s)
  {
    if (s >= 1)
      _fatFactor = s;
  }

  RegionGrid(const bounds_type& bounds, real h);
  RegionGrid(const bounds_type& bounds, const index_type& size);

  RegionGrid(const bounds_type& bounds, id_type size):
    grid_type{bounds, index_type{size}}
  {
    // do nothing
  }

  RegionGrid(grid_type&& other):
    Base{std::move(other)},
    _bounds{other._bounds},
    _cellSize{other._cellSize},
    _inverseCellSize{other._inverseCellSize}
  {
    // do nothing
  }

  const auto& bounds() const
  {
    return _bounds;
  }

  auto cellSize() const
  {
    return _cellSize;
  }

  auto floatIndex(const vec_type& p) const
  {
    return (p - _bounds[0]) * _inverseCellSize;
  }

  auto index(const vec_type& p) const
  {
    return index_type{floatIndex(p)};
  }

  auto id(const vec_type& p) const
  {
    return Base::id(index(p));
  }

  const auto& operator [](const vec_type& p) const
  {
    return (*this)[id(p)];
  }

  auto& operator [](const vec_type& p)
  {
    return (*this)[id(p)];
  }

  bool contains(const vec_type& p) const
  {
    return _bounds.contains(p);
  }

  bool intersect(const Ray<real, D>& ray, real& tMin, real& tMax) const
  {
    return _bounds.intersect(ray, tMin, tMax);
  }

  auto basePoint(const index_type& index) const
  {
    return _bounds[0] + vec_type{index} * _cellSize;
  }

  auto basePoint(id_type id) const
  {
    return basePoint(Base::index(id));
  }

  auto bounds(const index_type& index) const
  {
    auto p = basePoint(index);
    return bounds_type{p, p + _cellSize};
  }

  auto bounds(id_type id) const
  {
    return bounds(Base::index(id));
  }

protected:
  bounds_type _bounds;
  vec_type _cellSize;
  vec_type _inverseCellSize;

private:
  static real _fatFactor;

}; // RegionGrid

template <int D, typename real, typename T>
inline real RegionGrid<D, real, T>::_fatFactor = dflFatFactor;

namespace internal::rg
{ // begin namespace internal::rg

template <typename real, int D>
inline auto
boundsSize(const Bounds<real, D>& bounds)
{
  auto s = bounds.size();

  for (int i = 0; i < D; i++)
    if (s[i] <= 0)
      throw std::runtime_error("RegionGrid: bad bounds");
  return s;
}

} // end namespace internal::rg

template <int D, typename real, typename T>
RegionGrid<D, real, T>::RegionGrid(const bounds_type& bounds, real h):
  _bounds{bounds}
{
  if (h <= 0)
    throw std::runtime_error("RegionGrid: bad cell size");
  _bounds.inflate(_fatFactor);

  const auto s = internal::rg::boundsSize(_bounds);
  const auto invH = math::inverse(h);
  index_type size;

  for (int i = 0; i < D; ++i)
    size[i] = id_type(ceil(s[i] * invH));
  _bounds.inflate(_bounds.min() + vec_type{size} * h);
  Base::resize(size);
  _inverseCellSize.set(invH);
  _cellSize.set(h);
}

template <int D, typename real, typename T>
RegionGrid<D, real, T>::RegionGrid(const bounds_type& bounds,
  const index_type& size):
  _bounds{bounds}
{
  _bounds.inflate(_fatFactor);

  auto s = internal::rg::boundsSize(_bounds);

  Base::resize(size);
  for (int i = 0; i < D; ++i)
    _inverseCellSize[i] = real(size[i] / s[i]);
  _cellSize = _inverseCellSize.inverse();
}


/////////////////////////////////////////////////////////////////////
//
// GridDataBase: region grid class
// ============
template <int D, typename T>
class GridDataBase
{
public:
  ASSERT_NOT_VOID(T, "Grid data type cannot be void");

  using id_type = typename Grid<D, T>::id_type;
  using index_type = typename Grid<D, T>::index_type;

  GridDataBase():
    _size{0}
  {
    // do nothing
  }

  GridDataBase(const index_type& size)
  {
    resize(size);
  }

  GridDataBase(GridDataBase<D, T>&& other)
  {
    _data = other._data;
    _length = other._length;
    _size = other._size;
    other._data = nullptr;
  }

  ~GridDataBase()
  {
    delete []_data;
  }

  void resize(const index_type& size);

  const auto& size() const
  {
    return _size;
  }

  auto length() const
  {
    return _length;
  }

  const auto& operator [](id_type id) const
  {
    assert(id >= 0 && id < _length);
    return _data[id];
  }

  auto& operator [](id_type id)
  {
    assert(id >= 0 && id < _length);
    return _data[id];
  }

protected:
  T* _data{};
  id_type _length{};
  index_type _size;

}; // GridDataBase

template <int D, typename T>
void
GridDataBase<D, T>::resize(const index_type& size)
{
  auto length = size.prod();

  if (length <= 0)
    throw std::runtime_error("GridData: bad size");
  if (length != _length)
  {
    delete []_data;
    _data = new T[_length = length];
  }
  _size = size;
}

} // end namespace cg

#endif // __GridBase_h
