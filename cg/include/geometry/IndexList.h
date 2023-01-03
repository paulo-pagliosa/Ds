//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// Last revision: 31/12/2022

#ifndef __IndexList_h
#define __IndexList_h

#include "core/BlockAllocable.h"
#include "core/ContentHolder.h"

namespace cg
{ // begin namespace cg

template <typename T> class IndexList;
template <typename T> class IndexListIterator;

template <typename T>
inline constexpr bool
isSignedInt()
{
  return std::is_integral_v<T> && std::is_signed_v<T>;
}

#define ASSERT_SIGNED(T, msg) static_assert(isSignedInt<T>(), msg)


/////////////////////////////////////////////////////////////////////
//
// IndexListNode: index list node class
// =============
template <typename T>
class IndexListNode: public BlockAllocable<IndexListNode<T>, 1024>
{
public:
  IndexListNode() = default;

private:
  T _index{-1};
  IndexListNode<T>* _next{};

  IndexListNode(T index, IndexListNode<T>* next):
    _index{index}, _next{next}
  {
    // do nothing
  }

  static IndexListNode<T>* null;

  friend IndexList<T>;
  friend IndexListIterator<T>;

}; // IndexListNode

namespace il
{ // begin namespace il

template <typename T>
inline static IndexListNode<T> nullIndex;

} // end namespace il

template <typename T>
inline IndexListNode<T>* IndexListNode<T>::null = &il::nullIndex<T>;


/////////////////////////////////////////////////////////////////////
//
// IndexListIterator: index list iterator class
// =================
template <typename T>
class IndexListIterator
{
public:
  using value_type = T;
  using list = IndexList<T>;
  using iterator = IndexListIterator<T>;

  const value_type& operator *() const
  {
#ifdef _DEBUG
    if (_list == nullptr || *_node == IndexListNode<T>::null)
      throw std::logic_error("IndexList: iterator not dereferencable");
#endif // _DEBUG
    return (*_node)->_index;
  }

  iterator& operator ++()
  {
#ifdef _DEBUG
    if (_list == nullptr || *_node == IndexListNode<T>::null)
      throw std::logic_error("IndexList: iterator not incrementable");
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

  bool operator ==(const iterator& other) const
  {
#ifdef _DEBUG
    if (_list != other._list)
      throw std::logic_error("IndexList: iterators incompatible");
#endif // _DEBUG
    return *_node == *other._node;
  }

  bool operator !=(const iterator& other) const
  {
    return !operator ==(other);
  }

private:
  IndexListNode<T>** _node;
#ifdef _DEBUG
  const list* _list;
#endif // _DEBUG

  IndexListIterator(IndexListNode<T>*& node, const list& list):
#ifdef _DEBUG
    _list{&list},
#endif // _DEBUG
    _node{&node}
  {
    (void)list;
  }

  friend list;

}; // IndexListIterator


/////////////////////////////////////////////////////////////////////
//
// IndexList: index list class
// =========
class IndexListBase
{
protected:
  IndexListBase() = default;

}; // IndexListBase

template <typename T = int>
class IndexList: public IndexListBase
{
public:
  ASSERT_SIGNED(T, "IndexList: signed integral type expected");

  using value_type = T;
  using list = IndexList<T>;
  using iterator = IndexListIterator<T>;

  IndexList():
    _head{IndexListNode<T>::null},
    _size{0}
  {
    // do nothing
  }

  IndexList(list&& other) noexcept:
    _head{other._head},
    _size{other._size}
  {
    other._head = IndexListNode<T>::null;
    other._size = 0;
  }

  ~IndexList()
  {
    clear();
  }

  list& operator =(list&& other) noexcept
  {
    clear();
    _head = other._head;
    other._head = IndexListNode<T>::null;
    other._size = 0;
    return *this;
  }

  auto add(value_type index)
  {
    _head = new IndexListNode<T>{index, _head};
    return ++_size;
  }

  void clear();

  auto size() const
  {
    return _size;
  }

  bool empty() const
  {
    return _size == 0;
  }

  iterator begin() const
  {
    return iterator{const_cast<list*>(this)->_head, *this};
  }

  iterator end() const
  {
    return iterator{IndexListNode<T>::null, *this};
  }

  iterator remove(iterator i);

  auto removeFront()
  {
    return remove(begin());
  }

private:
  IndexListNode<T>* _head;
  size_t _size;

  friend iterator;

}; // IndexList

template <typename T>
void
IndexList<T>::clear()
{
  while (_head != IndexListNode<T>::null)
  {
    auto temp = _head;

    _head = temp->_next;
    delete temp;
  }
  _size = 0;
}

template <typename T>
typename IndexList<T>::iterator
IndexList<T>::remove(iterator i)
{
#ifdef _DEBUG
  if (i._list != this)
    throw std::logic_error("IndexList: bad iterator");
#endif // _DEBUG
  if (*i._node != IndexListNode<T>::null)
  {
    auto temp = *i._node;

    *i._node = temp->_next;
    delete temp;
    --_size;
  }
  return i;
}

template <typename T>
inline constexpr bool
isIndexList()
{
  return std::is_assignable_v<IndexListBase, T>;
}

#define ASSERT_INDEX_LIST(T, msg) static_assert(isIndexList<T>(), msg)

} // end namespace cg

#endif // __IndexList_h
