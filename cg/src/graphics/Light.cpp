//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// OVERVIEW: Light.cpp
// ========
// Source file for light.
//
// Author: Paulo Pagliosa
// Last revision: 27/05/2019

#include "graphics/Light.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Light implementation
// =====
static inline unsigned int
nextLightId()
{
  static unsigned int lid;
  return ++lid;
}

Light::Light()
{
  setName("Light %d", nextLightId());
  flags = TurnedOn;
  color = Color::white;
  position = vec3f::null();
  direction = quatf::eulerAngles(50, -30, 0) * vec3f{0, 0, -1};
}

} // end namespace cg
