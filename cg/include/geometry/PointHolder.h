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
// OVERVIEW: PointHolder.h
// ========
// Class definition for point holder.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __PointHolder_h
#define __PointHolder_h

#include "geometry/Bounds3.h"

namespace cg
{ // begin namespace cg

template <typename index_t, typename PA>
[[nodiscard]] constexpr auto
activePointFlag(const PA&, index_t)
{
  return true;
}


/////////////////////////////////////////////////////////////////////
//
// PointHolder: point holder class
// ===========
template <int D, IsReal R, typename PA>
class PointHolder
{
private:
  PA* _points;

public:
  using Bounds = cg::Bounds<R, D>;

  template <typename index_t>
  [[nodiscard]] auto activePoint(index_t index) const
  {
    return activePointFlag(*_points, index);
  }

  [[nodiscard]] const auto& points() const
  {
    return *_points;
  }

  [[nodiscard]] auto& points()
  {
    return *_points;
  }

  [[nodiscard]] static Bounds computeBounds(const PA&, bool = false);

protected:
  PointHolder(PA& points):
    _points{&points}
  {
    // do nothing
  }

  template <typename P>
  void setPositions(const P& points)
  {
    const auto n = _points->size();

    assert(n == points.size());
    for (decltype(n) i = 0; i < n; ++i)
      (*_points)[i].set(points[i]);
  }

  void setPoints(PA& points)
  {
    _points = &points;
  }

}; // PointHolder

template <int D, IsReal R, typename PA>
auto
PointHolder<D, R, PA>::computeBounds(const PA& points, bool squared) -> Bounds
{
  using psize_t = decltype(points.size());

  Bounds bounds;

  for (psize_t n = points.size(), i = 0; i < n; ++i)
    bounds.extend(points[i]);
  if (squared)
  {
    auto s = Vector<R, D>{bounds.maxExtent() * R(0.5)};
    auto c = bounds.center();

    bounds.extend(c - s);
    bounds.extend(c + s);
  }
  return bounds;
}

} // namespace cg

#endif // __PointHolder_h
