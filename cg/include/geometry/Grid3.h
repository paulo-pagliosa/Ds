//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Grid3.h
// ========
// Class definition for 3D grid.
//
// Author: Paulo Pagliosa
// Last revision: 16/12/2022

#ifndef __Grid3_h
#define __Grid3_h

#include "geometry/GridBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Grid3: generic 3D grid class
// =====
template <typename T>
class GridData<3, T> final: public GridDataBase<3, T>
{
public:
  using Base = GridDataBase<3, T>;
  using id_type = typename Base::id_type;
  using index_type = typename Base::index_type;

  GridData() = default;

  GridData(const index_type& size)
  {
    resize(size);
  }

  GridData(GridData<3, T>&& other):
    Base{std::move(other)}
  {
    _size_xy = other._size_xy;
  }

  void resize(const index_type& size)
  {
    Base::resize(size);
    _size_xy = size.x * size.y;
  }

  auto id(const index_type& index) const
  {
    return index.x + index.y * this->_size.x + index.z * _size_xy;
  }

  auto index(id_type id) const
  {
    index_type i;

    i.z = id / _size_xy;
    id -= _size_xy * i.z;
    i.y = id / this->_size.x;
    i.x = id - this->_size.x * i.y;
    return i;
  }

private:
  id_type _size_xy{};

}; // GridData

template <typename T>
using Grid3 = Grid<3, T>;


/////////////////////////////////////////////////////////////////////
//
// RegionGrid3: 3D region grid class
// ===========
template <typename real, typename T>
using RegionGrid3 = RegionGrid<3, real, T>;

} // end namespace cg

#endif // __Grid3_h
