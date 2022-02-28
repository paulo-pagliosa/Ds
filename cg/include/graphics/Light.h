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
// OVERVIEW: Light.h
// ========
// Class definition for light.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

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
  enum class Type
  {
    Directional,
    Point,
    Spot
  };

  enum class Falloff
  {
    Constant,
    Linear,
    Quadratic
  };

  enum LightBits
  {
    Infinite = 1,
    Camera = 2,
    TurnedOn = 4
  };

  static constexpr auto minSpotAngle = 1.0f;
  static constexpr auto maxSpotAngle = 179.0f;

  using LightFlags = Flags<LightBits>;

  LightFlags flags;
  Color color;
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

  const auto& position() const
  {
    return _position;
  }

  void setPosition(const vec3f& value)
  {
    _position = value;
  }

  const auto& direction() const
  {
    return _direction;
  }

  const auto& eulerAngles() const
  {
    return _eulerAngles;
  }

  void setEulerAngles(const vec3f& value);

  const auto& rotation() const
  {
    return _rotation;
  }

  void setRotation(const quatf& value);

  auto range() const
  {
    return _range;
  }

  void setRange(float value);

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
  vec3f _position;
  vec3f _direction;
  vec3f _eulerAngles;
  quatf _rotation;
  float _range;
  float _spotAngle;

  void updateDirection()
  {
    _direction = _rotation * vec3f{0, 0, 1};
  }

  auto outOfRange(float distance) const
  {
    return !flags.isSet(Infinite) && distance > _range;
  }

}; // Light

inline Color
Light::lightColor(float distance) const
{
  if (_type == Type::Directional || falloff == Falloff::Constant)
    return color;

  float f;

  if (flags.isSet(Infinite))
  {
    f = math::inverse(distance);
    if (falloff == Falloff::Quadratic)
      f *= f;
  }
  else
  {
    f = distance / _range;
    f = falloff == Falloff::Quadratic ? 1 + f * (f - 2) : 1 - f;
  }
  return color * f;
}

inline bool
Light::lightVector(const vec3f& P, vec3f& L, float& distance) const
{
  if (_type == Type::Directional)
  {
    L = -_direction.versor();
    return distance = math::Limits<float>::inf();
  }
  distance = (L = _position -  P).length();
  if (math::isZero(distance) || outOfRange(distance))
    return false;
  L *= math::inverse(distance);
  if (_type == Type::Point)
    return true;

  // Spot light
  auto DL = _direction.dot(L);

  return DL < 0 && _spotAngle >= 2 * math::toRadians(acos(DL));
}

} // end namespace cg

#endif // __Light_h
