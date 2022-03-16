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
// OVERVIEW: TriCell.h
// ========
// Class definition for vis linear triangular cell.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __TriCell_h
#define __TriCell_h

#include "Cell.h"

namespace cg::vis
{ // begin namespace cg::vis

//
// Forward definitions
//
class PolyData;
class Scalars;

extern void contourTriangle(const vec3f* vertices,
  const float* scalars,
  float value,
  PolyData& output);


/////////////////////////////////////////////////////////////////////
//
// TriCell: vis linear triangular cell class
// =======
template <typename DataSet>
class TriCell: public Cell<DataSet, 3>
{
public:
  using Base = Cell<DataSet, 3>;

  TriCell(const DataSet& dataSet, int vid0, int vid1, int vid2):
    Base{dataSet, vid0, vid1, vid2}
  {
    // do nothing
  }

  void contour(float value, Scalars* scalars, PolyData& output) const
  {
    const vec3f vertices[]
    {
      this->vertex(0),
      this->vertex(1),
      this->vertex(2)
    };
    const float cellScalars[]
    {
      scalars->get(this->vertexId(0)),
      scalars->get(this->vertexId(1)),
      scalars->get(this->vertexId(2)),
    };

    contourTriangle(vertices, cellScalars, value, output);
  }

}; // TriCell

} // end namespace cg::vis

#endif // __TriCell_h
