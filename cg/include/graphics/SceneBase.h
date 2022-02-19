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
// OVERVIEW: SceneBase.h
// ========
// Class definition for scene base.
//
// Author: Paulo Pagliosa
// Last revision: 18/02/2022

#ifndef __SceneBase_h
#define __SceneBase_h

#include "core/List.h"
#include "graphics/Actor.h"
#include "graphics/Light.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneBase: scene base class
// =========
class SceneBase: public virtual NameableObject
{
public:
  Color backgroundColor{Color::gray};
  Color ambientLight{Color::black};

  auto actorCount() const
  {
    return (int)_actors.size();
  }

  const auto& actors() const
  {
    return _actors;
  }

  auto& actors()
  {
    return _actors;
  }

  auto lightCount() const
  {
    return (int)_lights.size();
  }

  const auto& lights() const
  {
    return _lights;
  }

  auto& lights()
  {
    return _lights;
  }

  auto addActor(Actor* actor)
  {
    if (actor != nullptr)
      _actors.add(actor);
    return actor;
  }

  auto removeActor(Actor* actor)
  {
    return _actors.remove(actor);
  }

  void removeActors()
  {
    _actors.clear();
  }

  auto addLight(Light* light)
  {
    if (light != nullptr)
      _lights.add(light);
    return light;
  }

  auto removeLight(Light* light)
  {
    return _lights.remove(light);
  }

  void removeLights()
  {
    _lights.clear();
  }

  void removeAll()
  {
    removeActors();
    removeLights();
  }

private:
  using Actors = List<Reference<Actor>>;
  using Lights = List<Reference<Light>>;

  Actors _actors;
  Lights _lights;

}; // SceneBase

} // end namespace cg

#endif // __Scene_h
