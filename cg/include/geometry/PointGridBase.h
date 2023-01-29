//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: PointGridBase.h
// ========
// Class definition for point grid base.
//
// Author: Paulo Pagliosa
// Last revision: 28/01/2023

#ifndef __PointGridBase_h
#define __PointGridBase_h

#include "geometry/GridBase.h"
#include "geometry/IndexList.h"
#include "geometry/KNNHelper.h"
#include "geometry/PointHolder.h"

namespace cg
{ // begin namespace cg

template <int, typename, typename, typename> class PointGridSearcher;


/////////////////////////////////////////////////////////////////////
//
// PointGridBase: point grid base class
// =============
template <int D, typename real, typename PA, typename IL>
class PointGridBase: public RegionGrid<D, real, IL>,
  public PointHolder<D, real, PA>
{
protected:
  ASSERT_INDEX_LIST(IL, "Index list expected");

  using Base = RegionGrid<D, real, IL>;
  using PointSet = PointHolder<D, real, PA>;

  PointGridBase(const Bounds<real, D>& bounds, PA& points, real h):
    Base{bounds, h},
    PointSet{points}
  {
    // do nothing
  }

  PointGridBase(PointGridBase<D, real, PA, IL>&& other):
    Base{std::move(other)},
    PointSet{other.points()}
  {
    // do nothing
  }

}; // PointGridBase


/////////////////////////////////////////////////////////////////////
//
// PointGrid: generic point tree class
// =========
template <int D, typename real, typename PA, typename IL = IndexList<>>
class PointGrid: public PointGridBase<D, real, PA, IL>
{
public:
  using type = PointGrid<D, real, PA, IL>;
  using Base = PointGridBase<D, real, PA, IL>;
  using PointSet = typename Base::PointSet;
  using point_id = typename IL::value_type;
  using pid_list = IndexList<point_id>;
  using vec_type = Vector<real, D>;
  using KNN = KNNHelper<vec_type, point_id>;
  using Searcher = PointGridSearcher<D, real, PA, pid_list>;

  PointGrid(const Bounds<real, D>& bounds, PA& points, real h);

  PointGrid(PA& points, real h, bool squared = true):
    type{PointSet::computeBounds(points, squared), points, h}
  {
    // do nothing
  }

  PointGrid(PointGrid<D, real, PA, IL>&& other):
    Base{std::move(other)}
  {
    // do nothing
  }

  int findNearestNeighbors(const vec_type& point,
    int k,
    point_id indices[],
    real* distances = nullptr,
    typename KNN::Norm norm = KNN::squaredNorm) const;

  size_t findNeighbors(const vec_type& point, pid_list& nids) const
  {
    return Searcher::findNeighbors(*this, point, nids);
  }

  size_t findNeighbors(size_t i, pid_list& nids) const
  {
    assert(i < this->points().size());
    return findNeighbors(vec_type{this->points()[i]}, nids);
  }

  bool addPoint(point_id i)
  {
    assert(i < this->points().size());
    return addPoint(this->points()[i], i);
  }

protected:
  bool addPoint(const vec_type& point, point_id i)
  {
    if (this->bounds().contains(point))
      return (*this)[this->id(point)].add(i);
    return false;
  }

}; // PointGrid

template <int D, typename real, typename PA, typename IL>
PointGrid<D, real, PA, IL>::PointGrid(const Bounds<real, D>& bounds,
  PA& points,
  real h):
  Base{bounds, points, h}
{
  for (point_id n = points.size(), i = 0; i < n; ++i)
    if (this->activePoint(i))
      addPoint(points[i], i);
}

template <int D, typename real, typename PA, typename IL>
int
PointGrid<D, real, PA, IL>::findNearestNeighbors(const vec_type& p,
  int k,
  point_id indices[],
  real* distances,
  typename KNN::Norm norm) const
{
  /*
  if (!this->bounds().contains(p))
    return 0;
  */

  KNN knn{p, k, norm};
  const auto& points = this->points();
  auto n = points.size;

  if (n <= k)
    for (decltype(n) i = 0; i < n; ++i)
      knn.test(points[i], i);
  else
  {
    // TODO
  }
  return knn.results(indices, distances);
}

} // namespace cg

#endif // __PointGridBase_h
