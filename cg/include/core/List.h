//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// Last revision: 16/02/2019

#ifndef __List_h
#define __List_h

#include "core/BlockAllocable.h"
#include "core/ListBase.h"

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

template <typename T, unsigned size = DflBlockSize>
class List: public ListBase<ListNode<T, size>>
{
public:
  using node_type = ListNode<T, size>;
  using Base = ListBase<node_type>;
  using allocator = typename node_type::allocator;
  using const_iterator = typename Base::const_iterator;

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

  bool remove(const const_iterator& i);

  bool remove(const T& value)
  {
    return remove(find(value));
  }

  void clear();

private:
  const_iterator find(const T& value) const;

}; // List


/////////////////////////////////////////////////////////////////////
//
// List implementation
// ====
template <typename T, unsigned blockSize>
template <typename L>
void
List<T, blockSize>::addList(const L& list)
{
  for (const auto& value : list)
    add(value);
}

template <typename T, unsigned blockSize>
bool
List<T, blockSize>::remove(const const_iterator& i)
{
  if (i == this->end())
    return false;

  auto node = Base::node(i);

  Base::remove(node);
  allocator::destroy(node);
  return true;
}

template <typename T, unsigned blockSize>
void
List<T, blockSize>::clear()
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

template <typename T, unsigned blockSize>
typename List<T, blockSize>::const_iterator
List<T, blockSize>::find(const T& value) const
{
  for (auto node = Base::nextNode(this->head());
    node != this->head();
    node = Base::nextNode(node))
    if (value == **node)
      return {node, this};
  return this->end();
}

} // end namespace cg

#endif // __List_h
