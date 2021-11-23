//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2020 Orthrus Group.                         |
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
// Last revision: 04/07/2020

#ifndef __Grid2_h
#define __Grid2_h

#include "geometry/GridBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Grid2: generic 2D grid class
// =====
template <typename T>
class GridData<2, T> final: public GridDataBase<2, T>
{
public:
  using GridDataBase<2, T>::GridDataBase;
  using Index2 = Index<2>;

  auto id(const Index2& index) const
  {
    return index.x + index.y * this->_size.x;
  }

  auto index(Index2::base_type id) const
  {
    Index2 i;

    i.y = id / this->_size.x;
    i.x = id - this->_size.x * i.y;
    return i;
  }

}; // GridData

template <typename T>
using Grid2 = Grid<2, T>;


/////////////////////////////////////////////////////////////////////
//
// RegionGrid2: 2D region grid class
// ===========
template <typename real, typename T>
using RegionGrid2 = RegionGrid<2, real, T>;

} // end namespace cg

#endif // __Grid2_h
