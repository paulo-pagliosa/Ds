//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: TreeBase.h
// ========
// Class definition for quadtree/octree base.
//
// Author: Paulo Pagliosa
// Last revision: 12/01/2023

#ifndef __TreeBase_h
#define __TreeBase_h

#include "core/AllocableObject.h"
#include "core/BlockAllocable.h"
#include "core/ContentHolder.h"
#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include <cassert>
#include <set>

namespace cg
{ // begin namespace cg

template <size_t n>
inline constexpr size_t
ipow2()
{
  return 2 * ipow2<n - 1>();
}

template <>
inline constexpr size_t
ipow2<0>()
{
  return 1;
}

//
// Forward definitions
//
template <int D> class TreeBase;
template <int D> class TreeBranchNodeBase;
template <int D> class TreeKey;
template <int D> class TreeNeighborInfo;


/////////////////////////////////////////////////////////////////////
//
// TreeNodeBase: tree node base class
// ============
template <int D>
class TreeNodeBase
{
public:
  static constexpr auto dim()
  {
    return D;
  }

#ifdef _COLORED_TREE
  mutable int color;
#endif // _COLORED_TREE

  virtual ~TreeNodeBase() = default;

  virtual bool isLeaf() const
  {
    return true;
  }

  auto parent() const
  {
    return _parent;
  }

  auto index() const
  {
    return _index;
  }

  auto depth() const
  {
    return _depth;
  }

protected:
  TreeBranchNodeBase<D>* _parent;
  int _index;
  uint32_t _depth;

  friend TreeBranchNodeBase<D>;

}; // TreeNodeBase


/////////////////////////////////////////////////////////////////////
//
// TreeBranchNodeBase: tree branch node base class
// ==================
template <int D>
class TreeBranchNodeBase: public TreeNodeBase<D>
{
public:
  static constexpr auto N = (int)ipow2<D>();

  TreeBranchNodeBase()
  {
    for (int i = 0; i < N; ++i)
      _children[i] = nullptr;
    this->_depth = 0;
  }

  ~TreeBranchNodeBase() override
  {
    for (int i = 0; i < N; ++i)
      delete _children[i];
  }

  bool isLeaf() const override
  {
    return false;
  }

  auto child(int index) const
  {
    assert(index >= 0 && index < N);
    return _children[index];
  }

  bool hasChild(int index) const
  {
    return child(index) != nullptr;
  }

  void setChild(int index, TreeNodeBase<D>* child)
  {
    assert(index >= 0 && index < N);
    _children[child->_index = index] = child;
    child->_depth = (child->_parent = this)->_depth + 1;
  }

protected:
  TreeNodeBase<D>* _children[N];

  friend class TreeBase<D>;

}; // TreeBranchNodeBase


/////////////////////////////////////////////////////////////////////
//
// TreeLeafNode: generic tree leaf node class
// ============
template <int D, typename T>
class TreeLeafNode: public TreeNodeBase<D>,
  public BlockAllocable<TreeLeafNode<D, T>, 128>,
  public ContentHolder<T>
{
public:
  ASSERT_NOT_VOID(T, "Tree leaf data type cannot be void");

}; // TreeLeafNode


/////////////////////////////////////////////////////////////////////
//
// TreeBranchNode: generic tree branch node class
// ==============
template <int D, typename T>
class TreeBranchNode: public TreeBranchNodeBase<D>,
  public BlockAllocable<TreeBranchNode<D, T>, 128>,
  public ContentHolder<T>
{
public:
  template <typename Node>
  Node* createChild(int index)
  {
    auto child = new Node;

    this->setChild(index, child);
    return child;
  }

}; // TreeBranchNode


/////////////////////////////////////////////////////////////////////
//
// TreeBase: tree base class
// ========
template <int D>
class TreeBase: public SharedObject
{
public:
  static_assert(D == 2 || D == 3, "Tree: bad dimension");

  static constexpr auto dim()
  {
    return D;
  }

  ~TreeBase() override
  {
    delete _root;
  }

  void clear();

