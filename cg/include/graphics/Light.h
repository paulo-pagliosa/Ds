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
// OVERVIEW: Light.h
// ========
// Class definition for light.
//
// Author: Paulo Pagliosa
// Last revision: 02/06/2019

#ifndef __Light_h
#define __Light_h

#include "core/Flags.h"
#include "core/NameableObject.h"
#include "graphics/Color.h"
#include "math/Quaternion.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Light: light class
// =====
class Light: public NameableObject
{
public:
  enum LightBits
  {
    Linear = 1,
    Squared = 2,
    Falloff = Linear | Squared,
    Directional = 4,
    Spot = 8, // TODO
    Camera = 16,
    TurnedOn = 32
  };

  using LightFlags = Flags<LightBits>;

  LightFlags flags;
  Color color;
  vec3f position;
  vec3f direction;

  // Constructor
  Light();

  bool isDirectional() const
  {
    return flags.isSet(Directional);
  }

  void setDirectional(bool state)
  {
    flags.enable(Directional, state);
  }

  bool isTurnedOn() const
  {
    return flags.isSet(TurnedOn);
  }

  void setSwitch(bool state)
  {
    flags.enable(TurnedOn, state);
  }

  /// Returns the light color at a point.
  Color lightColor(float distance) const;

  /// Returns the incident vector at a point.
  void lightVector(const vec3f& P, vec3f& L, float& distance) const;

}; // Light

inline Color
Light::lightColor(float distance) const
{
  if (flags.test(Light::Directional))
    return color;
  if (flags.test(Light::Falloff) == false)
    return color;

  float f = math::inverse(distance);

  if (flags.isSet(Light::Squared))
    f *= f;
  return color * f;
}

inline void
Light::lightVector(const vec3f& P, vec3f& L, float& distance) const
{
  if (flags.test(Light::Directional))
  {
    L = direction;
    distance = math::Limits<float>::inf();
  }
  else if (!math::isZero(distance = (L = P - position).length()))
    L *= math::inverse(distance);
}

} // end namespace cg

#endif // __Light_h
