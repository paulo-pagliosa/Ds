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
// OVERVIEW: BVH.cpp
// ========
// Source file for BVH.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#include "geometry/BVH.h"
#include <algorithm>
#include <stack>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// BVHBase implementation
// =======
class BVHBase::NodeRay: public Ray3f
{
public:
  explicit NodeRay(const Ray3f& r):
    Ray3f{r}
  {
    invDir = r.direction.inverse();
    isNegDir[0] = r.direction.x < 0;
    isNegDir[1] = r.direction.y < 0;
    isNegDir[2] = r.direction.z < 0;
  }

  vec3f invDir;
  int isNegDir[3];

}; // BVHBase::NodeRay

inline bool
BVHBase::Node::intersect(const NodeRay& r) const
{
  auto tMin = (_bounds[r.isNegDir[0]].x - r.origin.x) * r.invDir.x;
  auto tMax = (_bounds[1 - r.isNegDir[0]].x - r.origin.x) * r.invDir.x;
  auto aMin = (_bounds[r.isNegDir[1]].y - r.origin.y) * r.invDir.y;
  auto aMax = (_bounds[1 - r.isNegDir[1]].y - r.origin.y) * r.invDir.y;

  if (tMin > aMax || aMin > tMax)
    return false;
  if (aMin > tMin)
    tMin = aMin;
  if (aMax < tMax)
    tMax = aMax;
  aMin = (_bounds[r.isNegDir[2]].z - r.origin.z) * r.invDir.z;
  aMax = (_bounds[1 - r.isNegDir[2]].z - r.origin.z) * r.invDir.z;
  if (tMin > aMax || aMin > tMax)
    return false;
  if (aMin > tMin)
    tMin = aMin;
  if (aMax < tMax)
    tMax = aMax;
  return tMin < r.tMax && tMax > r.tMin;
}

void
BVHBase::Node::iterate(const Node* node, NodeFunction f)
{
  if (node == nullptr)
    return;

  auto isLeaf = node->isLeaf();

  f(node);
  if (!node->isLeaf())
  {
    iterate(node->_children[0], f);
    iterate(node->_children[1], f);
  }
}

inline auto
maxDim(const Bounds3f& b)
{
  auto s = b.size();
  return s.x > s.y && s.x > s.z ? 0 : (s.y > s.z ? 1 : 2);
}

/**
 * @brief Build the nodes of this BVH.
 * 
 * The method is based on the source code of pbrt-v4 as
 * described in the 4th edition of "Physically Based Rendering:
 * From Theory to Implementation" by M. Pharr, Wenzel Jakob,
 * and Greg Humphreys, and available at
 * https://github.com/mmp/pbrt-v4.
 * 
 */
