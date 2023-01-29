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
// OVERVIEW: PointTreeBase.h
// ========
// Class definition for point quadtree/octree base.
//
// Author: Paulo Pagliosa
// Last revision: 28/01/2023

#ifndef __PointTreeBase_h
#define __PointTreeBase_h

#include "geometry/IndexList.h"
#include "geometry/KNNHelper.h"
#include "geometry/PointHolder.h"
#include "geometry/TreeBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointTreeBase: point tree base class
// =============
template <int D, typename real, typename PA, typename IL>
class PointTreeBase: public RegionTree<D, real, IL>,
  public PointHolder<D, real, PA>
{
protected:
  ASSERT_INDEX_LIST(IL, "Index list expected");

  using Base = RegionTree<D, real, IL>;
  using PointSet = PointHolder<D, real, PA>;

  PointTreeBase(const Bounds<real, D>& bounds,
    PA& points,
    uint32_t maxDepth = 20):
    Base{bounds, maxDepth},
    PointSet{points}
  {
    // do nothing
  }

  PointTreeBase(PointTreeBase<D, real, PA, IL>&& other):
    Base{std::move(other)},
    PointSet{other.points()}
  {
    // do nothing
  }

}; // PointTreeBase


/////////////////////////////////////////////////////////////////////
//
// PointTree: generic point tree class
// =========
template <int D, typename real, typename PA, typename IL = IndexList<>>
class PointTree: public PointTreeBase<D, real, PA, IL>
{
public:
  using Base = PointTreeBase<D, real, PA, IL>;
  using PointSet = typename Base::PointSet;
  using point_id = typename IL::value_type;
  using pid_list = IndexList<point_id>;
  using vec_type = Vector<real, D>;
  using key_type = TreeKey<D>;
  using bounds_type = Bounds<real, D>;
  using KNN = KNNHelper<vec_type, point_id>;
  using tree_type = PointTree<D, real, PA, IL>;
  using leaf_iterator = typename Base::leaf_iterator;

  using SplitTest = std::function<bool(const PA&, IL&, uint32_t)>;

  PointTree(const bounds_type& bounds,
    PA& points,
    SplitTest splitTest,
    uint32_t maxDepth = 20,
    bool fullTree = false);

  PointTree(const bounds_type& bounds,
    PA& points,
    uint32_t splitThreshold = 20,
    uint32_t maxDepth = 20,
    bool fullTree = false):
    tree_type{bounds,
      points,
      defaultSplitTest(splitThreshold),
      maxDepth,
      fullTree}
  {
    // do nothing
  }

  PointTree(PA& points,
    SplitTest splitTest,
    uint32_t maxDepth = 20,
    bool fullTree = false,
    bool squared = true):
    tree_type{PointSet::computeBounds(points, squared),
      points,
      splitTest,
      maxDepth,
      fullTree}
  {
    // do nothing
  }

  PointTree(PA& points,
    uint32_t splitThreshold = 20,
    uint32_t maxDepth = 20,
    bool fullTree = false,
    bool squared = true):
    tree_type{points,
      defaultSplitTest(splitThreshold),
      maxDepth,
      fullTree,
      squared}
  {
    // do nothing
  }

  PointTree(PointTree<D, real, PA, IL>&& other):
    Base{std::move(other)},
    _splitTest{other._splitTest}
  {
    // do nothing
  }

  void rebuild(bool clear, bool fullTree = false)
  {
    if (clear)
      this->clear();
    build(fullTree);
  }

  int findNearestNeighbors(const vec_type& point,
    int k,
    point_id indices[],
    real* distances = nullptr,
    typename KNN::Norm norm = KNN::squaredNorm) const;

  size_t findNeighbors(const vec_type& point,
    real radius,
    pid_list& list) const;

  size_t findNeighbors(int i, real radius, pid_list& list) const
  {
    return findNeighbors(vec_type{this->points()[i]}, radius, list);
  }

protected:
  using BranchNode = typename Base::BranchNode;
  using LeafNode = typename Base::LeafNode;