  auto leafCount() const
  {
    return _leafCount;
  }

  auto branchCount() const
  {
    return _branchCount;
  }

  auto depth() const
  {
    return _depth;
  }

  auto maxDepth() const
  {
    return _maxDepth;
  }

protected:
  using Node = TreeNodeBase<D>;
  using BranchNode = TreeBranchNodeBase<D>;

  BranchNode* _root;
  size_t _leafCount;
  size_t _branchCount;
  uint32_t _depth;
  uint32_t _maxDepth;
  uint64_t _depthMask;

  static uint64_t sizeBits(uint32_t d)
  {
    return 1LL << d;
  }

  TreeBase(BranchNode* root, uint32_t maxDepth):
    _root{root},
    _leafCount{0},
    _branchCount{1},
    _depth{0},
    _maxDepth{maxDepth}
  {
    if (maxDepth < 1 || maxDepth > 64)
      throw std::logic_error("TreeBase: bad max depth");
    _depthMask = sizeBits(maxDepth - 1);
    root->_parent = nullptr;
    root->_index = -1;
#ifdef _COLORED_TREE
    root->color = 0;
#endif // _COLORED_TREE
  }

  TreeBase(TreeBase<D>&& other, BranchNode* otherRoot = nullptr):
    _root{other._root},
    _leafCount{other._leafCount},
    _branchCount{other._branchCount},
    _depth{other._depth},
    _maxDepth{other._maxDepth},
    _depthMask{other._depthMask}
  {
    other._root = otherRoot;
    other._leafCount = other._branchCount = other._depth = 0;
  }

  void setDepth(uint32_t d)
  {
    _depth = d;
  }

  Node* findLeaf(const TreeKey<D>& k) const
  {
    return findLeaf(k, _depthMask, _root);
  }

private:
  static Node* findLeaf(const TreeKey<D>&, uint64_t, BranchNode*);

}; // TreeBase

template <int D>
void
TreeBase<D>::clear()
{
  for (int i = 0; i < BranchNode::N; ++i)
  {
    delete _root->_children[i];
    _root->_children[i] = nullptr;
  }
  _leafCount = 0;
  _branchCount = 1;
}

template <int D>
typename TreeBase<D>::Node*
TreeBase<D>::findLeaf(const TreeKey<D>& k, uint64_t mask, BranchNode* branch)
{
  auto* child = branch->child(k.childIndex(mask));

  return nullptr == child || child->isLeaf() ?
    child : findLeaf(k, mask >> 1, (BranchNode*)child);
}


/////////////////////////////////////////////////////////////////////
//
// TreeIteratorBase: tree iterator base class
// ================
template <int D>
class TreeIteratorBase
{
public:
  static constexpr auto dim()
  {
    return D;
  }

  bool operator ==(const TreeIteratorBase<D>& other) const
  {
    return _node == other._node;
  }

  bool operator !=(const TreeIteratorBase<D>& other) const
  {
    return !operator ==(other);
  }

  bool isNull() const
  {
    return _node == nullptr;
  }

  bool isLeaf() const
  {
    return _node->isLeaf();
  }

  const auto& key() const
  {
    return _key;
  }

  auto index() const
  {
    return _node->index();
  }

  auto depth() const
  {
    return _node->depth();
  }

  void pushChild(int i)
  {
    if (_node->isLeaf())
      _node = nullptr;
    else
    {
      auto child = ((TreeBranchNodeBase<D>*)_node)->child(i);

      if ((_node = child) != nullptr)
        _key.pushChild(i);
    }
  }

#ifdef _COLORED_TREE
  const auto& color() const
  {
    return _node->color;
  }

  auto& color()
  {
    return _node->color;
  }

  template <typename I>
  void setColor(I color)
  {
    _node->color = (int)color;
  }
#endif // _COLORED_TREE

protected:
  TreeNodeBase<D>* _node;
  TreeKey<D> _key;

  TreeIteratorBase(TreeBranchNodeBase<D>* root):
    _node{root},
    _key{0LL}
  {
    // do nothing
  }