BVHBase::Node*
BVHBase::makeNode(const PrimitiveInfoArray& primitiveInfo,
  uint32_t first,
  uint32_t end)
{
  Bounds3f bounds;

  // Compute the bounds of all primitives in the node.
  for (auto i = first; i < end; ++i)
    bounds.extend(primitiveInfo[_primitiveIds[i]].bounds);
  ++_nodeCount;

  auto count = end - first;

  // If the number of primitives is less than the maximum number
  // of primitives per node, then create a leaf node.
  if (count <= _maxPrimitivesPerNode)
    return new Node{bounds, first, count};

  Bounds3f centroidBounds;

  // Compute the bounds of the primitive centroids.
  for (auto i = first; i < end; ++i)
    centroidBounds.extend(primitiveInfo[_primitiveIds[i]].centroid);

  auto dim = maxDim(centroidBounds);

  // If all primitive centroids are at the same position (volume
  // of centroid bounds is zero), then create a leaf node.
  if (centroidBounds.max()[dim] == centroidBounds.min()[dim])
    return new Node{bounds, first, count};

  const auto pidBegin = _primitiveIds.begin();
  auto mid = (first + end) >> 1;

  if (_splitMethod == Median)
  // Partition primitives into two (equally sized) subsets using
  // the median of the primitive centroids.
    std::nth_element(pidBegin + first,
      pidBegin + mid,
      pidBegin + end,
      [&primitiveInfo, dim](uint32_t a, uint32_t b)
      {
        const auto& pa = primitiveInfo[a];
        const auto& pb = primitiveInfo[b];

        return pa.centroid[dim] < pb.centroid[dim];
      });
  else
  // Partition primitives using the approximate SAH.
  {
    constexpr int maxBuckets{12};
    struct
    {
      int count{};
      Bounds3f bounds;

    } buckets[maxBuckets];
    const auto x = centroidBounds[0][dim];
    const auto s = maxBuckets / (centroidBounds[1][dim] - x);
    const auto bucketId = [dim, x, s](const PrimitiveInfo& p)
      {
        auto bid = int(s * (p.centroid[dim] - x));

        assert(bid >= 0 && bid <= maxBuckets);
        return bid != maxBuckets ? bid : maxBuckets - 1;
      };

    // Initialize the data of all SAH partition buckets.
    for (auto i = first; i < end; ++i)
    {
      const auto& p = primitiveInfo[_primitiveIds[i]];
      auto bid = bucketId(p);

      buckets[bid].count++;
      buckets[bid].bounds.extend(p.bounds);
    }

    // Compute the cost for splitting after each bucket.
    constexpr int maxSplits{maxBuckets - 1};
    float costs[maxSplits]{};
    {
      Bounds3f b;

      for (int c = 0, i = 0; i < maxSplits;)
      {
        b.extend(buckets[i].bounds);
        c += buckets[i].count;
        costs[i++] += c * b.area();
      }
    }
    {
      Bounds3f b;

      for (int c = 0, i = maxSplits; i > 0;)
      {
        b.extend(buckets[i].bounds);
        c += buckets[i].count;
        costs[--i] += c * b.area();
      }
    }

    // Find a bucket to split at that minimizes the SAH metric.
    auto minCostSplitBucket = -1;
    auto minCost = math::Limits<float>::inf();

    for (int i = 0; i < maxSplits; ++i)
      if (costs[i] < minCost)
      {
        minCost = costs[i];
        minCostSplitBucket = i;
      }

    const auto leafCost = float(count);
    minCost = 1.f / 2.f + minCost / bounds.area();

    // If the chosen bucket boundary for splitting does not have
    // a lower cost than having a node with all primitives, then
    // create a leaf node.
    if (leafCost <= minCost)
      return new Node{bounds, first, count};

    // Otherwise, partition primitives.
    auto mit = std::partition(pidBegin + first,
      pidBegin + end,
      [&primitiveInfo, &bucketId, minCostSplitBucket](uint32_t i)
      {
        return bucketId(primitiveInfo[i]) <= minCostSplitBucket;
      });
    mid = uint32_t(mit - pidBegin);
  }
  // Create an interior node and its two children.
  return new Node{makeNode(primitiveInfo, first, mid),
    makeNode(primitiveInfo, mid, end)};
}

BVHBase::~BVHBase()
{
  delete _root;
}

bool
BVHBase::intersect(const Ray3f& ray) const
{
  NodeRay r{ray};
  std::stack<Node*> stack;

  stack.push(_root);
  while (!stack.empty())
  {
    auto node = stack.top();

    stack.pop();
    if (node->intersect(r))
      if (!node->isLeaf())
      {
        stack.push(node->_children[0]);
        stack.push(node->_children[1]);
      }
      else if (intersectLeaf(node->_first, node->_count, ray))
        return true;
  }
  return false;
}

bool
BVHBase::intersect(const Ray3f& ray, Intersection& hit) const
{
  hit.object = nullptr;
  hit.distance = ray.tMax;

  NodeRay r{ray};
  std::stack<Node*> stack;

  stack.push(_root);
  while (!stack.empty())
  {
    auto node = stack.top();

    stack.pop();
    if (node->intersect(r))
      if (node->isLeaf())
        intersectLeaf(node->_first, node->_count, ray, hit);
      else
      {
        stack.push(node->_children[0]);
        stack.push(node->_children[1]);
      }
  }
  return hit.object != nullptr;
}

Bounds3f
BVHBase::bounds() const
{
  return _root == nullptr ? Bounds3f{} : _root->_bounds;
}

void
BVHBase::iterate(NodeFunction f) const
{
  Node::iterate(_root, f);
}

} // end namespace cg
