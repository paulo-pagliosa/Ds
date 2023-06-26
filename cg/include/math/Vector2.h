//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: Vector2.h
// ========
// Class definition for 2D vector.
//
// Author: Paulo Pagliosa
// Last revision: 18/06/2023

#ifndef __Vector2_h
#define __Vector2_h

#include "math/Real.h"
#include <concepts>
#include <cstdio>

namespace cg
{ // begin namespace cg

#define ASSERT_REAL(T, msg) static_assert(std::floating_point<T>, msg)

template <typename real, int N> class Vector;


/////////////////////////////////////////////////////////////////////
//
// Vector2: 2D vector class
// =======
template <typename real>
class Vector<real, 2>
{
public:
  ASSERT_REAL(real, "Vector2: floating point type expected");

  using type = Vector<real, 2>;
  using value_type = real;

  real x;
  real y;

  /// Default constructor.
  HOST DEVICE
  Vector()
  {
    // do nothing
  }

  /// Constructs a Vector2 object from (x, y).
  HOST DEVICE
  Vector(real x, real y)
  {
    set(x, y);
  }

  /// Constructs a Vector2 object from v[2].
  HOST DEVICE
  explicit Vector(const real v[])
  {
    set(v);
  }

  /// Constructs a Vector2 object with (v, v) or v.
  template <typename T>
  HOST DEVICE
  explicit Vector(const T& v)
  {
    set(v);
  }

  /// Sets this object to v.
  HOST DEVICE
  void set(const type& v)
  {
    *this = v;
  }

  /// Sets the coordinates of this object to (x, y).
  HOST DEVICE
  void set(real x, real y)
  {
    this->x = x;
    this->y = y;
  }

  /// Sets the coordinates of this object to v[2].
  HOST DEVICE
  void set(const real v[])
  {
    x = v[0];
    y = v[1];
  }

  /// Sets the coordinates of this object to (v, v) or v.
  template <typename T>
  HOST DEVICE
  void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = real(v);
    else
      set(real(v.x), real(v.y));
  }

  template <typename T>
  HOST DEVICE
  auto& operator =(const T& v)
  {
    set(v);
    return *this;
  }

  /// Returns a null vector.
  HOST DEVICE
  static type null()
  {
    return type{real(0)};
  }

  /// Returns ths size of this object.
  HOST DEVICE
  constexpr int size() const
  {
    return 2;
  }

  /// Returns true if this object is equal to v.
  HOST DEVICE
  bool equals(const type& v, real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x - v.x, y - v.y, eps);
  }

  HOST DEVICE
  bool operator ==(const type& v) const
  {
    return equals(v);
  }

  /// Returns true if this object is not equal to v.
  HOST DEVICE
  bool operator !=(const type& v) const
  {
    return !operator ==(v);
  }

  /// Returns a reference to this object += v.
  HOST DEVICE
  auto& operator +=(const type& v)
  {
    x += v.x;
    y += v.y;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator -=(const type& v)
  {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(real s)
  {
    x *= s;
    y *= s;
    return *this;
  }

  /// Returns a reference to this object *= v.
  HOST DEVICE
  auto& operator *=(const type& v)
  {
    x *= v.x;
    y *= v.y;
    return *this;
  }

  /// Returns a reference to the i-th coordinate of this object.
  HOST DEVICE
  auto& operator [](int i)
  {
    return (&x)[i];
  }

  /// Returns the i-th coordinate of this object.
  HOST DEVICE
  const auto& operator [](int i) const
  {
    return (&x)[i];
  }

  /// Returns a pointer to the elements of this object.
  HOST DEVICE
  explicit operator const real*() const
  {
    return &x;
  }

  /// Returns this object + v.
  HOST DEVICE
  type operator +(const type& v) const
  {
    return {x + v.x, y + v.y};
  }

  /// Returns this object - v.
  HOST DEVICE
  type operator -(const type& v) const
  {
    return {x - v.x, y - v.y};
  }

  /// Returns a vector in the direction opposite to this object.
  HOST DEVICE
  type operator -() const
  {
    return {-x, -y};
  }

  /// Returns the scalar multiplication of this object and s.
  HOST DEVICE
  type operator *(real s) const
  {
    return {x * s, y * s};
  }

  /// Returns the multiplication of this object and v.
  HOST DEVICE
  type operator *(const type& v) const
  {
    return {x * v.x, y * v.y};
  }

  /// Returns true if this object is null.
  HOST DEVICE
  bool isNull(real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x, y, eps);
  }

  /// Returns the squared norm of this object.
  HOST DEVICE
  real squaredNorm() const
  {
    return math::sqr(x) + math::sqr(y);
  }

  /// Returns the length of this object.
  HOST DEVICE
  real length() const
  {
    return real(sqrt(squaredNorm()));
  }

  /// Returns the maximum coordinate of this object.
  HOST DEVICE
  real max() const
  {
    return math::max(x, y);
  }

  /// Returns the minimum coordinate of this object.
  HOST DEVICE
  real min() const
  {
    return math::min(x, y);
  }

  /// Returns the inverse of this object.
  HOST DEVICE
  type inverse() const
  {
    return {1 / x, 1 / y};
  }

  /// Inverts and returns a reference to this object.
  HOST DEVICE
  auto& invert()
  {
    x = 1 / x;
    y = 1 / y;
    return *this;
  }

  /// Negates and returns a reference to this object.
  HOST DEVICE
  auto& negate()
  {
    x = -x;
    y = -y;
    return *this;
  }

  /// Normalizes and returns a reference to this object.
  HOST DEVICE
  auto& normalize(real eps = math::Limits<real>::eps())
  {
    const auto len = length();

    if (!math::isZero(len, eps))
      operator *=(math::inverse(len));
    return *this;
  }

  /// Returns the unit vector of this this object.
  HOST DEVICE
  type versor(real eps = math::Limits<real>::eps()) const
  {
    return type{*this}.normalize(eps);
  }

  /// Returns the unit vector of v.
  HOST DEVICE
  static type versor(const type& v, real eps = math::Limits<real>::eps())
  {
    return v.versor(eps);
  }

  /// Returns the dot product of this object and v.
  HOST DEVICE
  real dot(const type& v) const
  {
    return x * v.x + y * v.y;
  }

  /// Returns the dot product of this object and (x, y).
  HOST DEVICE
  real dot(real x, real y) const
  {
    return dot(type{x, y});
  }

  /// Returns the dot product of v and w.
  HOST DEVICE
  static real dot(const type& v, const type& w)
  {
    return v.dot(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%g,%g>\n", s, x, y);
  }

}; // Vector2

template <typename real> using Vector2 = Vector<real, 2>;

/// Returns the scalar multiplication of s and v.
template <typename real>
HOST DEVICE inline auto
operator *(real s, const Vector2<real>& v)
{
  return v * s;
}

namespace math
{ // begin namespace math

template <typename real>
inline Vector2<real>
min(const Vector2<real>& a, const Vector2<real>& b)
{
  return {math::min(a.x, b.x), math::min(a.y, b.y)};
}

template <typename real>
inline Vector2<real>
max(const Vector2<real>& a, const Vector2<real>& b)
{
  return {math::max(a.x, b.x), math::max(a.y, b.y)};
}

} // end namespace math

using vec2f = Vector2<float>;
using vec2d = Vector2<double>;

} // end namespace cg

#endif // __Vector2_h