  TreeIteratorBase<D>& operator ++();

private:
  static TreeNodeBase<D>* nextChild(TreeBranchNodeBase<D>*, int, TreeKey<D>&);

  TreeIteratorBase():
    _node{nullptr}
  {
    // do nothing
  }

}; // TreeIteratorBase

template <int D>
inline TreeNodeBase<D>*
TreeIteratorBase<D>::nextChild(TreeBranchNodeBase<D>* branch,
  int i,
  TreeKey<D>& key)
{
  constexpr auto N = (int)ipow2<D>();

  while (++i < N)
    if (auto node = branch->child(i))
    {
      key.pushChild(i);
      return node;
    }
  return nullptr;
}

template <int D>
TreeIteratorBase<D>&
TreeIteratorBase<D>::operator ++()
{
  if (_node != nullptr)
  {
    if (!_node->isLeaf())
      _node = nextChild((TreeBranchNodeBase<D>*)_node, -1, _key);
    else
    {
      auto temp = _node;
      auto parent = temp->parent();

      for (;;)
      {
        _key.popChild();
        _node = nextChild(parent, temp->index(), _key);
        if (_node != nullptr)
          break;
        parent = (temp = parent)->parent();
        if (parent == nullptr)
          break;
      }
    }
  }
  return *this;
}

template <typename iterator>
inline auto
child(const iterator& tit, int i)
{
  auto cit = tit;

  cit.pushChild(i);
  return cit;
}


/////////////////////////////////////////////////////////////////////
//
// TreeIterator: generic tree iterator
// ============
template <int D, typename Tree>
class TreeIterator: public TreeIteratorBase<D>
{
public:
  using Base = TreeIteratorBase<D>;
  using iterator = TreeIterator<D, Tree>;

  TreeIterator():
    Base{nullptr}
  {
    // do nothing
  }

  explicit TreeIterator(Tree& tree):
    Base{tree.root()}
  {
    // do nothing
  }

  iterator& operator ++()
  {
    return static_cast<iterator&>(Base::operator ++());
  }

  iterator operator++(int)
  {
    iterator temp{*this};

    operator ++();
    return temp;
  }

protected:
  explicit TreeIterator(TreeNodeBase<D>* node, const TreeKey<D>& key):
    Base{nullptr}
  {
    this->_node = node;
    this->_key = key;
  }

  friend Tree;

}; // TreeIterator


/////////////////////////////////////////////////////////////////////
//
// TreeLeafIterator: generic tree leaf iterator
// ================
template <int D, typename Tree>
class TreeLeafIterator: public TreeIterator<D, Tree>
{
public:
  using Base = TreeIterator<D, Tree>;
  using iterator = TreeLeafIterator<D, Tree>;
  using data_type = typename Tree::leaf_data_type;

  TreeLeafIterator() = default;
  TreeLeafIterator(const iterator&) = default;

  explicit TreeLeafIterator(Tree& tree):
    Base{tree}
  {
    operator ++();
  }

  template <typename iterator>
  TreeLeafIterator(const iterator& tit):
    Base{tit}
  {
    if (!tit.isNull() && !tit.isLeaf())
      this->_node = nullptr;
  }

  auto parentIndex() const
  {
    return this->_node->parent()->index();
  }

  const auto& data() const
  {
    return leafNode()->data();
  }

  auto& data()
  {
    return leafNode()->data();
  }

  void setData(const data_type& data)
  {
    leafNode()->setData(data);
  }

  iterator& operator ++()
  {
    do
      Base::operator ++();
    while (this->_node && !this->_node->isLeaf());
    return *this;
  }

  iterator operator ++(int)
  {
    iterator temp{*this};

    operator ++();
    return temp;
  }

private:
  using LeafNode = TreeLeafNode<D, data_type>;

  explicit TreeLeafIterator(LeafNode* node, const TreeKey<D>& key):
    Base{node, key}
  {
    // do nothing
  }

  auto leafNode() const
  {
    return (LeafNode*)this->_node;
  }

  auto leafNode()
  {
    return (LeafNode*)this->_node;
  }

