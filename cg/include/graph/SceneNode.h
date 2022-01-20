//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Orthrus Group.                         |
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
// Last revision: 17/01/2022

#ifndef __SceneNode_h
#define __SceneNode_h

#include "core/NameableObject.h"

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph


/////////////////////////////////////////////////////////////////////
//
// SceneNode: generic scene node class
// =========
class SceneNode: public virtual NameableObject
{
public:
  using NameableObject::NameableObject;

  template <typename T>
  auto as()
  {
    return dynamic_cast<T*>(this);
  }

}; // SceneNode

} // end namespace graph

} // end namespace cg

#endif // __SceneNode_h
