//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2007, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Expression.cpp
// ========
// Source file for simple expression tree.
//
// Author: Paulo Pagliosa
// Last revision: 02/02/2022

#include "Expression.h"

namespace cg::parser
{ // begin namespace cg::parser

const char*
typeName(Expression::Type type)
{
  static const char* tn[]
  {
    "void",
    "integer",
    "float",
    "vec2",
    "vec3",
    "vec4",
    "color"
  };

  return tn[(int)type];
}


/////////////////////////////////////////////////////////////////////
//
// BadCast implementation
// =======
BadCast::BadCast(const Expression& e, Expression::Type type)
{
  constexpr auto maxLen = 64;
  char buffer[maxLen];

  snprintf(buffer,
    maxLen,
    "Cannot convert '%s' to '%s'",
    typeName(e.type()),
    typeName(type));
  _message = buffer;
}


/////////////////////////////////////////////////////////////////////
//
// IllegalOperation implementation
// ================
IllegalOperation::IllegalOperation(char op, const Expression& e)
{
  constexpr auto maxLen = 64;
  char buffer[maxLen];

  snprintf(buffer,
    maxLen,
    "'operator %c(%s)' is undefined",
    op,
    typeName(e.type()));
  _message = buffer;
}

IllegalOperation::IllegalOperation(char op,
  const Expression& e1,
  const Expression& e2)
{
  constexpr auto maxLen = 64;
  char buffer[maxLen];

  snprintf(buffer,
    maxLen,
    "'operator %c(%s,%s)' is undefined",
    op,
    typeName(e1.type()),
    typeName(e2.type()));
  _message = buffer;
}


/////////////////////////////////////////////////////////////////////
//
// Expression implementation
// ==========
Expression::operator int() const
{
  if (_type == Type::Integer)
    return _value.integer;
  throw BadCast{*this, Type::Integer};
}

Expression::operator float() const
{
  if (_type == Type::Float)
    return _value.real;
  if (_type == Type::Integer)
    return (float)_value.integer;
  throw BadCast{*this, Type::Float};
}

Expression::operator vec2f() const
{
  if (_type == Type::Vec2)
    return _value.vec2;
  throw BadCast{*this, Type::Vec2};
}

Expression::operator vec3f() const
{
  if (_type == Type::Vec3)
    return _value.vec3;
  throw BadCast{*this, Type::Vec3};
}

Expression::operator vec4f() const
{
  if (_type == Type::Vec4)
    return _value.vec4;
  throw BadCast{*this, Type::Vec4};
}

Expression::operator Color() const
{
  if (_type == Type::Color)
    return _value.color;
  throw BadCast{*this, Type::Color};
}

Expression
Expression::operator +() const
{
  switch (_type)
  {
    case Type::Integer:
    case Type::Float:
    case Type::Vec2:
    case Type::Vec3:
    case Type::Vec4:
      return *this;
  }
  throw IllegalOperation{'+', *this};
}

Expression
Expression::operator -() const
{
  switch (_type)
  {
    case Type::Integer:
      return -_value.integer;
    case Type::Float:
      return -_value.real;
    case Type::Vec2:
      return -_value.vec2;
    case Type::Vec3:
      return -_value.vec3;
    case Type::Vec4:
      return -_value.vec4;
  }
  throw IllegalOperation{'-', *this};
}

Expression
Expression::operator +(const Expression& rhs) const
{
  switch (_type)
  {
    case Type::Integer:
      if (rhs._type == Type::Integer)
        return _value.integer + rhs._value.integer;
      if (rhs._type == Type::Float)
        return (float)_value.integer + rhs._value.real;
      break;
    case Type::Float:
      if (rhs._type == Type::Integer)
        return _value.real + (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.real + rhs._value.real;
      break;
    case Type::Vec2:
      if (rhs._type == Type::Vec2)
        return _value.vec2 + rhs._value.vec2;
      break;
    case Type::Vec3:
      if (rhs._type == Type::Vec3)
        return _value.vec3 + rhs._value.vec3;
      break;
    case Type::Vec4:
      if (rhs._type == Type::Vec4)
        return _value.vec4 + rhs._value.vec4;
      break;
    case Type::Color:
      if (rhs._type == Type::Color)
        return _value.color + rhs._value.color;
      break;
  }
  throw IllegalOperation{'+', *this, rhs};
}

Expression
Expression::operator -(const Expression& rhs) const
{
  switch (_type)
  {
    case Type::Integer:
      if (rhs._type == Type::Integer)
        return _value.integer - rhs._value.integer;
      if (rhs._type == Type::Float)
        return (float)_value.integer - rhs._value.real;
      break;
    case Type::Float:
      if (rhs._type == Type::Integer)
        return _value.real - (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.real - rhs._value.real;
      break;
    case Type::Vec2:
      if (rhs._type == Type::Vec2)
        return _value.vec2 - rhs._value.vec2;
      break;
    case Type::Vec3:
      if (rhs._type == Type::Vec3)
        return _value.vec3 - rhs._value.vec3;
      break;
    case Type::Vec4:
      if (rhs._type == Type::Vec4)
        return _value.vec4 - rhs._value.vec4;
      break;
    case Type::Color:
      if (rhs._type == Type::Color)
        return _value.color - rhs._value.color;
      break;
  }
  throw IllegalOperation{'-', *this, rhs};
}

Expression
Expression::operator *(const Expression& rhs) const
{
  switch (_type)
  {
    case Type::Integer:
      if (rhs._type == Type::Integer)
        return _value.integer * rhs._value.integer;
      if (rhs._type == Type::Float)
        return (float)_value.integer * rhs._value.real;
      if (rhs._type == Type::Vec2)
        return (float)_value.integer * rhs._value.vec2;
      if (rhs._type == Type::Vec3)
        return (float)_value.integer * rhs._value.vec3;
      if (rhs._type == Type::Vec4)
        return (float)_value.integer * rhs._value.vec4;
      if (rhs._type == Type::Color)
        return (float)_value.integer * rhs._value.color;
      break;
    case Type::Float:
      if (rhs._type == Type::Integer)
        return _value.real * (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.real * rhs._value.real;
      if (rhs._type == Type::Vec2)
        return _value.real * rhs._value.vec2;
      if (rhs._type == Type::Vec3)
        return _value.real * rhs._value.vec3;
      if (rhs._type == Type::Vec4)
        return _value.real * rhs._value.vec4;
      if (rhs._type == Type::Color)
        return _value.real * rhs._value.color;
      break;
    case Type::Vec2:
      if (rhs._type == Type::Integer)
        return _value.vec2 * (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.vec2 * rhs._value.real;
      if (rhs._type == Type::Vec2)
        return _value.vec2 * rhs._value.vec2;
      break;
    case Type::Vec3:
      if (rhs._type == Type::Integer)
        return _value.vec3 * (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.vec3 * rhs._value.real;
      if (rhs._type == Type::Vec3)
        return _value.vec3 * rhs._value.vec3;
    case Type::Vec4:
      if (rhs._type == Type::Integer)
        return _value.vec4 * (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.vec4 * rhs._value.real;
      if (rhs._type == Type::Vec4)
        return _value.vec4 * rhs._value.vec4;
    case Type::Color:
      if (rhs._type == Type::Integer)
        return _value.color * (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.color * rhs._value.real;
      if (rhs._type == Type::Color)
        return _value.color + rhs._value.color;
  }
  throw IllegalOperation{'*', *this, rhs};
}

Expression
Expression::operator /(const Expression& rhs) const
{
  switch (_type)
  {
    case Type::Integer:
      if (rhs._type == Type::Integer)
        return _value.integer / rhs._value.integer;
      if (rhs._type == Type::Float)
        return (float)_value.integer / rhs._value.real;
      if (rhs._type == Type::Vec2)
        return (float)_value.integer * rhs._value.vec2.inverse();
      if (rhs._type == Type::Vec3)
        return (float)_value.integer * rhs._value.vec3.inverse();
      if (rhs._type == Type::Vec4)
        return (float)_value.integer * rhs._value.vec4.inverse();
    case Type::Float:
      if (rhs._type == Type::Integer)
        return _value.real / (float)rhs._value.integer;
      if (rhs._type == Type::Float)
        return _value.real / rhs._value.real;
      if (rhs._type == Type::Vec2)
        return _value.real * rhs._value.vec2.inverse();
      if (rhs._type == Type::Vec3)
        return _value.real * rhs._value.vec3.inverse();
      if (rhs._type == Type::Vec4)
        return _value.real * rhs._value.vec4.inverse();
      break;
    case Type::Vec2:
      if (rhs._type == Type::Integer)
        return _value.vec2 * math::inverse((float)rhs._value.integer);
      if (rhs._type == Type::Float)
        return _value.vec2 * math::inverse(rhs._value.real);
      if (rhs._type == Type::Vec2)
        return _value.vec2 * rhs._value.vec2.inverse();
      break;
    case Type::Vec3:
      if (rhs._type == Type::Integer)
        return _value.vec3 * math::inverse((float)rhs._value.integer);
      if (rhs._type == Type::Float)
        return _value.vec3 * math::inverse(rhs._value.real);
      if (rhs._type == Type::Vec3)
        return _value.vec3 * rhs._value.vec3.inverse();
    case Type::Vec4:
      if (rhs._type == Type::Integer)
        return _value.vec4 * math::inverse((float)rhs._value.integer);
      if (rhs._type == Type::Float)
        return _value.vec4 * math::inverse(rhs._value.real);
      if (rhs._type == Type::Vec4)
        return _value.vec4 * rhs._value.vec4.inverse();
    case Type::Color:
      if (rhs._type == Type::Integer)
        return _value.color * math::inverse((float)rhs._value.integer);
      if (rhs._type == Type::Float)
        return _value.color * math::inverse(rhs._value.real);
  }
  throw IllegalOperation{'/', *this, rhs};
}

} // end namespace cg::parser
