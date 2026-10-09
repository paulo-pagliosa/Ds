//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// Last revision: 09/10/2026

#ifndef __GridBase_h
#define __GridBase_h

#include "core/ContentHolder.h"
#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include "geometry/Index3.h"
#include <cassert>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace cg
{ // begin namespace cg

template <typename T>
constexpr auto max_v = std::numeric_limits<T>::max();

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
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = const T*;
  using reference = const T&;

  GridConstIterator() = default;

  GridConstIterator(id_type id, const grid_type* grid):
    _grid{grid},
    _id{id}
  {
    // do nothing
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

  [[nodiscard]] bool operator ==(const const_iterator& other) const
  {
    assert(_grid == other._grid);
    return _id == other._id;
  }

  [[nodiscard]] bool operator !=(const const_iterator& other) const
  {
    return !operator ==(other);
  }

  [[nodiscard]] reference operator *() const
  {
    return (*_grid)[_id];
  }

  [[nodiscard]] pointer operator ->() const
  {
    return &(operator *());
  }

  [[nodiscard]] auto index() const
  {
    return _grid->cellIndex(_id);
  }

  [[nodiscard]] auto id() const
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
  using Base = GridConstIterator<D, T>;
  using grid_type = Grid<D, T>;
  using id_type = typename grid_type::id_type;
  using iterator = GridIterator<D, T>;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  GridIterator() = default;

  GridIterator(id_type id, grid_type* grid):
    Base{id, grid}
  {
    // do nothing
  }

  iterator& operator ++()
  {
    Base::operator ++();
    return *this;
  }

  iterator operator ++(int)
  {
    iterator temp{*this};

    Base::operator ++();
    return temp;
  }

  iterator& operator --()
  {
    Base::operator --();
    return *this;
  }

  iterator operator --(int)
  {
    iterator temp{*this};

    Base::operator --();
    return temp;
  }

  [[nodiscard]] reference operator *() const
  {
    return const_cast<reference>(Base::operator *());
  }

  [[nodiscard]] pointer operator ->() const
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
  using id_type = int32_t;
  using index_type = Index<D, id_type>;
  using const_iterator = GridConstIterator<D, T>;
  using iterator = GridIterator<D, T>;
  using value_type = T;

  [[nodiscard]] static constexpr auto dim()
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

  [[nodiscard]] const auto& size() const
  {
    return _data.size();
  }

  [[nodiscard]] auto length() const
  {
    return _data.length();
  }

  [[nodiscard]] auto cellId(const index_type& index) const
  {
    return _data.cellId(index);
  }

  [[nodiscard]] auto cellIndex(id_type id) const
  {
    return _data.cellIndex(id);
  }

  [[nodiscard]] const auto& operator [](id_type id) const
  {
    return _data[id];
  }

  [[nodiscard]] auto& operator [](id_type id)
  {
    return _data[id];
  }

  [[nodiscard]] const auto& operator [](const index_type& index) const
  {
    return (*this)[cellId(index)];
  }

  [[nodiscard]] auto& operator [](const index_type& index)
  {
    return (*this)[cellId(index)];
  }

  /// Returns a const iterator to the beginning of this object.
  [[nodiscard]] const_iterator cbegin() const
  {
    return const_iterator{0, this};
  }

  [[nodiscard]] const_iterator begin() const
  {
    return cbegin();
  }

  /// Returns a const iterator to the end of this object.
  [[nodiscard]] const_iterator cend() const
  {
    return const_iterator{length(), this};
  }

  [[nodiscard]] const_iterator end() const
  {
    return cend();
  }

  /// Returns an iterator to the beginning of this object.
  [[nodiscard]] iterator begin()
  {
    return iterator{0, this};
  }

  /// Returns an iterator to the end of this object.
  [[nodiscard]] iterator end()
  {
    return iterator{length(), this};
  }

  /// Returns a pointer to the cell array of this object.
  [[nodiscard]] const T* cells() const
  {
    return _data.cells();
  }

  [[nodiscard]] T* cells()
  {
    return _data.cells();
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
template <int D, IsReal R, typename T>
class RegionGrid: public Grid<D, T>
{
public:
  using Base = Grid<D, T>;
  using id_type = typename Base::id_type;
  using index_type = typename Base::index_type;
  using grid_type = RegionGrid<D, R, T>;
  using bounds_type = Bounds<R, D>;
  using vec_type = Vector<R, D>;

  using Base::cellId;
  using Base::cellIndex;
  using Base::operator [];

  /// Constructs a grid with cells of size h covering bounds, so
  /// that every point of bounds maps to a valid cell.
  RegionGrid(const bounds_type& bounds, R h);

  /// Constructs a grid with size cells covering bounds, so that
  /// every point of bounds maps to a valid cell.
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

  /// Returns the region covered by the cells of this grid. It is the
  /// bounds given to the constructor -- every point of which maps to
  /// a valid cell -- with the max corner extended so that the bounds
  /// of every individual cell are fully contained within it.
  ///
  /// @note Cells are half-open, meaning the max faces of bounds() do
  /// not belong to any cell (contains(p) returns false for a point p
  /// on those faces, although bounds().contains(p) returns true).
  [[nodiscard]] const auto& bounds() const
  {
    return _bounds;
  }

  [[nodiscard]] auto cellSize() const
  {
    return _cellSize;
  }

  [[nodiscard]] auto floatIndex(const vec_type& p) const
  {
    return (p - _bounds[0]) * _inverseCellSize;
  }

  /// Bound for converting a float index to id_type.
  static constexpr auto maxIndex = R(max_v<id_type>);

  /// Returns the index of the cell containing p, which must satisfy
  /// contains(p) (asserted in debug). Any point of the bounds given
  /// to the constructor is valid.
  [[nodiscard]] auto cellIndex(const vec_type& p) const
  {
    index_type i{floatIndex(p)};

#ifndef _DEBUG
    for (int k = 0; k < D; ++k)
      assert(i[k] >= 0 && i[k] < this->size()[k]);
#endif // _DEBUG
    return i;
  }

  /// Returns the index of the cell containing p clamped to the grid.
  /// Points outside the bounds map to the nearest border cell.
  [[nodiscard]] auto clampedCellIndex(const vec_type& p) const
  {
    const auto f = floatIndex(p);
    const auto& n = this->size();
    index_type i;

    for (int k = 0; k < D; ++k)
    {
      const auto v = f[k] > 0 ? f[k] : R(0);
      const auto c = v < maxIndex ? id_type(v) : n[k] - 1;

      i[k] = math::min(c, n[k] - 1);
    }
    return i;
  }

  [[nodiscard]] auto cellId(const vec_type& p) const
  {
    return Base::cellId(cellIndex(p));
  }

  [[nodiscard]] auto clampedCellId(const vec_type& p) const
  {
    return Base::cellId(clampedCellIndex(p));
  }

  [[nodiscard]] const auto& operator [](const vec_type& p) const
  {
    return (*this)[cellId(p)];
  }

  [[nodiscard]] auto& operator [](const vec_type& p)
  {
    return (*this)[cellId(p)];
  }

  /// Returns true if p maps to a valid cell, i.e., if cellIndex(p)
  /// can be used. Equivalent to _bounds.min() <= p < _bounds.max()
  /// on every axis, but tested on floatIndex(p) itself, so that it
  /// agrees exactly with index(p) despite rounding.
  [[nodiscard]] bool contains(const vec_type& p) const
  {
    const auto f = floatIndex(p);
    const auto& n = this->size();

    for (int k = 0; k < D; ++k)
      if (!(f[k] >= 0 && f[k] < maxIndex) || id_type(f[k]) >= n[k])
        return false;
    return true;
  }

  [[nodiscard]] bool intersect(const Ray<R, D>& ray, R& tMin, R& tMax) const
  {
    return _bounds.intersect(ray, tMin, tMax);
  }

  [[nodiscard]] auto basePoint(const index_type& index) const
  {
    return _bounds[0] + vec_type{index} * _cellSize;
  }

  [[nodiscard]] auto basePoint(id_type id) const
  {
    return basePoint(Base::cellIndex(id));
  }

  [[nodiscard]] auto bounds(const index_type& index) const
  {
    auto p = basePoint(index);
    return bounds_type{p, p + _cellSize};
  }

  [[nodiscard]] auto bounds(id_type id) const
  {
    return bounds(Base::cellIndex(id));
  }

protected:
  bounds_type _bounds;
  vec_type _cellSize;
  vec_type _inverseCellSize;

private:
  void fitCells();

}; // RegionGrid

namespace internal::rg
{ // begin namespace internal::rg

template <IsReal R, int D>
inline auto
boundsSize(const Bounds<R, D>& bounds)
{
  auto s = bounds.size();

  for (int i = 0; i < D; i++)
    if (!(s[i] > 0)) // also rejects NaN
      throw std::runtime_error("RegionGrid: bad bounds");
  return s;
}

} // end namespace internal::rg

template <int D, IsReal R, typename T>
RegionGrid<D, R, T>::RegionGrid(const bounds_type& bounds, R h):
  _bounds{bounds}
{
  if (!(h > 0)) // also rejects NaN
    throw std::runtime_error("RegionGrid: bad cell size");

  const auto s = internal::rg::boundsSize(_bounds);
  const auto invH = math::inverse(h);
  index_type size;

  for (int i = 0; i < D; ++i)
  {
    const auto f = s[i] * invH;

    if (!(f < maxIndex))
      throw std::runtime_error("RegionGrid: too many cells");
    size[i] = id_type(f) + 1;
  }
  Base::resize(size);
  _inverseCellSize.set(invH);
  _cellSize.set(h);
  fitCells();
}

template <int D, IsReal R, typename T>
RegionGrid<D, R, T>::RegionGrid(const bounds_type& bounds,
  const index_type& size):
  _bounds{bounds}
{
  Base::resize(size);

  auto s = internal::rg::boundsSize(_bounds);

  for (int i = 0; i < D; ++i)
  {
    const auto n = R(size[i]);
    auto d = n / s[i];

    while (!(s[i] * d < n))
      d = std::nextafter(d, R(0));
    _inverseCellSize[i] = d;
  }
  _cellSize = _inverseCellSize.inverse();
  fitCells();
}

template <int D, IsReal R, typename T>
void
RegionGrid<D, R, T>::fitCells()
{
  const auto& n = this->size();
  index_type last;

  for (int i = 0; i < D; ++i)
    last[i] = n[i] - 1;
  _bounds.extend(bounds(last).max());
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

  GridDataBase() = default;

  GridDataBase(const index_type& size)
  {
    resize(size);
  }

  GridDataBase(const GridDataBase&) = delete;
  GridDataBase& operator =(const GridDataBase&) = delete;

  GridDataBase(GridDataBase&& other) noexcept:
    _cells{std::exchange(other._cells, nullptr)},
    _length{std::exchange(other._length, 0)},
    _size{std::exchange(other._size, index_type{0})}
  {
    // do nothing
  }

  ~GridDataBase()
  {
    delete []_cells;
  }

  void resize(const index_type& size);

  [[nodiscard]] const auto& size() const
  {
    return _size;
  }

  [[nodiscard]] auto length() const
  {
    return _length;
  }

  [[nodiscard]] const T* cells() const
  {
    return _cells;
  }

  [[nodiscard]] T* cells()
  {
    return _cells;
  }

  [[nodiscard]] const auto& operator [](id_type id) const
  {
    assert(id >= 0 && id < _length);
    return _cells[id];
  }

  [[nodiscard]] auto& operator [](id_type id)
  {
    assert(id >= 0 && id < _length);
    return _cells[id];
  }

protected:
  T* _cells{};
  id_type _length{};
  index_type _size{};

}; // GridDataBase

template <int D, typename T>
void
GridDataBase<D, T>::resize(const index_type& size)
{
  constexpr auto maxLength = math::min<int64_t>(max_v<id_type>,
    max_v<std::ptrdiff_t> / sizeof(T));
  id_type length{1};

  for (int i = 0; i < D; ++i)
  {
    if (size[i] <= 0)
      throw std::runtime_error("GridData: bad size");
    if (length > id_type(maxLength) / size[i])
      throw std::length_error("GridData: size too large");
    length *= size[i];
  }
  if (length != _length)
  {
    delete []_cells;
    try
    {
      _cells = new T[length];
      _length = length;
    }
    catch (...)
    {
      _cells = nullptr;
      _length = 0;
      _size = index_type{0};
      throw;
    }
  }
  _size = size;
}

} // end namespace cg

#endif // __GridBase_h
