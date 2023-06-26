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
// OVERVIEW: SceneNode.h
// ========
// Class definition for generic scene node.
//
// Author: Paulo Pagliosa
// Last revision: 13/06/2023

#ifndef __SceneNode_h
#define __SceneNode_h

#include "core/NameableObject.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// SceneNode: generic scene node class
// =========
class SceneNode
{
public:
  SceneNode() = default;

  SceneNode(NameableObject* object):
    _object{object}
  {
    // do nothing
  }

  auto& operator =(NameableObject* object)
  {
    _object = object;
    return *this;
  }

  auto operator ==(NameableObject* object) const
  {
    return _object == object;
  }

  auto operator !=(NameableObject* object) const
  {
    return _object != object;
  }

  auto operator ->() const
  {
    return _object;
  }

  operator NameableObject*() const
  {
    return _object;
  }

  template <typename T>
  auto as()
  {
    return dynamic_cast<T*>(_object);
  }

private:
  NameableObject* _object{};

}; // SceneNode

} // end namespace cg::graph

#endif // __SceneNode_h