  friend Tree;

}; // TreeLeafIterator


/////////////////////////////////////////////////////////////////////
//
// RegionTree: generic region tree class
// ==========
template <int D, typename real, typename LT, typename BT = void>
class RegionTree: public TreeBase<D>
{
public:
  using tree_type = RegionTree<D, real, LT, BT>;
  using iterator = TreeIterator<D, tree_type>;
  using leaf_iterator = TreeLeafIterator<D, tree_type>;
  using bounds_type = Bounds<real, D>;
  using vec_type = Vector<real, D>;
  using leaf_data_type = LT;
  using key_type = TreeKey<D>;

  static constexpr auto dflFatFactor = (real)1.01;

  static auto fatFactor()
  {
    return _fatFactor;
  }

  static void setFatFactor(real s)
  {
    if (s >= 1)
      _fatFactor = s;
  }

  RegionTree(const bounds_type& bounds, uint32_t maxDepth = 20);

  RegionTree(tree_type&& other):
    TreeBase<D>{std::move(other), new BranchNode{}},
    _bounds{other._bounds},
    _resolution{other._resolution},
    _scale{other._scale}
  {
    // do nothing
  }

  RegionTree(const tree_type&) = delete;
  tree_type& operator= (const tree_type&) = delete;

  iterator begin()
  {
    return iterator{*this};
  }

  const iterator end() const
  {
    return iterator{};
  }

  leaf_iterator leafBegin()
  {
    return leaf_iterator{*this};
  }

  const leaf_iterator leafEnd() const
  {
    return leaf_iterator{};
  }

  const leaf_iterator pickLeaf(const vec_type& p) const
  {
    if (!_bounds.contains(p))
      return leaf_iterator{nullptr, {}};

    auto k = this->key(p);
    auto node = this->findLeaf(k);

    k.popChildren(this->_maxDepth - node->depth());
    return leaf_iterator{(TreeLeafNode<D, LT>*)node, k};
  }

  const auto& bounds() const
  {
    return _bounds;
  }

  auto bounds(const key_type& key, int depth) const
  {
    const auto s = nodeSize(depth);
    const auto p = _bounds[0] + s * vec_type{key};

    return bounds_type{p, p + s};
  }

  auto bounds(const iterator& i) const
  {
    return bounds(i.key(), i.depth());
  }

  auto point(const key_type& key, int depth, const vec_type& p) const
  {
    return _bounds[0] + nodeSize(depth) * (vec_type{key} + p);
  }

  auto point(const leaf_iterator& i, const vec_type& p) const
  {
    return point(i.key(), i.depth(), p);
  }

  auto center(const key_type& key, int depth) const
  {
    return point(key, depth, vec_type{real(0.5)});
  }

  auto center(const leaf_iterator& i) const
  {
    return center(i.key(), i.depth());
  }

  const auto& resolution() const
  {
    return _resolution;
  }

  template <typename V>
  auto key(const V& p) const
  {
    return key_type{(vec_type{p} - _bounds[0]) * _scale};
  }

  void makeEmptyLeafs()
  {
    makeEmptyLeafs(root());
  }

  void balanceTree();

  iterator findNeighbor(const iterator& it, int direction) const
  {
    assert(direction >= 0 && direction < (int)D * 2);

    auto [node, key] = findNeighbor(NodeIt{it._node, it._key}, direction);
    return iterator{node, key};
  }

protected:
  using LeafNode = TreeLeafNode<D, LT>;
  using BranchNode = TreeBranchNode<D, BT>;

  bounds_type _bounds;
  vec_type _resolution;
  vec_type _scale;

  BranchNode* root() const
  {
    return (BranchNode*)this->_root;
  }

  LeafNode* makeLeaf(const key_type&, uint64_t, BranchNode*);

  LeafNode* makeLeaf(const key_type& key)
  {
    return this->makeLeaf(key, this->_depthMask, root());
  }

