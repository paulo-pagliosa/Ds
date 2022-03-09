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
// OVERVIEW: Scene.h
// ========
// Class definition for vis scene.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#ifndef __VisScene_h
#define __VisScene_h

#include "graphics/SceneBase.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Scene: vis scene class
// =====
class Scene: public SceneBase
{
public:
  /// Constructs an empty scene.
  static auto New(const char* name = "")
  {
    assert(name != nullptr);
    return new Scene{name};
  }

  void addActor(Actor* actor, const char* name = "");
  void addLight(Light* light, const char* name = "");

  Bounds3f computeBounds() const;

private:
  int _nextActorId{};
  int _nextLightId{};

  Scene(const char*);

}; // Scene

} // end namespace cg::vis

#endif // __VisScene_h
