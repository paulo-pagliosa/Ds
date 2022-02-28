//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: LightProxy.cpp
// ========
// Source file for light proxy.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

#include "graph/LightProxy.h"
#include "graph/Scene.h"

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph


/////////////////////////////////////////////////////////////////////
//
// LightProxy implementation
// ==========
void
LightProxy::afterAdded()
{
  assert(sceneObject() != nullptr);
  sceneObject()->scene()->addLight(_object);
}

void
LightProxy::beforeRemoved()
{
  assert(sceneObject() != nullptr);
  sceneObject()->scene()->removeLight(_object);
}

void
LightProxy::update()
{
  if (auto t = transform(); t->changed)
  {
    _object->setPosition(t->position());
    _object->setRotation(t->rotation());
  }
}

void
LightProxy::setVisible(bool value)
{
  _object->turnOn(value);
}

} // end namepace graph

} // end namespace cg
