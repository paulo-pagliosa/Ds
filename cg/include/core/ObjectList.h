//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2020 Paulo Pagliosa.                        |
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
// OVERVIEW: ObjectList.h
// ========
// Class definition for generic object list.
//
// Author: Paulo Pagliosa
// Last revision: 26/08/2020

#ifndef __ObjectList_h
#define __ObjectList_h

#include "core/BlockAllocable.h"
#include "core/ListBase.h"
#include "core/SharedObject.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// ObjectList: generic object list class
// ==========
template <typename Node>
class ObjectList: public ListBase<Node>
{
public:
  using Base = ListBase<Node>;

  ObjectList() = default;

  ~ObjectList()
  {
    clear();
  }

  Node* insert(Node* node);
  ObjectList<Node>& insert(ObjectList<Node>& list);

  template <typename... Args>
  Node* construct(Args&&... args)
  {
    return insert(new Node(std::forward<Args>(args)...));
  }

  bool contains(const Node*) const;

  bool remove(Node* const node);
  void clear();

  bool destroy(Node* node)
  {
    return remove(node) ? (deleteNode(node), true) : false;
  }

private:
  static void deleteNode(Node* node)
  {
    if constexpr (isSharedObject<Node>())
      SharedObject::release(node);
    else
      delete node;
  }

}; // ObjectList


/////////////////////////////////////////////////////////////////////
//
// ObjectListNode: object list node class
// ==============
template <typename T, typename Allocator = StandardAllocator<T>>
class ObjectListNode: public ListNodeLinks<T>,
  public AllocableObject<T, Allocator>
{
public:
  using node_type = T;
  using list_type = ObjectList<T>;

  ~ObjectListNode()
  {
    if (_container != nullptr)
      _container->remove(static_cast<node_type*>(this));
  }

  const list_type* container() const
  {
    return _container;
  }

private:
  list_type* _container{};

  friend list_type;

}; // ObjectListNode


/////////////////////////////////////////////////////////////////////
//
// ObjectList implementation
// ==========
template <typename Node>
Node*
ObjectList<Node>::insert(Node* node)
{
  if (node == nullptr)
    return nullptr;
  // If the node already belongs to this container,
  // then we return true.
  if (node->_container == this)
    return node;
  // If the node belongs to another container, then
  // we remove it from its owner.
  if (node->_container != nullptr)
    node->_container->Base::remove(node);
  // Next, we insert the node in this container and
  // set this as its owner.
  Base::insert(this->head(), node);
  node->_container = this;
  return node;
}

template<typename Node>
ObjectList<Node>&
ObjectList<Node>::insert(ObjectList<Node>& list)
{
  if (&list != this)
    while (!list.empty())
      insert(Base::nextNode(list.head()));
  return *this;
}

template<typename Node>
inline bool
ObjectList<Node>::contains(const Node* node) const
{
  return node != nullptr && node->_container == this;
}

template<typename Node>
bool
ObjectList<Node>::remove(Node* const node)
{
  if (node == nullptr || node->_container != this)
    return false;
  Base::remove(node);
  node->_container = nullptr;
  return true;
}

template<typename Node>
void
ObjectList<Node>::clear()
{
  auto node = Base::nextNode(this->head());

  while (node != this->head())
  {
    auto temp = node;

    node = Base::nextNode(temp);
    temp->_container = nullptr;
    deleteNode(temp);
  }
  Base::setEmpty();
}

} // end namespace cg

#endif // __ObjectList_h
