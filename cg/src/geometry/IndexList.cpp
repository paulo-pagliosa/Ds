//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2019 Orthrus Group.                         |
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
// OVERVIEW: IndexList.cpp
// ========
// Source file for index list.
//
// Author: Paulo Pagliosa
// Last revision: 21/03/2019

#include "geometry/IndexList.h"

namespace cg
{ // begin namespace cg

namespace internal
{ // begin namespace internal

static IndexListNode _nullObjectIndex;

} // end namespace internal


/////////////////////////////////////////////////////////////////////
//
// IndexList implementation
// =========
IndexListNode* IndexListNode::_null = &internal::_nullObjectIndex;

void
IndexList::clear()
{
  while (_head != IndexListNode::_null)
  {
    auto temp = _head;

    _head = temp->_next;
    delete temp;
  }
  _size = 0;
}

typename IndexList::iterator
IndexList::remove(iterator i)
{
#ifdef _DEBUG
  if (i._list != this)
    throw std::logic_error("Index list: bad iterator");
#endif // _DEBUG
  if (*i._node != IndexListNode::_null)
  {
    auto temp = *i._node;

    *i._node = temp->_next;
    delete temp;
    --_size;
  }
  return i;
}

} // end namespace cg
