//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019 Orthrus Group.                               |
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
// OVERVIEW: PointHolder.h
// ========
// Class definition for point holder.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2019

#ifndef __PointHolder_h
#define __PointHolder_h

#include "geometry/Bounds3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointHolder: point holder class
// ===========
template <typename PointArray>
class PointHolder
{
public:
  const auto& points() const
  {
    return _points;
  }

protected:
  const PointArray& _points;

  PointHolder(const PointArray& points):
    _points(points)
  {
    // do nothing
  }

  template <size_t D, typename real>
  static auto computeBounds(const PointArray& points, bool squared)
  {
    using size_type = decltype(points.size());

    Bounds<real, D> bounds;

    for (size_type n = points.size(), i = 0; i < n; ++i)
      bounds.inflate(points[i]);
    if (squared)
    {
      auto s = Vector<real, D>{bounds.maxSize() * real(0.5)};
      auto c = bounds.center();

      bounds.inflate(c - s);
      bounds.inflate(c + s);
    }
    return bounds;
  }

}; // PointHolder

} // namespace cg

#endif // __PointHolder_h

