//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2024 Paulo Pagliosa.                        |
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
// Last revision: 01/05/2024

#ifndef __BVH_h
#define __BVH_h

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
  class NodeView;

  using NodeFunction = std::function<void(const NodeView&)>;

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

  Node* makeNode(PrimitiveInfoArray&, uint32_t, uint32_t, IndexArray&);
  Node* makeLeaf(PrimitiveInfoArray&, uint32_t, uint32_t, IndexArray&);

  friend NodeView;

}; // BVHBase

class BVHBase::Node
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
    _count{}
  {
    _bounds.inflate(c0->_bounds);
    _bounds.inflate(c1->_bounds);
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
  const auto& bounds() const
  {
    return _node->_bounds;
  }

  auto isLeaf() const
  {
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
    return _node->_first;
  }

  auto count() const
  {
    return _node->_count;
  }

  auto operator ==(const NodeView& other) const
  {
    return _node == other._node;
  }

private:
  const Node* _node;

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

inline BVHBase::NodeView
BVHBase::root() const
{
  return _root;
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

  BVH(PrimitiveArray&&, uint32_t = 8);

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
BVH<T>::BVH(PrimitiveArray&& primitives, uint32_t maxPrimitivesPerNode):
  BVHBase{maxPrimitivesPerNode},
  _primitives{std::move(primitives)}
{
  auto np = (uint32_t)_primitives.size();

  assert(np > 0);
  _primitiveIds.resize(np);

  PrimitiveInfoArray primitiveInfo(np);

  for (uint32_t i = 0; i < np; ++i)
    primitiveInfo[i] = {_primitiveIds[i] = i, _primitives[i]->bounds()};
  build(primitiveInfo);
}

template <typename T>
bool
BVH<T>::intersectLeaf(uint32_t first, uint32_t count, const Ray3f& ray) const
{
  for (auto i = first, e = i + count; i < e; ++i)
  {
    const auto& p = _primitives[_primitiveIds[i]];
    Intersection temp;

    if (p->intersect(ray))
      return true;
  }
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
    const auto& p = _primitives[_primitiveIds[i]];
    Intersection temp;

    if (p->intersect(ray, temp) && temp.distance < hit.distance)
      hit = temp;
  }
}

} // end namespace cg

#endif // __BVH_h
