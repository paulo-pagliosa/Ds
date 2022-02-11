//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Orthrus Group.                         |
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
// OVERVIEW: IndexList.h
// ========
// Class definition for index list.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#ifndef __IndexList_h
#define __IndexList_h

#include "core/BlockAllocable.h"
#include "core/ContentHolder.h"

namespace cg
{ // begin namespace cg

class IndexList;
class IndexListIterator;


/////////////////////////////////////////////////////////////////////
//
// IndexListNode: index list node class
// =============
class IndexListNode: public BlockAllocable<IndexListNode, 1024>
{
public:
  IndexListNode() = default;

private:
  using value_type = int;

  value_type _index{-1};
  IndexListNode* _next{};

  IndexListNode(value_type index, IndexListNode* next):
    _index{index}, _next{next}
  {
    // do nothing
  }

  static IndexListNode* _null;

  friend IndexList;
  friend IndexListIterator;

}; // IndexListNode


/////////////////////////////////////////////////////////////////////
//
// IndexListIterator: index list iterator class
// =================
class IndexListIterator
{
public:
  using iterator = IndexListIterator;
  using index_type = IndexListNode::value_type;

  IndexListIterator(IndexListNode*& node, const IndexList& list):
#ifdef _DEBUG
    _list{&list},
#endif // _DEBUG
    _node{&node}
  {
    (void)list;
  }

  const index_type& operator *() const
  {
#ifdef _DEBUG
    if (_list == nullptr || *_node == IndexListNode::_null)
      throw std::logic_error("Index list iterator not dereferencable");
#endif // _DEBUG
    return (*_node)->_index;
  }

  iterator& operator ++()
  {
#ifdef _DEBUG
    if (_list == nullptr || *_node == IndexListNode::_null)
      throw std::logic_error("Index list iterator not incrementable");
#endif // _DEBUG
    _node = &(*_node)->_next;
    return *this;
  }

  iterator operator ++(int)
  {
    iterator temp{*this};

    ++*this;
    return temp;
  }

  bool operator ==(const IndexListIterator& other) const
  {
#ifdef _DEBUG
    if (_list != other._list)
      throw std::logic_error("Index list iterators incompatible");
#endif // _DEBUG
    return *_node == *other._node;
  }

  bool operator !=(const IndexListIterator& other) const
  {
    return !operator ==(other);
  }

private:
  IndexListNode** _node;
#ifdef _DEBUG
  const IndexList* _list;
#endif // _DEBUG

  friend class IndexList;

}; // IndexListIterator


/////////////////////////////////////////////////////////////////////
//
// IndexList: index list class
// =========
class IndexList
{
public:
  using iterator = IndexListIterator;
  using index_type = IndexListNode::value_type;

  IndexList():
    _head{IndexListNode::_null},
    _size{0}
  {
    // do nothing
  }

  IndexList(IndexList&& other) noexcept:
    _head{other._head},
    _size{other._size}
  {
    other._head = IndexListNode::_null;
    other._size = 0;
  }

  ~IndexList()
  {
    clear();
  }

  IndexList& operator =(IndexList&& other) noexcept
  {
    clear();
    _head = other._head;
    other._head = IndexListNode::_null;
    other._size = 0;
    return *this;
  }

  auto add(index_type index)
  {
    _head = new IndexListNode{index, _head};
    return ++_size;
  }

  void clear();

  auto size() const
  {
    return _size;
  }

  iterator begin() const
  {
    return iterator{const_cast<IndexList*>(this)->_head, *this};
  }

  iterator end() const
  {
    return iterator{IndexListNode::_null, *this};
  }

  iterator remove(iterator i);

private:
  IndexListNode* _head;
  size_t _size;

  friend iterator;

}; // IndexList

template <typename T>
inline constexpr bool isIndexList()
{
  return std::is_assignable_v<IndexList, T>;
}

#define ASSERT_INDEX_LIST(T, msg) static_assert(isIndexList<T>(), msg)

} // end namespace cg

#endif // __IndexList_h
