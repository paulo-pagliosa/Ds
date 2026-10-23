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
// OVERVIEW: Grid2.h
// ========
// Class definition for 2D grid.
//
// Author: Paulo Pagliosa
// Last revision: 23/10/2026

#ifndef __Grid2_h
#define __Grid2_h

#include "geometry/GridBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GridShape2: 2D grid shape class
// ==========
template <>
class GridShape<2>
{
public:
  using id_type = int32_t;
  using index_type = Index<2, id_type>;

  GridShape() = default;

  /// Constructs the shape of a grid with size cells per axis.
  HOST DEVICE
  explicit GridShape(const index_type& size):
    _size{size},
    _length{size.x * size.y}
  {
    // do nothing
  }

  [[nodiscard]] HOST DEVICE
  const auto& size() const
  {
    return _size;
  }

  [[nodiscard]] HOST DEVICE
  auto length() const
  {
    return _length;
  }

  [[nodiscard]] HOST DEVICE
  id_type cellId(const index_type& index) const
  {
    return index.x + index.y * _size.x;
  }

  [[nodiscard]] HOST DEVICE
  index_type cellIndex(id_type id) const
  {
    index_type i;

    i.y = id / _size.x;
    i.x = id - _size.x * i.y;
    return i;
  }

private:
  index_type _size{0};
  id_type _length{};

}; // GridShape


/////////////////////////////////////////////////////////////////////
//
// Grid2: generic 2D grid class
// =====
template <typename T, typename Allocator = ArrayAllocator>
using Grid2 = Grid<2, T, Allocator>;


/////////////////////////////////////////////////////////////////////
//
// RegionGrid2: 2D region grid class
// ===========
template <IsReal R, typename T, typename Allocator = ArrayAllocator>
using RegionGrid2 = RegionGrid<2, R, T, Allocator>;

} // end namespace cg

#endif // __Grid2_h
