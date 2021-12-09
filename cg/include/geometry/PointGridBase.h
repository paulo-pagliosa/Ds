//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2021 Orthrus Group.                         |
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
// Last revision: 08/12/2021

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
class PointGridBase: public RegionGrid<D, real, IL>, public PointHolder<PA>
{
protected:
  ASSERT_INDEX_LIST(IL, "IndexList expected");

  using Base = RegionGrid<D, real, IL>;
  using PointSet = PointHolder<PA>;

  PointGridBase(const PA& points, real h, bool squared = false):
    Base{PointSet::computeBounds<D, real>(points, squared), h},
    PointSet(points)
  {
    // do nothing
  }

  template <typename P>
  PointGridBase(PointGridBase<D, real, P, IL>&& other, const PA& points):
    Base{std::move(other)},
    PointSet(points)
  {
    if (points.size() != other.points().size())
      throw std::logic_error("PointGridBase(): bad points");
  }

}; // PointGridBase


/////////////////////////////////////////////////////////////////////
//
// PointGrid: generic point tree class
// =========
template <int D, typename real, typename PA, typename IL = IndexList>
class PointGrid: public PointGridBase<D, real, PA, IL>
{
public:
  using type = PointGrid<D, real, PA, IL>;
  using Base = PointGridBase<D, real, PA, IL>;
  using vec_type = Vector<real, D>;
  using KNN = KNNHelper<vec_type>;
  using Searcher = PointGridSearcher<D, real, PA, IL>;

  PointGrid(const PA& points, real h, bool squared = true);

  template <typename P>
  PointGrid(PointGrid<D, real, P, IL>&& other, const PA& points):
    Base{std::move(other), points}
  {
    // do nothing
  }

  int findNearestNeighbors(const vec_type& point,
    int k,
    int indices[],
    real* distances = nullptr,
    typename KNN::Norm norm = KNN::squaredNorm) const;

  size_t findNeighbors(const vec_type& point, IndexList& nids) const
  {
    return Searcher::findNeighbors(*this, point, nids);
  }

  size_t findNeighbors(size_t i, IndexList& nids) const
  {
    assert(i < this->_points.size());
    return findNeighbors(vec_type{this->_points[i]}, nids);
  }

  template <typename I>
  void addPoint(I i)
  {
    assert(i < this->_points.size());
    addPoint(this->_points[i], (int)i);
  }

protected:
  void addPoint(const vec_type& point, int i)
  {
    (*this)[this->id(point)].add(i);
  }

}; // PointGrid

template <int D, typename real, typename PA, typename IL>
PointGrid<D, real, PA, IL>::PointGrid(const PA& points, real h, bool squared):
  Base{points, h, squared}
{
  using index_type = decltype(points.size());

  for (index_type n = points.size(), i = 0; i < n; ++i)
    addPoint(points[i], (int)i);
}

template <int D, typename real, typename PA, typename IL>
int
PointGrid<D, real, PA, IL>::findNearestNeighbors(const vec_type& p,
  int k,
  int indices[],
  real* distances,
  typename KNN::Norm norm) const
{
  /*
  if (!this->bounds().contains(p))
    return 0;
  */

  KNN knn{p, k, norm};
  auto n = this->_points.size();

  if (n <= k)
    for (int i = 0; i < n; ++i)
      knn.test(this->_points[i], i);
  else
  {
    // TODO
  }
  return knn.results(indices, distances);
}

} // namespace cg

#endif // __PointGridBase_h
