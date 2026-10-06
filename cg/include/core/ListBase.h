//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: ListBase.h
// ========
// Class definition for generic list base.
//
// Author: Paulo Pagliosa
// Last revision: 06/10/2026

#ifndef __ListBase_h
#define __ListBase_h

#ifdef _DEBUG
#include <stdexcept>
#endif

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// ListConstIterator: generic list constant iterator class
// =================
template <typename List>
class ListConstIterator
{
public:
  using const_iterator = ListConstIterator<List>;
  using node_type = List::node_type;
  using node_value_type = node_type::value_type;

  ListConstIterator():
#ifdef _DEBUG
    _list{},
#endif // _DEBUG
    _node{}
  {
    // do nothing
  }

  ListConstIterator(node_type* node, const List* list):
#ifdef _DEBUG
    _list{list},
#endif // _DEBUG
    _node{node}
  {
    (void)list;
  }

  const_iterator& operator ++()
  {
#ifdef _DEBUG
    if (!_list || !_node || _node == _list->head())
      throw std::logic_error{"List iterator not incrementable"};
#endif // _DEBUG
    _node = List::nextNode(_node);
    return *this;
  }

  const_iterator operator ++(int)
  {
    const_iterator temp(*this);

    ++*this;
    return temp;
  }

  const_iterator& operator --()
  {
#ifdef _DEBUG
    if (!_list || !_node || _node == _list->head())
      throw std::logic_error{"List iterator not decrementable"};
#endif // _DEBUG
    _node = List::prevNode(_node);
    return *this;
  }

  const_iterator operator --(int)
  {
    const_iterator temp(*this);

    --*this;
    return temp;
  }

  [[nodiscard]] const node_value_type& operator *() const
  {
#ifdef _DEBUG
    if (!_list || !_node || _node == _list->head())
      throw std::logic_error{"List iterator not dereferencable"};
#endif // _DEBUG
    return **_node;
  }

  [[nodiscard]] auto operator ->() const
  {
    return &(operator *());
  }

  [[nodiscard]] bool operator ==(const const_iterator& other) const
  {
#ifdef _DEBUG
    if (_list != other._list)
      throw std::logic_error{"List iterators incompatible"};
#endif // _DEBUG
    return _node == other._node;
  }

  [[nodiscard]] bool operator !=(const const_iterator& other) const
  {
    return !operator ==(other);
  }

private:
  node_type* _node;
#if _DEBUG
  const List* const _list;
#endif // _DEBUG

  friend List;

}; // ListConstIterator


/////////////////////////////////////////////////////////////////////
//
// ListIterator: generic list iterator class
// ============
template <typename List>
class ListIterator: public ListConstIterator<List>
{
public:
  using iterator = ListIterator<List>;
  using const_iterator = ListConstIterator<List>;
  using node_type = const_iterator::node_type;
  using node_value_type = const_iterator::node_value_type;

  using const_iterator::ListConstIterator;

  iterator& operator ++()
  {
    ++*((const_iterator*)this);
    return *this;
  }

  iterator operator ++(int)
  {
    iterator temp(*this);

    ++*this;
    return temp;
  }

  iterator& operator --()
  {
    --*((const_iterator*)this);
    return *this;
  }

  iterator operator --(int)
  {
    iterator temp(*this);

    --*this;
    return temp;
  }

  [[nodiscard]] node_value_type& operator *() const
  {
    return (node_value_type&)**((const_iterator*)this);
  }

  [[nodiscard]] auto operator ->() const
  {
    return &(operator *());
  }

}; // ListIterator


/////////////////////////////////////////////////////////////////////
//
// ListBase: generic list base class
// ========
template <typename Node> class ListBase;

template <typename Node>
class ListNodeLinks
{
public:
  using value_type = Node;

  [[nodiscard]] const value_type& operator *() const
  {
    return *((const Node*)this);
  }

  [[nodiscard]] value_type& operator *()
  {
    return *((Node*)this);
  }

private:
  Node* _next;
  Node* _prev;

  friend class ListBase<Node>;

}; // ListNodeLinks

template <typename Node>
class ListBase
{
public:
  using node_type = Node;
  using List = ListBase<node_type>;
  using iterator = ListIterator<List>;
  using const_iterator = ListConstIterator<List>;

  ~ListBase()
  {
    Node::free(_head);
  }

  ListBase(const List&) = delete;
  List& operator =(const List&) = delete;

  [[nodiscard]] bool empty() const
  {
    return _head->_next == head();
  }

  [[nodiscard]] auto size() const
  {
    return _count;
  }

  [[nodiscard]] const_iterator begin() const
  {
    return const_iterator{_head->_next, this};
  }

  [[nodiscard]] iterator begin()
  {
    return iterator{_head->_next, this};
  }

  [[nodiscard]] const_iterator cbegin() const
  {
    return begin();
  }

  [[nodiscard]] const_iterator end() const
  {
    return const_iterator{head(), this};
  }

  [[nodiscard]] iterator end()
  {
    return iterator{head(), this};
  }

  [[nodiscard]] const_iterator cend() const
  {
    return end();
  }

protected:
  ListBase()
  {
    _head = Node::allocate();
    setEmpty();
  }

  Node* head() const
  {
    return _head;
  }

  void setEmpty()
  {
    _head->_next = _head->_prev = head();
    _count = 0;
  }
  
  void insert(Node* next, Node* node);
  void remove(Node* const node);

  static Node* nextNode(Node* node)
  {
    return node->_next;
  }

  static Node* prevNode(Node* node)
  {
    return node->_prev;
  }

  static auto node(const const_iterator& i)
  {
    return i._node;
  }

private:
  Node* _head;
  size_t _count;

  friend class ListConstIterator<ListBase<Node>>;

}; // ListBase

template <typename Node>
void
ListBase<Node>::insert(Node* next, Node* node)
{
  next->_prev->_next = node;
  node->_prev = next->_prev;
  next->_prev = node;
  node->_next = next;
  ++_count;
}

template <typename Node>
void
ListBase<Node>::remove(Node* const node)
{
  node->_next->_prev = node->_prev;
  node->_prev->_next = node->_next;
  --_count;
}

} // end namespace cg

#endif // __ListBase_h
