//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Scene.cpp
// ========
// Source file for vis scene.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#include "Mapper.h"
#include "Scene.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Scene implementation
// =====
Scene::Scene(const char* name)
{
  if (*name)
    setName(name);
  else
  {
    static int nextSceneId;
    setName("Scene %d", ++nextSceneId);
  }
}

void
Scene::addActor(Actor* actor, const char* name)
{
  assert(name != nullptr);
  if (SceneBase::addActor(actor) != nullptr)
    if (*name)
      actor->setName(name);
    else
      actor->setName("Actor %d", ++_nextActorId);
}

void
Scene::addLight(Light* light, const char* name)
{
  assert(name != nullptr);
  if (SceneBase::addLight(light) != nullptr)
    if (*name)
      light->setName(name);
    else
      light->setName("Light %d", ++_nextLightId);
}

Bounds3f
Scene::computeBounds() const
{
  Bounds3f bounds;

  for (auto& actor : actors())
    if (auto mapper = asAbstractMapper(actor->mapper()))
      bounds.inflate(mapper->bounds());
  return bounds;
}

} // end namespace cg::vis