  LeafNode* createLeafChild(BranchNode* parent, int i)
  {
    auto child = parent->template createChild<LeafNode>(i);
    auto childDepth = child->depth();

    if (childDepth > this->_depth)
      this->_depth = childDepth;
#ifdef _COLORED_TREE
    child->color = parent->color;
#endif // _COLORED_TREE
    ++this->_leafCount;
    return child;
  }

  BranchNode* createBranchInPlaceOf(LeafNode* leaf)
  {
    auto parent = (BranchNode*)leaf->parent();
    auto child = parent->template createChild<BranchNode>(leaf->index());

#ifdef _COLORED_TREE
    child->color = leaf->color;
#endif // _COLORED_TREE
    ++this->_branchCount;
    return child;
  }

  void deleteLeaf(LeafNode* leaf)
  {
    delete leaf;
    --this->_leafCount;
  }

  BranchNode* splitEmptyLeaf(LeafNode* leaf)
  {
    if (leaf->depth() == this->_maxDepth)
      return nullptr;

    auto branch = createBranchInPlaceOf(leaf);
    constexpr auto N = (int)ipow2<D>();

    for (int i = 0; i < N; ++i)
      createLeafChild(branch, i);
    deleteLeaf(leaf);
    return branch;
  }

  virtual void moveDataToChildren(LeafNode* leaf,
    BranchNode* branch,
    const key_type& key)
  {
    (void)leaf;
    (void)branch;
    (void)key;
  }

  auto nodeSize(uint32_t depth) const
  {
    return _resolution * real(this->sizeBits(this->_maxDepth - depth));
  }

  static auto leafNode(const leaf_iterator& lit)
  {
    return lit.leafNode();
  }

  static auto leafIterator(LeafNode* leaf, const key_type& key)
  {
    return leaf_iterator{leaf, key};
  }

private:
  struct NodeIt
  {
    TreeNodeBase<D>* node;
    key_type key;

    NodeIt() = default;

    NodeIt(TreeNodeBase<D>* node, const key_type& key):
      node{node}, key{key}
    {
      // do nothing
    }

    NodeIt child(int i) const
    {
      return {((BranchNode*)node)->child(i), key.childKey(i)};
    }

    bool operator <(const NodeIt& other) const
    {
      return node < other.node;
    }

    static auto null()
    {
      NodeIt n;

      n.node = nullptr;
      return n;
    }

  }; // NodeIt

  using NodeSet = std::set<NodeIt>;

  static real _fatFactor;

  static bool isBalanced(const BranchNode* branch, int direction)
  {
    const auto nc = TreeNeighborInfo<D>::neighborChildCode(direction);

    for (int i = 0; i < TreeNeighborInfo<D>::S; ++i)
      if (!branch->child(nc[i])->isLeaf())
        return false;
    return true;
  }

  void makeEmptyLeafs(BranchNode*);
  void makeEmptyLeafs(BranchNode*, const key_type&, NodeSet&);

  NodeIt findNeighbor(const NodeIt&, int) const;

