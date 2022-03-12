//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Cell.h
// ========
// Class definition for generic vis cell.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#ifndef __Cell_h
#define __Cell_h

#include "core/SharedObject.h"
#include <cstdint>

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Cell: generic vis cell class
// ====
template <typename DataSet, int32_t nv>
class Cell: public virtual SharedObject
{
public:
  constexpr auto vertexCount() const
  {
    return nv;
  }

  auto vertexId(int i) const
  {
    return _vertexIds[i];
  }

  auto vertex(int i) const
  {
    return _dataSet->vertex(vertexId(i));
  }

protected:
  Reference<DataSet> _dataSet;
  int _vertexIds[nv];

  template <typename... T>
  Cell(const DataSet& dataSet, T... vertexIds):
    _dataSet{&dataSet},
    _vertexIds{vertexIds...}
  {
    // do nothing
  }

}; // Cell

} // end namespace cg::vis

#endif // __Cell_h
