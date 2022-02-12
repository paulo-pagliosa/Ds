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
// OVERVIEW: PrimitiveProxy.cpp
// ========
// Source file for primitive proxy.
//
// Author: Paulo Pagliosa
// Last revision: 24/01/2022

#include "graph/PrimitiveProxy.h"
#include "graph/Scene.h"

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph


/////////////////////////////////////////////////////////////////////
//
// PrimitiveProxy implementation
// ==============
void
PrimitiveProxy::afterAdded()
{
  assert(sceneObject() != nullptr);
  _actor = new Actor{*_object};
  sceneObject()->scene()->addActor(_actor);
}

void
PrimitiveProxy::beforeRemoved()
{
  if (_actor != nullptr)
  {
    assert(sceneObject() != nullptr);
    sceneObject()->scene()->removeActor(_actor);
    _actor = nullptr;
  }
}

void
PrimitiveProxy::update()
{
  if (auto t = transform(); t->changed)
    if (auto p = _object->primitive())
      p->setTransform(t->localToWorldMatrix(), t->worldToLocalMatrix());
}

void
PrimitiveProxy::setVisible(bool value)
{
  if (_actor != nullptr)
    _actor->visible = value;
}

} // end namepace graph

} // end namespace cg