  SplitTest _splitTest;

  bool addPoint(const vec_type& point, point_id i)
  {
    if (this->bounds().contains(point))
      return this->makeLeaf(this->key(point))->data().add(i);
    return false;
  }

  void movePoint(const vec_type& point,
    point_id i,
    uint64_t mask,
    BranchNode* branch)
  {
    this->makeLeaf(this->key(point), mask, branch)->data().add(i);
  }

  void removePoints();

  bool splitChildren(BranchNode* branch, bool fullTree);
  bool split(LeafNode* leaf, bool fullTree);

  void moveDataToChildren(LeafNode* leaf,
    BranchNode* branch,
    const key_type& key) override;

  void radiusSearch(const vec_type& point,
    real radius,
    const key_type& key,
    BranchNode* branch,
    pid_list& list) const;

  real knnSearch(KNN& knn,
    real r2,
    const key_type& key,
    BranchNode* branch) const;

private:
  static SplitTest defaultSplitTest(uint32_t splitThreshold)
  {
    return [=](const PA&, IL& list, uint32_t) -> bool
    {
      return list.size() > splitThreshold;
    };
  }

  void build(bool = false);

}; // PointTree

template <int D, typename real, typename PA, typename IL>
PointTree<D, real, PA, IL>::PointTree(const bounds_type& bounds,
  PA& points,
  SplitTest splitTest,
  uint32_t maxDepth,
  bool fullTree):
  Base{bounds, points, maxDepth},
  _splitTest{splitTest}
{
  build(fullTree);
}

template <int D, typename real, typename PA, typename IL>
void
PointTree<D, real, PA, IL>::build(bool fullTree)
{
  const auto& points = this->points();

  for (point_id n = points.size(), i = 0; i < n; ++i)
    if (this->activePoint(i))
      addPoint(points[i], i);
  if (_splitTest != nullptr)
    splitChildren(this->root(), fullTree);
}

template <int D, typename real, typename PA, typename IL>
bool
PointTree<D, real, PA, IL>::splitChildren(BranchNode* branch, bool fullTree)
{
  if (branch->depth() + 1 == this->_maxDepth)
    return false;

  constexpr auto N = (int)ipow2<D>();
  auto s = false;

  for (int i = 0; i < N; i++)
  {
    auto child = branch->child(i);

    if (child == nullptr)
    {
      if (fullTree)
        child = this->createLeafChild(branch, i);
      else
        continue;
    }
    s |= child->isLeaf() ? split((LeafNode*)child, fullTree) :
      splitChildren((BranchNode*)child, fullTree);
  }
  return s;
}

template <int D, typename real, typename PA, typename IL>
bool
PointTree<D, real, PA, IL>::split(LeafNode* leaf, bool fullTree)
{
  const auto& points = this->points();

  if (!_splitTest(points, leaf->data(), leaf->depth()))
    return false;

  auto branch = this->createBranchInPlaceOf(leaf);
  uint64_t mask{this->_depthMask >> branch->depth()};

  for (auto index : leaf->data())
    movePoint(points[index], index, mask, branch);
  this->deleteLeaf(leaf);
  return splitChildren(branch, fullTree);
}

template <int D, typename real, typename PA, typename IL>
void
PointTree<D, real, PA, IL>::moveDataToChildren(LeafNode* leaf,
  BranchNode* branch,
  const key_type& key)
{
  (void)key;

  const auto& points = this->points();
  uint64_t mask{this->_depthMask >> branch->depth()};

  for (auto index : leaf->data())
  {
    auto i = this->key(points[index]).childIndex(mask);
    auto child = (LeafNode *)branch->child(i);

    child->data().add(index);
  }
}

template <int D, typename real, typename PA, typename IL>
void
PointTree<D, real, PA, IL>::removePoints()
{
  auto& points = this->points();

  for (auto cell = this->leafBegin(); !cell.isNull(); ++cell)
    for (auto& pids = cell.data(); !pids.empty();)
    {
      auto pit = pids.begin();
      auto pid = *pit;

      pids.remove(pit);
      points.remove(pid);
    }
}

