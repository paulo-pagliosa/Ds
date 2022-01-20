//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Orthrus Group.                         |
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
// OVERVIEW: BVH.h
// ========
// Class definition for BVH.
//
// Author: Paulo Pagliosa
// Last revision: 20/01/2022

#ifndef __BVH_h
#define __BVH_h

#include "graphics/Primitive.h"
#include <functional>
#include <cassert>
#include <cinttypes>
#include <vector>

namespace cg
{ // begin namespace cg

struct BVHNodeInfo
{
  Bounds3f bounds;
  bool isLeaf;
  uint32_t first;
  uint32_t count;

}; // BVHNodeInfo

using BVHNodeFunction = std::function<void(const BVHNodeInfo&)>;


/////////////////////////////////////////////////////////////////////
//
// BVHBase: BVH base class
// =======
class BVHBase abstract: public Aggregate
{
public:
  ~BVHBase() override;

  auto size() const
  {
    return (size_t)_nodeCount;
  }

  bool intersect(const Ray3f&) const override;
  bool intersect(const Ray3f&, Intersection&) const override;
  Bounds3f bounds() const override;

  void iterate(BVHNodeFunction) const;

protected:
  struct PrimitiveInfo;

  using PrimitiveInfoArray = std::vector<PrimitiveInfo>;
  using IndexArray = std::vector<uint32_t>;

  IndexArray _primitiveIds;

  BVHBase(uint32_t maxPrimitivesPerNode):
    _maxPrimitivesPerNode{maxPrimitivesPerNode}
  {
    // do nothing
  }

  void build(PrimitiveInfoArray& primitiveInfo)
  {
    auto np = (uint32_t)primitiveInfo.size();
    IndexArray orderedPrimitiveIds(np);

    _root = makeNode(primitiveInfo, 0, np, orderedPrimitiveIds);
    _primitiveIds.swap(orderedPrimitiveIds);
  }

  virtual void intersectPrimitives(uint32_t first,
    uint32_t count,
    const Ray3f& ray,
    Intersection& hit) const abstract;

private:
  struct NodeRay;
  struct Node;

  Node* _root{};
  uint32_t _nodeCount{};
  uint32_t _maxPrimitivesPerNode;

  Node* makeNode(PrimitiveInfoArray&, uint32_t, uint32_t, IndexArray&);
  Node* makeLeaf(PrimitiveInfoArray&, uint32_t, uint32_t, IndexArray&);

}; // BVHBase

struct BVHBase::PrimitiveInfo
{
  uint32_t index;
  Bounds3f bounds;
  vec3f centroid;

  PrimitiveInfo() = default;

  PrimitiveInfo(uint32_t index, const Bounds3f& bounds):
    index{index},
    bounds{bounds},
    centroid{bounds.center()}
  {
    // do nothing
  }

}; // BVHBase::PrimitiveInfo


/////////////////////////////////////////////////////////////////////
//
// BVH: BVH class
// ===
class BVH final: public BVHBase
{
public:
  using PrimitiveArray = Array<Reference<Primitive>>;

  BVH(PrimitiveArray&&, uint32_t = 16);

  auto& primitives() const
  {
    return _primitives;
  }

private:
  PrimitiveArray _primitives;

  void intersectPrimitives(uint32_t first,
    uint32_t count,
    const Ray3f& ray,
    Intersection& hit) const override;

}; // BVH

} // end namespace cg

#endif // __BVH_h
