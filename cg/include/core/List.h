//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: List.h
// ========
// Class definition for generic list.
//
// Author: Paulo Pagliosa
// Last revision: 26/08/2023

#ifndef __List_h
#define __List_h

#include "core/BlockAllocable.h"
#include "core/ListBase.h"
#include "core/SharedObject.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// List: generic list class
// ====
template <typename T, unsigned size>
class ListNode: public ListNodeLinks<ListNode<T, size>>,
  public BlockAllocable<ListNode<T, size>, size>
{
public:
  using value_type = T;

  ListNode(const T& value):
    _value{value}
  {
    // do nothing
  }

  auto& operator *()
  {
    return _value;
  }

private:
  T _value;

}; // ListNode

namespace internal::list
{ // begin namespace internal::list

template <typename T, unsigned size = DflBlockSize>
class List: public ListBase<ListNode<T, size>>
{
public:
  using node_type = ListNode<T, size>;
  using Base = ListBase<node_type>;
  using allocator = node_type::allocator;
  using const_iterator = Base::const_iterator;
  using iterator = Base::iterator;

  List() = default;

  ~List()
  {
    clear();
  }

  void add(const T& value)
  {
    Base::insert(this->head(), allocator::construct(value));
  }

  template <typename L>
  void addList(const L& list);

  template <typename... Args>
  void construct(Args&&... args)
  {
    add(T(std::forward<Args>(args)...));
  }

  bool contains(const T& value) const
  {
    return find(value) != this->end();
  }

  bool remove(const iterator& i);

  bool remove(const T& value)
  {
    return remove(find(value));
  }

  void clear();

private:
  iterator find(const T& value) const;

}; // List

template <typename T, unsigned size>
template <typename L>
void
List<T, size>::addList(const L& list)
{
  for (const auto& value : list)
    add(value);
}

template <typename T, unsigned size>
bool
List<T, size>::remove(const iterator& i)
{
  if (i == this->end())
    return false;

  auto node = Base::node(i);

  Base::remove(node);
  allocator::destroy(node);
  return true;
}

template <typename T, unsigned size>
void
List<T, size>::clear()
{
  auto node = Base::nextNode(this->head());

  while (node != this->head())
  {
    auto temp = node;

    node = Base::nextNode(temp);
    allocator::destroy(temp);
  }
  Base::setEmpty();
}

template <typename T, unsigned size>
typename List<T, size>::iterator
List<T, size>::find(const T& value) const
{
  for (auto node = Base::nextNode(this->head());
    node != this->head();
    node = Base::nextNode(node))
    if (value == **node)
      return {node, this};
  return const_cast<List*>(this)->end();
}

} // end namespace internal::list

template <typename T, unsigned size = DflBlockSize>
class List: public internal::list::List<T, size>
{
  // empty

}; // List

//
// Specializations for shared object list
//
using RefListBase = internal::list::List<Reference<SharedObject>>;
using RefListConstIteratorBase = RefListBase::const_iterator;
using RefListIteratorBase = RefListBase::iterator;

template <typename T>
class ListConstIterator<Reference<T>>: public RefListConstIteratorBase
{
public:
  ListConstIterator& operator ++()
  {
    ++(*(RefListConstIteratorBase*)this);
    return *this;
  }

  ListConstIterator operator ++(int)
  {
    return ListConstIterator{(*(RefListConstIteratorBase*)this)++};
  }

  ListConstIterator& operator --()
  {
    --(*(RefListConstIteratorBase*)this);
    return *this;
  }

  ListConstIterator operator --(int)
  {
    return ListConstIterator{(*(RefListConstIteratorBase*)this)--};
  }

  const T* operator *() const
  {
    return (T*)RefListConstIteratorBase::operator *().get();
  }

  auto operator ->() const
  {
    return operator *();
  }

}; // ListConstIterator

template <typename T>
class ListIterator<Reference<T>>: public RefListIteratorBase
{
public:
  ListIterator& operator ++()
  {
    ++(*(RefListIteratorBase*)this);
    return *this;
  }

  ListIterator operator ++(int)
  {
    return ListIterator{(*(RefListIteratorBase*)this)++};
  }

  ListIterator& operator --()
  {
    --(*(RefListIteratorBase*)this);
    return *this;
  }

  ListIterator operator --(int)
  {
    return ListIterator{(*(RefListIteratorBase*)this)--};
  }

  T* operator *()
  {
    return (T*)RefListIteratorBase::operator *().get();
  }

  auto operator ->()
  {
    return operator *();
  }

}; // ListConstIterator

template <typename T>
class List<Reference<T>>: public RefListBase
{
public:
  using value_type = Reference<T>;
  using iterator = ListIterator<Reference<T>>;
  using const_iterator = ListConstIterator<Reference<T>>;

  void add(const value_type& value)
  {
    RefListBase::add(value.get());
  }

  bool contains(const value_type& value) const
  {
    return RefListBase::contains(value.get());
  }

  bool remove(const iterator& i)
  {
    return RefListBase::remove(i);
  }

  bool remove(const value_type& value)
  {
    return RefListBase::remove(value.get());
  }

  const_iterator begin() const
  {
    return const_iterator{RefListBase::begin()};
  }

  auto cbegin() const
  {
    return begin();
  }

  iterator begin()
  {
    return iterator{RefListBase::begin()};
  }

  const_iterator end() const
  {
    return const_iterator{RefListBase::end()};
  }

  iterator end()
  {
    return iterator{RefListBase::end()};
  }

  auto cend() const
  {
    return end();
  }

}; // List

} // end namespace cg

#endif // __List_h
