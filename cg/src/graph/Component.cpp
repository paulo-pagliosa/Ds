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
// OVERVIEW: Component.cpp
// ========
// Source file for scene object component.
//
// Author: Paulo Pagliosa
// Last revision: 28/06/2023

#include "graph/Component.h"

namespace cg::graph
{ // begin namespace cg::graph


/////////////////////////////////////////////////////////////////////
//
// Component implementation
// =========
bool
Component::canAdd(Component* other) const
{
  return other != this && other->_typeName != _typeName;
}

void
Component::afterAdded()
{
  // do nothing
}

void
Component::beforeRemoved()
{
  // do nothing
}

void
Component::transformChanged()
{
  // do nothing
}

void
Component::setVisible(bool)
{
  // do nothing
}

bool
Component::tryConnectingTo(Component*)
{
  return false;
}

bool
Component::tryDisconnectingFrom(Component*)
{
  return false;
}

} // end namespace cg::graph
