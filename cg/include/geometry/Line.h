//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2026 Paulo Pagliosa.                        |
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
//  OVERVIEW: Line.h
//  ========
//  Class definition for 2D/3D line.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Line_h
#define __Line_h

#include "geometry/Bounds3.h"
#include "geometry/Point3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Line: 2D/3D line class
// ====
template <int D, IsReal R>
class Line
{
public:
  using Point = cg::Point<R, D>;

  Point p0;
  Point p1;

  auto operator ()(R u) const
  {
    return R(1 - u) * p0 + u * p1;
  }

  auto bounds() const
  {
    return Bounds<R, D>{p0, p1};
  }

  auto direction() const
  {
    return (p1 - p0).versor();
  }

  auto length() const
  {
    return (p1 - p0).length();
  }

}; // Line

template <IsReal R> using Line2 = Line<2, R>;
template <IsReal R> using Line3 = Line<3, R>;

using Line2f = Line2<float>;
using Line2d = Line2<double>;
using Line3f = Line3<float>;
using Line3d = Line3<double>;

} // end namespace cg

#endif // __Line_h
