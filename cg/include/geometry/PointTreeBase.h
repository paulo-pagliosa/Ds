//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2019 Orthrus Group.                         |
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
// Last revision: 28/03/2019

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
template <size_t D, typename real, typename PointArray>
class PointTreeBase: public RegionTree<D, real, IndexList>,
  public PointHolder<PointArray>
{
protected:
  using Base = RegionTree<D, real, IndexList>;
  using PointSet = PointHolder<PointArray>;

  PointTreeBase(const PointArray& points,
    uint32_t maxDepth = 20,
    bool squared = false):
    Base{PointSet::computeBounds<D, real>(points, squared), maxDepth},
    PointSet(points)
  {
    // do nothing
  }

  template <typename P>
  PointTreeBase(PointTreeBase<D, real, P>&& other, const PointArray& points):
    Base{std::move(other)},
    PointSet(points)
  {
    if (points.size() != other.points().size())
      throw std::logic_error("PointTreeBase(): bad points");
  }

}; // PointTreeBase


/////////////////////////////////////////////////////////////////////
//
// PointTree: generic point tree class
// =========
template <size_t D, typename real, typename PointArray>
class PointTree: public PointTreeBase<D, real, PointArray>
{
public:
  using type = PointTree<D, real, PointArray>;
  using Base = PointTreeBase<D, real, PointArray>;
  using key_type = TreeKey<D>;
  using vec_type = Vector<real, D>;
  using KNN = KNNHelper<vec_type>;

  using SplitTest = std::function<bool(const PointArray&, IndexList&, int)>;

  PointTree(const PointArray& points,
    SplitTest spliTest,
    uint32_t maxDepth = 20,
    bool squared = true);

  PointTree(const PointArray& points,
    uint32_t splitThreshold = 20,
    uint32_t maxDepth = 20,
    bool squared = true):
    type{points, defaultSplitTest(splitThreshold), maxDepth, squared}
  {
    // do nothing
  }

  template <typename P>
  PointTree(PointTree<D, real, P>&& other, const PointArray& points):
    Base{std::move(other), points},
    _splitTest{other._splitTest}
  {
    // do nothing
  }

  void rebuild()
  {
    this->clear();
    build();
  }

  int findNearestNeighbors(const vec_type& point,
    int k,
    int indices[],
    real* distances = nullptr,
    typename KNN::Norm norm = KNN::squaredNorm) const;

  size_t findNeighbors(const vec_type& point, real radius, IndexList& list) const;

  size_t findNeighbors(int i, real radius, IndexList& list) const
  {
    return findNeighbors(vec_type{this->_points[i]}, radius, list);
  }

protected:
  using BranchNode = typename Base::BranchNode;
  using LeafNode = typename Base::LeafNode;

  void addPoint(const vec_type& point, int i)
  {
    this->makeLeaf(this->key(point))->data().add(i);
  }

  void addPoint(const vec_type& point,
    int i,
    uint64_t mask,
    BranchNode* branch)
  {
    this->makeLeaf(this->key(point), mask, branch)->data().add(i);
  }

  bool splitChildren(BranchNode* branch);
  bool split(LeafNode* leaf);

  void moveDataToChildren(LeafNode* leaf,
    BranchNode* branch,
    const key_type& key) override;

  void radiusSearch(const vec_type& point,
    real radius,
    const key_type& key,
    BranchNode* branch,
    IndexList& list) const;

  real knnSearch(KNN& knn,
    real r2,
    const key_type& key,
    BranchNode* branch) const;

private:
  SplitTest _splitTest;

  static SplitTest defaultSplitTest(uint32_t splitThreshold)
  {
    return [=](const PointArray&, IndexList& list, int) -> bool
    {
      return list.size() > splitThreshold;
    };
  }

  void build();

}; // PointTree

template <size_t D, typename real, typename PointArray>
PointTree<D, real, PointArray>::PointTree(const PointArray& points,
  SplitTest splitTest,
  uint32_t maxDepth, bool squared):
  Base{points, maxDepth, squared},
  _splitTest{splitTest}
{
  build();
}

