//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: PointGrid2.h
// ========
// Class definition for generic 2D point grid.
//
// Author: Paulo Pagliosa
// Last revision: 12/09/2022

#ifndef __PointGrid2_h
#define __PointGrid2_h

#include "geometry/Grid2.h"
#include "geometry/PointGridBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointGrid2: generic 2D point grid class
// ==========
template <typename real, typename PA, typename IL>
class PointGridSearcher<2, real, PA, IL>
{
public:
  using Grid = PointGrid<2, real, PA, IL>;
  using vec_type = typename Grid::vec_type;
  using pid_list = typename Grid::pid_list;

  static size_t findNeighbors(const Grid& grid,
    const vec_type& point,
    pid_list& nids);

}; // PointGridSearcher

template <typename real, typename PA, typename IL>
size_t
PointGridSearcher<2, real, PA, IL>::findNeighbors(const Grid& grid,
  const vec_type& point,
  pid_list& nids)
{
  auto s = grid.index(point);
  const auto& n = grid.size();
  auto h = math::sqr(grid.cellSize().min());
  Index<2> c;

  nids.clear();
  for (int j = -1; j <= 1; j++)
  {
    c.y = s.y + j;
    if (c.y < 0 || c.y >= n.y)
      continue;
    for (int i = -1; i <= 1; i++)
    {
      c.x = s.x + i;
      if (c.x < 0 || c.x >= n.x)
        continue;
      for (auto id : grid[c])
      {
        auto d2 = (point - grid.points()[id]).squaredNorm();

        if (d2 != 0 && d2 <= h)
          nids.add(id);
      }
    }
  }
  return nids.size();
}

template <typename real, typename PA, typename IL = IndexList<>>
using PointGrid2 = PointGrid<2, real, PA, IL>;

} // namespace cg

#endif // __PointGrid2_h
