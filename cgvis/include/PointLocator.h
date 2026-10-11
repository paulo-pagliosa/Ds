//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: PointLocator.h
// ========
// Class definition for point locator.
//
// Author: Paulo Pagliosa
// Last revision: 11/10/2022

#ifndef __PointLocator_h
#define __PointLocator_h

#include "geometry/Grid3.h"
#include <algorithm>
#include <vector>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointLocator: point locator class
// ============
template <typename Point, typename real = float>
class PointLocator: public SharedObject
{
public:
  using PointSet = std::vector<Point>;
  using Grid = RegionGrid3<real, int>;
  using id_type = typename Grid::id_type;
  using vec_type = Vector3<real>;

  static constexpr auto eps = (real)1e-5;

  PointLocator(const Bounds3<real>& bounds, id_type n, PointSet& points):
    _grid{bounds, n},
    _points{&points}
  {
    clear();
  }

  void setPointBuffer(PointSet&);

  void clear();

  [[nodiscard]] auto& point(int i) const
  {
    return (*_points)[i];
  }

  /// Returns the id of a point within eps of p, or -1 if none.
  [[nodiscard]] int index(const Point& p) const
  {
    return find(p);
  }

  /// Returns the id of a point within eps of p, adding p if none.
  int add(const Point& p);

private:
  static constexpr int eol = -1;

  Grid _grid;
  PointSet* _points;
  std::vector<int> _next; // next point of the same cell, or eol

  int find(const Point&) const;

}; // PointLocator

template <typename Point, typename real>
inline void
PointLocator<Point, real>::setPointBuffer(PointSet& buffer)
{
  if (&buffer != _points)
  {
    _points = &buffer;
    clear();
  }
}

template <typename Point, typename real>
void
PointLocator<Point, real>::clear()
{
  _points->clear();
  _next.clear();
  std::fill_n(_grid.cells(), _grid.length(), eol);
}

template <typename Point, typename real>
int
PointLocator<Point, real>::find(const Point& p) const
{
  constexpr auto eps2 = eps * eps;
  const vec_type q{p};
  const vec_type e{eps};
  const auto lo = _grid.clampedCellIndex(q - e);
  const auto hi = _grid.clampedCellIndex(q + e);
  const auto& n = _grid.size();
  const auto cells = _grid.cells();
  const auto next = _next.data();
  const auto& points = *_points;

  for (auto z = lo.z; z <= hi.z; ++z)
    for (auto y = lo.y; y <= hi.y; ++y)
    {
      const auto row = (z * n.y + y) * n.x;

      for (auto x = lo.x; x <= hi.x; ++x)
        for (auto id = cells[row + x]; id != eol; id = next[id])
          if ((p - points[id]).squaredNorm() <= eps2)
            return id;
    }
  return eol;
}

template <typename Point, typename real>
int
PointLocator<Point, real>::add(const Point& p)
{
  if (auto id = find(p); id != eol)
    return id;

  auto& head = _grid[_grid.clampedCellId(vec_type{p})];
  const auto id = int(_points->size());

  _next.push_back(head);
  try
  {
    _points->push_back(p);
  }
  catch (...)
  {
    _next.pop_back();
    throw;
  }
  return head = id;
}

} // end namespace cg

#endif // __PointLocator_h
