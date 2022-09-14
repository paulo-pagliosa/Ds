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
// OVERVIEW: PointLocator.h
// ========
// Class definition for point locator.
//
// Author: Paulo Pagliosa
// Last revision: 14/09/2022

#ifndef __PointLocator_h
#define __PointLocator_h

#include "geometry/Grid3.h"
#include "geometry/IndexList.h"
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
  using Grid = RegionGrid3<real, IndexList<>>;
  using id_type = typename Grid::id_type;

  static constexpr auto eps = (real)1e-5;

  PointLocator(const Bounds3<real>& bounds, id_type n, PointSet& points):
    _grid{bounds, n},
    _points{&points}
  {
    points.clear();
  }

  void setPointBuffer(PointSet&);

  void clear();

  auto& point(int i) const
  {
    return (*_points)[i];
  }

  auto index(const Point& p) const
  {
    return const_cast<PointLocator<Point, real>*>(this)->test(p);
  }

  int add(const Point& p)
  {
    return test(p, true);
  }

private:
  Grid _grid;
  PointSet* _points;

  int test(const Point&, bool = false);
  int test(id_type, const Point&, bool = false);

}; // PointLocator

template <typename Point, typename real>
void
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
  for (auto e = _grid.end(), i = _grid.begin(); i != e;)
    i++->clear();
}

namespace internal
{ // begin namespace internal

template <typename Point, typename real>
inline bool
gt(const Point& p, real a)
{
  return p.x > a && p.y > a && p.z > a;
}

template <typename Point, typename real>
inline bool
lt(const Point& p, real a)
{
  return p.x < a&& p.y < a&& p.z < a;
}

} // end namespace internal

template <typename Point, typename real>
int
PointLocator<Point, real>::test(const Point& p, bool insert)
{
  using vec3 = Vector3<real>;
  using index_type = Index3<id_type>;

  const auto o = _grid.floatIndex(p);
  const auto s = index_type{o};
  const vec3 d{real(o.x - s.x), real(o.y - s.y), real(o.z - s.z)};

  if (internal::gt(d, eps) && internal::lt(d, 1 - eps))
    return test(_grid.id(s), p, insert);

  const auto& n = _grid.size();

  for (id_type k = -1; k <= 1; k++)
  {
    auto z = s.z + k;

    if (z < 0 || z >= n.z)
      continue;
    for (id_type j = -1; j <= 1; j++)
    {
      auto y = s.y + j;

      if (y < 0 || y >= n.y)
        continue;
      for (id_type i = -1; i <= 1; i++)
      {
        auto x = s.x + i;

        if (x < 0 || x >= n.x)
          continue;

        auto id = test(_grid.id(index_type{x, y, z}), p, insert);

        if (id != -1)
          return id;
      }
    }
  }
  return -1;
}

template <typename Point, typename real>
int
PointLocator<Point, real>::test(id_type cid, const Point& p, bool insert)
{
  /*
  if (cid >= _grid.length() || cid < 0)
  {
    if (!insert)
      return -1;
    _points->push_back(p);
    return (int)_points->size() - 1;
  }
  */

  auto& list = _grid[cid];

  for (auto id : list)
    if ((p - point(id)).squaredNorm() <= (eps * eps))
      return id;
  if (!insert)
    return -1;
  _points->push_back(p);

  auto id = (int)_points->size() - 1;

  list.add(id);
  return id;
}

} // end namespace cg

#endif // __PointLocator_h
