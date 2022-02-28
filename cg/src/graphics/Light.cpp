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
// OVERVIEW: Light.cpp
// ========
// Source file for light.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

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

Light::Light():
  flags{TurnedOn | Infinite},
  color{Color::white},
  falloff{Falloff::Constant},
  _type{Type::Point},
  _position{vec3f::null()},
  _direction{0, 0, 1},
  _eulerAngles{vec3f::null()},
  _rotation{quatf::identity()},
  _range{0},
  _spotAngle{60}
{
  setName("Light %d", nextLightId());
}

void
Light::setEulerAngles(const vec3f& value)
{
  _rotation = quatf::eulerAngles(_eulerAngles = value);
  updateDirection();
}

void
Light::setRotation(const quatf& value)
{
  _eulerAngles = (_rotation = value).eulerAngles();
  updateDirection();
}

void
Light::setRange(float value)
{
  _range = math::max<float>(0, value);
}

void
Light::setSpotAngle(float value)
{
  _spotAngle = math::clamp(value, minSpotAngle, maxSpotAngle);
}

} // end namespace cg
