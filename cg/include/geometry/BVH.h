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
// OVERVIEW: BVH.h
// ========
// Class definition for BVH.
//
// Author: Paulo Pagliosa
// Last revision: 19/08/2026

#ifndef __BVH_h
#define __BVH_h

#include "core/BlockAllocable.h"
#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include "geometry/Intersection.h"
#include <functional>
#include <cassert>
#include <cinttypes>
#include <vector>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// BVHBase: BVH base class
// =======
class BVHBase: public SharedObject
{
public:
  enum class SplitMethod
  {
    SAH,
    Median
  };

  class NodeView;

  using NodeFunction = std::function<void(const NodeView&)>;
  using enum SplitMethod;

  ~BVHBase() override;

  auto size() const
  {
    return (size_t)_nodeCount;
  }

  NodeView root() const;
  Bounds3f bounds() const;
  bool intersect(const Ray3f&) const;
  bool intersect(const Ray3f&, Intersection&) const;
  void iterate(NodeFunction) const;

  auto empty() const
  {
    return _nodeCount == 0;
  }

  auto primitiveId(int i) const
  {
    return _primitiveIds[i];
  }

protected:
  class PrimitiveInfo;

  using PrimitiveInfoArray = std::vector<PrimitiveInfo>;
  using IndexArray = std::vector<uint32_t>;

  BVHBase(uint32_t maxPrimitivesPerNode, SplitMethod splitMethod):
    _maxPrimitivesPerNode{maxPrimitivesPerNode},
    _splitMethod{splitMethod}
  {
    assert(maxPrimitivesPerNode > 0);
  }

  void build(const PrimitiveInfoArray&);

  virtual bool intersectLeaf(uint32_t, uint32_t, const Ray3f&) const = 0;
  virtual void intersectLeaf(uint32_t,
    uint32_t,
    const Ray3f&,
    Intersection&) const = 0;

private:
  class NodeRay;
  class Node;

  Node* _root{};
  uint32_t _nodeCount{};
  uint32_t _maxPrimitivesPerNode;
  IndexArray _primitiveIds;
  SplitMethod _splitMethod;

  Node* makeNode(const PrimitiveInfoArray&, uint32_t, uint32_t);

  friend NodeView;

}; // BVHBase

class BVHBase::Node: public BlockAllocable<BVHBase::Node, DflBlockSize>
{
public:
  ~Node()
  {
    delete _children[0];
    delete _children[1];
  }

private:
  Bounds3f _bounds;
  Node* _children[2];
  uint32_t _first;
  uint32_t _count;

  Node(const Bounds3f& bounds, uint32_t first, uint32_t count):
    _bounds{bounds},
    _first{first},
    _count{count}
  {
    _children[0] = _children[1] = nullptr;
  }

  Node(Node* c0, Node* c1):
    _bounds{c0->_bounds + c1->_bounds},
    _count{}
  {
    _children[0] = c0;
    _children[1] = c1;
  }

  bool isLeaf() const
  {
    return _children[0] == nullptr;
  }

  bool intersect(const NodeRay&) const;

  static void iterate(const Node*, NodeFunction);

  friend BVHBase;
  friend NodeView;

}; // BVHBase::Node

class BVHBase::NodeView
{
public:
  NodeView() = default;

  const auto& bounds() const
  {
    assert(_node);
    return _node->_bounds;
  }

  auto isLeaf() const
  {
    assert(_node);
    return _node->isLeaf();
  }

  auto child(int i) const
  {
    assert(!isLeaf());
    assert(i == 0 || i == 1);
    return NodeView{_node->_children[i]};
  }

  auto first() const
  {
    assert(_node);
    return _node->_first;
  }

  auto count() const
  {
    assert(_node);
    return _node->_count;
  }

  auto operator ==(const NodeView& other) const
  {
    return _node == other._node;
  }

private:
  const Node* _node{};

  NodeView(const Node* node):
    _node{node}
  {
    // do nothing
  }

  friend BVHBase;

}; // BVHBase::NodeView

class BVHBase::PrimitiveInfo
{
public:
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

inline auto
BVHBase::root() const -> NodeView
{
  return _root;
}

inline void
BVHBase::build(const PrimitiveInfoArray& primitiveInfo)
{
  auto np = (uint32_t)primitiveInfo.size();

  _primitiveIds.resize(np);
  for (uint32_t i = 0; i < np; ++i)
    _primitiveIds[i] = i;
  _root = makeNode(primitiveInfo, 0, np);
}


/////////////////////////////////////////////////////////////////////
//
// BVH: BVH class
// ===
template <typename T>
class BVH final: public BVHBase
{
public:
  using PrimitiveArray = std::vector<Reference<T>>;

  BVH(PrimitiveArray&&, uint32_t = 8, SplitMethod = SAH);

  auto& primitives() const
  {
    return _primitives;
  }

private:
  PrimitiveArray _primitives;

  bool intersectLeaf(uint32_t, uint32_t, const Ray3f&) const override;
  void intersectLeaf(uint32_t,
    uint32_t,
    const Ray3f&,
    Intersection&) const override;

}; // BVH

template <typename T>
BVH<T>::BVH(PrimitiveArray&& primitives,
  uint32_t maxPrimitivesPerNode,
  SplitMethod splitMethod):
  BVHBase{maxPrimitivesPerNode, splitMethod},
  _primitives{std::move(primitives)}
{
  auto np = (uint32_t)_primitives.size();

  assert(np > 0);

  PrimitiveInfoArray primitiveInfo(np);

  for (uint32_t i = 0; i < np; ++i)
    primitiveInfo[i] = {i, _primitives[i]->bounds()};
  build(primitiveInfo);
}

template <typename T>
bool
BVH<T>::intersectLeaf(uint32_t first, uint32_t count, const Ray3f& ray) const
{
  for (auto i = first, e = i + count; i < e; ++i)
    if (const auto& p = _primitives[primitiveId(i)]; p->intersect(ray))
      return true;
  return false;
}

template <typename T>
void
BVH<T>::intersectLeaf(uint32_t first,
  uint32_t count,
  const Ray3f& ray,
  Intersection& hit) const
{
  for (auto i = first, e = i + count; i < e; ++i)
  {
    const auto& p = _primitives[primitiveId(i)];
    Intersection temp;

    if (p->intersect(ray, temp) && temp.distance < hit.distance)
      hit = temp;
  }
}

} // end namespace cg

#endif // __BVH_h