  friend iterator;

}; // RegionTree

template <int D, typename real, typename LT, typename BT>
real RegionTree<D, real, LT, BT>::_fatFactor = dflFatFactor;

template <int D, typename real, typename LT, typename BT>
RegionTree<D, real, LT, BT>::RegionTree(const bounds_type& bounds,
  uint32_t maxDepth):
  TreeBase<D>{new BranchNode(), maxDepth},
  _bounds{bounds}
{
  if (bounds.empty())
    throw std::runtime_error("RegionTree: empty bounds");
  _bounds.inflate(_fatFactor);
  _resolution = _bounds.size() * (1 / real(this->sizeBits(maxDepth)));
  _scale = _resolution.inverse();
}

template <int D, typename real, typename LT, typename BT>
typename RegionTree<D, real, LT, BT>::LeafNode*
RegionTree<D, real, LT, BT>::makeLeaf(const key_type& key,
  uint64_t mask,
  BranchNode* branch)
{
  if (branch->depth() == this->_maxDepth)
    return nullptr;

  auto i = key.childIndex(mask);
  auto child = branch->child(i);

  if (child == nullptr)
    child = createLeafChild(branch, i);
  else if (!child->isLeaf())
    return makeLeaf(key, mask >> 1, (BranchNode*)child);
  return (LeafNode*)child;
}

template <int D, typename real, typename LT, typename BT>
void
RegionTree<D, real, LT, BT>::makeEmptyLeafs(BranchNode* branch)
{
  constexpr auto N = (int)ipow2<D>();

  for (int i = 0; i < N; ++i)
  {
    auto child = branch->child(i);

    if (child == nullptr)
      createLeafChild(branch, i);
    else if (!child->isLeaf())
      makeEmptyLeafs((BranchNode*)child);
  }
}

template <int D, typename real, typename LT, typename BT>
void
RegionTree<D, real, LT, BT>::makeEmptyLeafs(BranchNode* branch,
  const key_type& key,
  NodeSet& leafs)
{
  constexpr auto N = (int)ipow2<D>();

  for (int i = 0; i < N; ++i)
  {
    auto child = branch->child(i);
    auto childKey = key.childKey(i);

    if (child == nullptr)
      child = createLeafChild(branch, i);
    else if (!child->isLeaf())
    {
      makeEmptyLeafs((BranchNode*)child, childKey, leafs);
      continue;
    }
    leafs.emplace((LeafNode*)child, childKey);
  }
}

template <int D, typename real, typename LT, typename BT>
typename RegionTree<D, real, LT, BT>::NodeIt
RegionTree<D, real, LT, BT>::findNeighbor(const NodeIt& nit,
  int direction) const
{
  auto node = nit.node;
  auto parent = node->parent();

  if (parent == nullptr)
    return NodeIt::null();

  auto pit = NodeIt{parent, key_type(nit.key).popChild()};
  const auto nc = TreeNeighborInfo<D>::neighborCode(direction);
  constexpr auto S = TreeNeighborInfo<D>::S * 2;

  for (int i = 0; i < S; i += 2)
    if (node == parent->child(nc[i]))
      return pit.child(nc[i + 1]);

  auto pnit = findNeighbor(pit, direction);

  if (pnit.node == nullptr || pnit.node->isLeaf())
    return pnit;
  for (int i = 0; i < S; i += 2)
    if (node == parent->child(nc[i + 1]))
      return pnit.child(nc[i]);
  return NodeIt::null();
}

template <int D, typename real, typename LT, typename BT>
void
RegionTree<D, real, LT, BT>::balanceTree()
{
  constexpr auto S = (int)D * 2;
  NodeSet leafs;

  makeEmptyLeafs(root(), key_type{0LL}, leafs);
  while (!leafs.empty())
  {
    auto lit = leafs.begin();
    auto currentLeaf = *lit;
    auto key = currentLeaf.key;
    auto depth = currentLeaf.node->depth();
    NodeIt neighbors[S];
    bool splitted{false};

    for (int direction = 0; direction < S; ++direction)
    {
      auto neighbor = findNeighbor(currentLeaf, direction);

      if (neighbor.node == nullptr || neighbor.node->isLeaf())
      {
        neighbors[direction] = neighbor;
        continue;
      }
      neighbors[direction] = NodeIt::null();
      if (!splitted && !isBalanced((BranchNode*)neighbor.node, direction))
      {
        auto leaf = (LeafNode*)currentLeaf.node;
        auto branch = createBranchInPlaceOf(leaf);

        for (int i = 0; i < (int)ipow2<D>(); ++i)
        {
          auto child = createLeafChild(branch, i);
          leafs.emplace(child, key_type(currentLeaf.key).pushChild(i));
        }
        moveDataToChildren(leaf, branch, key);
        deleteLeaf(leaf);
        splitted = true;
      }
    }
    leafs.erase(lit);
    if (!splitted)
      continue;
    for (int direction = 0; direction < S; ++direction)
    {
      auto neighbor = neighbors[direction];

      if (neighbor.node != nullptr && neighbor.node->depth() <= depth)
        leafs.insert(neighbor);
    }
  }
}

} // end namespace cg

#endif // __TreeBase_h
