//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2012 Orthrus Group.                         |
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
// Last revision: 22/01/2022

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
  enum Type
  {
    Directional,
    Point,
    Spot
  };

  enum Falloff
  {
    Constant,
    Linear,
    Quadratic
  };

  enum LightBits
  {
    Camera = 1,
    TurnedOn = 2
  };

  static constexpr auto minSpotAngle = float(1);
  static constexpr auto maxSpotAngle = float(179);

  using LightFlags = Flags<LightBits>;

  LightFlags flags;
  Color color;
  vec3f position;
  vec3f direction;
  Falloff falloff;

  // Constructor
  Light();

  auto type() const
  {
    return _type;
  }

  void setType(Type type)
  {
    _type = type;
  }

  bool isTurnedOn() const
  {
    return flags.isSet(TurnedOn);
  }

  void turnOn(bool state = true)
  {
    flags.enable(TurnedOn, state);
  }

  auto spotAngle() const
  {
    return _spotAngle;
  }

  void setSpotAngle(float value);

  /// Returns the light color at a point.
  Color lightColor(float distance) const;

  /**
   *  Computes the light incident vector at the point P.
   *  Returns true if the light incides at P.
   */ 
  bool lightVector(const vec3f& P, vec3f& L, float& distance) const;

private:
  Type _type;
  float _spotAngle;

}; // Light

inline Color
Light::lightColor(float distance) const
{
  if (_type == Type::Directional)
    return color;
  if (falloff == Falloff::Constant)
    return color;

  float f = math::inverse(distance);

  if (falloff == Falloff::Quadratic)
    f *= f;
  return color * f;
}

inline bool
Light::lightVector(const vec3f& P, vec3f& L, float& distance) const
{
  if (_type == Light::Directional)
  {
    L = direction;
    distance = math::Limits<float>::inf();
    return true;
  }
  distance = (L = P - position).length();
  if (!math::isZero(distance))
    return false;
  L *= math::inverse(distance);
  if (_type == Light::Point)
    return true;
  // Spot light
  return _spotAngle >= 2 * math::toRadians(acos(direction.dot(L)));
}

} // end namespace cg

#endif // __Light_h