template <size_t D, typename real, typename PointArray>
void
PointTree<D, real, PointArray>::build()
{
  using index_type = decltype(this->_points.size());

  for (index_type n = this->_points.size(), i = 0; i < n; ++i)
    addPoint(this->_points[i], int(i));
  if (_splitTest != nullptr)
    splitChildren(this->root());
}

template <size_t D, typename real, typename PointArray>
bool
PointTree<D, real, PointArray>::splitChildren(BranchNode* branch)
{
  if (branch->depth() + 1 == this->_maxDepth)
    return false;

  constexpr auto N = (int)ipow2<D>();
  auto s = false;

  for (int i = 0; i < N; i++)
    if (auto child = branch->child(i))
      s |= split((LeafNode *)child);
  return s;
}

template <size_t D, typename real, typename PointArray>
bool
PointTree<D, real, PointArray>::split(LeafNode *leaf)
{
  if (!_splitTest(this->_points, leaf->data(), leaf->depth()))
    return false;

  auto branch = this->createBranchInPlaceOf(leaf);
  uint64_t mask{this->_depthMask >> branch->depth()};

  for (auto index : leaf->data())
    addPoint(this->_points[index], index, mask, branch);
  this->deleteLeaf(leaf);
  return splitChildren(branch);
}

template <size_t D, typename real, typename PointArray>
void
PointTree<D, real, PointArray>::moveDataToChildren(LeafNode*leaf,
  BranchNode* branch,
  const key_type& key)
{
  (void)key;

  uint64_t mask{this->_depthMask >> branch->depth()};

  for (auto index : leaf->data())
  {
    auto i = this->key(this->_points[index]).childIndex(mask);
    auto child = (LeafNode *)branch->child(i);

    child->data().add(index);
  }
}

template <size_t D, typename real, typename PointArray>
size_t
PointTree<D, real, PointArray>::findNeighbors(const vec_type& p,
  real radius,
  IndexList& list) const
{
  if (/*!this->bounds().contains(p) || */ radius <= 0)
    return 0;
  list.clear();
  radiusSearch(p, radius * radius, key_type{0LL}, this->root(), list);
  return list.size();
}

template <size_t D, typename real, typename PointArray>
void
PointTree<D, real, PointArray>::radiusSearch(const vec_type& p,
  real r2,
  const key_type& key,
  BranchNode* branch,
  IndexList& list) const
{
  auto depth = branch->depth() + 1;
  auto s2 = searchSize2(this->nodeSize(depth).squaredNorm(), r2);
  constexpr auto N = (int)ipow2<D>();

  for (int i = 0; i < N; i++)
    if (auto child = branch->child(i))
    {
      auto childKey = key_type(key).pushChild(i);
      auto d2 = (p - this->center(childKey, depth)).squaredNorm();

      if (epsilon<real>() + d2 > s2)
        continue;
      if (!child->isLeaf())
        radiusSearch(p, r2, childKey, (BranchNode*)child, list);
      else
        for (auto index : ((LeafNode*)child)->data())
          if ((p - this->_points[index]).squaredNorm() <= r2)
            list.add(index);
    }
}

template <size_t D, typename real, typename PointArray>
int
PointTree<D, real, PointArray>::findNearestNeighbors(const vec_type& p,
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
    knnSearch(knn, inf<real>(), key_type{0LL}, this->root());
  return knn.results(indices, distances);
}

template <size_t D, typename real, typename PointArray>
real
PointTree<D, real, PointArray>::knnSearch(KNN& knn,
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
  for (auto n = nq.size(), i = 0;
    i < n && epsilon<real>() + nq.key(i) <= searchSize2(n2, r2);
    ++i)
  {
    const auto& e = nq.value(i);

    if (!e.node->isLeaf())
      r2 = knnSearch(knn, r2, e.key, (BranchNode*)e.node);
    else
      for (auto index : ((LeafNode*)e.node)->data())
        knn.test(this->_points[index], index, r2);
  }
  return r2;
}

} // namespace cg

#endif // __PointTreeBase_h
