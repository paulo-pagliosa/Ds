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
// OVERVIEW: Scene.h
// ========
// Class definition for graph scene.
//
// Author: Paulo Pagliosa
// Last revision: 03/07/2023

#ifndef __GraphScene_h
#define __GraphScene_h

#include "graph/SceneObject.h"
#include "graphics/SceneBase.h"

namespace cg::graph
{ // begin namespace cg::graph

class SceneObject;


/////////////////////////////////////////////////////////////////////
//
// Scene: graph scene class
// =====
class Scene: public SceneBase
{
public:
  /// Constructs an empty scene.
  static auto New(const char* const name = nullptr)
  {
    return new Scene{name};
  }

  /// Returns the root scene object of this scene.
  const auto root() const
  {
    return &_root;
  }

  auto root()
  {
    return &_root;
  }

protected:
  Scene(const char* name); // implemented in SceneObject.cpp

private:
  SceneObject _root;

  friend SceneObject;

}; // Scene

} // end namespace cg::graph

#endif // __Scene_h
