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
// OVERVIEW: BVH.cpp
// ========
// Source file for BVH.
//
// Author: Paulo Pagliosa
// Last revision: 21/01/2022

#include "geometry/BVH.h"
#include <algorithm>
#include <stack>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// BVHBase implementation
// =======
struct BVHBase::NodeRay: public Ray3f
{
  NodeRay(const Ray3f& r):
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

struct BVHBase::Node
{
  Bounds3f bounds;
  Node* children[2];
  uint32_t first;
  uint32_t count;

  ~Node()
  {
    delete children[0];
    delete children[1];
  }

  Node(const Bounds3f& bounds, uint32_t first, uint32_t count):
    bounds{bounds},
    first{first},
    count{count}
  {
    children[0] = children[1] = nullptr;
  }

  Node(Node* c0, Node* c1):
    count{0}
  {
    bounds.inflate(c0->bounds);
    bounds.inflate(c1->bounds);
    children[0] = c0;
    children[1] = c1;
  }

  bool isLeaf() const
  {
    return children[0] == nullptr;
  }

  bool intersect(const NodeRay& r) const
  {
    auto tMin = (bounds[    r.isNegDir[0]].x - r.origin.x) * r.invDir.x;
    auto tMax = (bounds[1 - r.isNegDir[0]].x - r.origin.x) * r.invDir.x;
    auto aMin = (bounds[    r.isNegDir[1]].y - r.origin.y) * r.invDir.y;
    auto aMax = (bounds[1 - r.isNegDir[1]].y - r.origin.y) * r.invDir.y;

    if (tMin > aMax || aMin > tMax)
      return false;
    if (aMin > tMin)
      tMin = aMin;
    if (aMax < tMax)
      tMax = aMax;
    aMin = (bounds[    r.isNegDir[2]].z - r.origin.z) * r.invDir.z;
    aMax = (bounds[1 - r.isNegDir[2]].z - r.origin.z) * r.invDir.z;
    if (tMin > aMax || aMin > tMax)
      return false;
    if (aMin > tMin)
      tMin = aMin;
    if (tMin > r.tMin)
      return tMin < r.tMax;
    if (aMax < tMax)
      tMax = aMax;
    if (tMax > r.tMin)
      return tMax < r.tMax;
    return false;
  }

  static void iterate(const Node*, BVHNodeFunction);

}; // BVHBase::Node

void
BVHBase::Node::iterate(const Node* node, BVHNodeFunction f)
{
  if (node == nullptr)
    return;

  auto isLeaf = node->isLeaf();

  f({node->bounds, isLeaf, node->first, node->count});
  if (!node->isLeaf())
  {
    iterate(node->children[0], f);
    iterate(node->children[1], f);
  }
}

inline BVHBase::Node*
BVHBase::makeLeaf(PrimitiveInfoArray& primitiveInfo,
  uint32_t start,
  uint32_t end,
  IndexArray& orderedPrimitiveIds)
{
  Bounds3f bounds;
  auto first = uint32_t(orderedPrimitiveIds.size());

  for (uint32_t i = start; i < end; ++i)
  {
    bounds.inflate(primitiveInfo[i].bounds);
    orderedPrimitiveIds.push_back(_primitiveIds[primitiveInfo[i].index]);
  }
  return new Node{bounds, first, end - start};
}

inline auto
maxDim(const Bounds3f& b)
{
  auto s = b.size();
  return s.x > s.y && s.x > s.z ? 0 : (s.y > s.z ? 1 : 2);
}

BVHBase::Node*
BVHBase::makeNode(PrimitiveInfoArray& primitiveInfo,
  uint32_t start,
  uint32_t end,
  IndexArray& orderedPrimitiveIds)
{
  ++_nodeCount;
  if (end - start <= _maxPrimitivesPerNode)
    return makeLeaf(primitiveInfo, start, end, orderedPrimitiveIds);

  Bounds3f centroidBounds;

  for (auto i = start; i < end; ++i)
    centroidBounds.inflate(primitiveInfo[i].centroid);

  auto dim = maxDim(centroidBounds);

  if (centroidBounds.max()[dim] == centroidBounds.min()[dim])
    return makeLeaf(primitiveInfo, start, end, orderedPrimitiveIds);

  // Partition primitives into two sets and build children
  auto mid = (start + end) / 2;

  std::nth_element(&primitiveInfo[start],
    &primitiveInfo[mid],
    &primitiveInfo[end - 1] + 1,
    [dim](const PrimitiveInfo& a, const PrimitiveInfo& b)
    {
      return a.centroid[dim] < b.centroid[dim];
    });
  return new Node{makeNode(primitiveInfo, start, mid, orderedPrimitiveIds),
    makeNode(primitiveInfo, mid, end, orderedPrimitiveIds)};
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
    if (node->intersect(ray))
      if (!node->isLeaf())
      {
        stack.push(node->children[0]);
        stack.push(node->children[1]);
      }
      else if (intersectLeaf(node->first, node->count, ray))
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
    if (node->intersect(ray))
      if (node->isLeaf())
        intersectLeaf(node->first, node->count, ray, hit);
      else
      {
        stack.push(node->children[0]);
        stack.push(node->children[1]);
      }
  }
  return hit.object != nullptr;
}

Bounds3f
BVHBase::bounds() const
{
  return _root == nullptr ? Bounds3f{} : _root->bounds;
}

void
BVHBase::iterate(BVHNodeFunction f) const
{
  Node::iterate(_root, f);
}

} // end namespace cg