namespace internal::pt
{ // begin namespace internal::pt

template <typename real>
inline auto
searchSize2(real d2, real r2)
{
  return d2 * real(0.25) + r2 + sqrt(d2 * r2);
}

template <typename real>
inline constexpr auto
eps(real x)
{
  return std::numeric_limits<real>::epsilon() + x;
}

} // end namespace internal::pt

template <int D, typename real, typename PA, typename IL>
size_t
PointTree<D, real, PA, IL>::findNeighbors(const vec_type& p,
  real radius,
  pid_list& list) const
{
  if (/*!this->bounds().contains(p) || */radius <= 0)
    return 0;
  list.clear();
  radiusSearch(p, radius * radius, key_type{0LL}, this->root(), list);
  return list.size();
}

template <int D, typename real, typename PA, typename IL>
void
PointTree<D, real, PA, IL>::radiusSearch(const vec_type& p,
  real r2,
  const key_type& key,
  BranchNode* branch,
  pid_list& list) const
{
  constexpr auto N = (int)ipow2<D>();
  auto depth = branch->depth() + 1;
  auto s2 = internal::pt::searchSize2(this->nodeSize(depth).squaredNorm(), r2);
  const auto& points = this->points();

  for (int i = 0; i < N; i++)
    if (auto child = branch->child(i))
    {
      auto childKey = key_type(key).pushChild(i);
      auto d2 = (p - this->center(childKey, depth)).squaredNorm();

      if (internal::pt::eps(d2) > s2)
        continue;
      if (!child->isLeaf())
        radiusSearch(p, r2, childKey, (BranchNode*)child, list);
      else
        for (auto index : ((LeafNode*)child)->data())
          if ((p - points[index]).squaredNorm() <= r2)
            list.add(index);
    }
}

template <int D, typename real, typename PA, typename IL>
int
PointTree<D, real, PA, IL>::findNearestNeighbors(const vec_type& p,
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
  auto n = points.size();

  if (n <= k)
    for (decltype(n) i = 0; i < n; ++i)
      knn.test(points[i], i);
  else
    knnSearch(knn, math::Limits<real>::inf(), key_type{0LL}, this->root());
  return knn.results(indices, distances);
}

template <int D, typename real, typename PA, typename IL>
real
PointTree<D, real, PA, IL>::knnSearch(KNN& knn,
  real r2,
  const key_type& key,
  BranchNode* branch) const
{
  struct NodeEntry
  {
    TreeNodeBase<D>* node;
    key_type key;

    NodeEntry() = default;

    NodeEntry(TreeNodeBase<D>* node, const key_type& key):
      node{node}, key{key}
    {
      // do nothing
    }

  }; // NodeEntry

  using Queue = typename KNN::template Queue<NodeEntry>;
  constexpr auto N = (int)ipow2<D>();

  Queue nq{N};
  auto depth = branch->depth() + 1;
  auto n2 = this->nodeSize(depth).squaredNorm();

  for (int i = 0; i < N; i++)
    if (auto child = branch->child(i))
    {
      auto childKey = key_type(key).pushChild(i);
      auto d2 = (knn.sample() - this->center(childKey, depth)).squaredNorm();

      nq.insert(d2, NodeEntry{child, childKey});
    }

  const auto& points = this->points();

  for (auto n = nq.size(), i = 0;
    i < n &&
    internal::pt::eps(nq.key(i)) <= internal::pt::searchSize2(n2, r2);
    ++i)
  {
    const auto& e = nq.value(i);

    if (!e.node->isLeaf())
      r2 = knnSearch(knn, r2, e.key, (BranchNode*)e.node);
    else
      for (auto index : ((LeafNode*)e.node)->data())
        knn.test(points[index], index, r2);
  }
  return r2;
}

} // namespace cg

#endif // __PointTreeBase_h
