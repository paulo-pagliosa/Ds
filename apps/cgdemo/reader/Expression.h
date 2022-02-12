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
// OVERVIEW: Expression.h
// ========
// Class definition for simple expression.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#ifndef __Expression_h
#define __Expression_h

#include "graphics/Color.h"
#include <string>

namespace cg::parser
{ // begin namespace cg::parser


/////////////////////////////////////////////////////////////////////
//
// Expression: generic expression class
// ==========
class Expression final
{
public:
  class Exception;

  // Expression type
  enum class Type
  {
    Void,
    Integer,
    Float,
    Vec2,
    Vec3,
    Vec4,
    Color
  };

  Expression();
  Expression(int);
  Expression(float);
  Expression(const vec2f&);
  Expression(const vec3f&);
  Expression(const vec4f&);
  Expression(const Color&);

  auto type() const
  {
    return _type;
  }

  operator int() const;
  operator float() const;
  operator vec2f() const;
  operator vec3f() const;
  operator vec4f() const;
  operator Color() const;

  Expression operator +() const;
  Expression operator -() const;
  Expression operator +(const Expression&) const;
  Expression operator -(const Expression&) const;
  Expression operator *(const Expression&) const;
  Expression operator /(const Expression&) const;

private:
  // Expression "variant" value
  union Value
  {
    int integer;
    float real;
    vec2f vec2;
    vec3f vec3;
    vec4f vec4;
    Color color;

    Value()
    {
      // do nothing
    }

  }; // Value

  Type _type;
  Value _value;

}; // Expression

class Expression::Exception
{
public:
  auto message() const
  {
    return _message.c_str();
  }

protected:
  std::string _message;

}; // Expression::Exception

const char* typeName(Expression::Type);


/////////////////////////////////////////////////////////////////////
//
// BadCast: bad cast class
// =======
class BadCast final: public Expression::Exception
{
public:
  BadCast(const Expression&, Expression::Type);

}; // BadCast


/////////////////////////////////////////////////////////////////////
//
// IllegalOperation: illegal operation class
// ================
class IllegalOperation final: public Expression::Exception
{
public:
  IllegalOperation(char, const Expression&);
  IllegalOperation(char, const Expression&, const Expression&);

}; // IllegalOperation


/////////////////////////////////////////////////////////////////////
//
// Expression inline implementation
// ==========
inline
Expression::Expression():
  _type{Type::Void}
{
  // do nothing
}

inline
Expression::Expression(int value):
  _type{Type::Integer}
{
  _value.integer = value;
}

inline
Expression::Expression(float value):
  _type{Type::Float}
{
  _value.real = value;
}

inline
Expression::Expression(const vec2f& value):
  _type{Type::Vec2}
{
  _value.vec2 = value;
}

inline
Expression::Expression(const vec3f& value):
  _type{Type::Vec3}
{
  _value.vec3 = value;
}

inline
Expression::Expression(const vec4f& value):
  _type{Type::Vec4}
{
  _value.vec4 = value;
}

inline
Expression::Expression(const Color& value):
  _type{Type::Color}
{
  _value.color = value;
}

//
// Auxiliary operators
//
inline Expression&
operator +=(Expression& lhs, const Expression& rhs)
{
  return lhs = lhs + rhs;
}

inline Expression&
operator -=(Expression& lhs, const Expression& rhs)
{
  return lhs = lhs - rhs;
}

inline Expression&
operator *=(Expression& lhs, const Expression& rhs)
{
  return lhs = lhs * rhs;
}

inline Expression&
operator /=(Expression& lhs, const Expression& rhs)
{
  return lhs = lhs / rhs;
}

} // end namespace cg::parser

#endif // __Expression_h
