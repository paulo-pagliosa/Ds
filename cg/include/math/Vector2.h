//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2026 Paulo Pagliosa.                        |
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
// Last revision: 24/08/2026

#ifndef __Vector2_h
#define __Vector2_h

#include "math/Real.h"
#include <cassert>
#include <cstdio>

namespace cg
{ // begin namespace cg

template <IsReal R, int N> class Vector;


/////////////////////////////////////////////////////////////////////
//
// Vector2: 2D vector class
// =======
template <IsReal R>
class Vector<R, 2>
{
public:
  using type = Vector<R, 2>;
  using value_type = R;

  R x;
  R y;

  /// Default constructor.
  constexpr Vector() = default;

  /// Constructs a Vector2 from (x, y).
  HOST DEVICE
  constexpr Vector(R x, R y)
  {
    set(x, y);
  }

  /// Constructs a Vector2 from v.
  template <typename T>
  HOST DEVICE
  explicit constexpr Vector(const T& v)
  {
    set(v);
  }

  /// Constructs a Vector2 from v[2].
  HOST DEVICE
  explicit Vector(const R v[])
  {
    assert(v);
    set(v[0], v[1]);
  }

  /// Sets the coordinates of this object to (x, y).
  HOST DEVICE
  constexpr void set(R x, R y)
  {
    this->x = x;
    this->y = y;
  }

  /// Sets the coordinates of this object to (v, v) or v.
  template <typename T>
  HOST DEVICE
  constexpr void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = R(v);
    else
      set(R(v.x), R(v.y));
  }

  /// Returns a null vector.
  [[nodiscard]] HOST DEVICE
  static auto null()
  {
    return type{R(0)};
  }

  /// Returns ths size of this object.
  [[nodiscard]] HOST DEVICE
  constexpr int size() const
  {
    return 2;
  }

  /// Returns true if this object is equal to v.
  [[nodiscard]] HOST DEVICE
  bool equals(const type& v, R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x - v.x, y - v.y, eps);
  }

  [[nodiscard]] HOST DEVICE
  bool operator ==(const type& v) const
  {
    return equals(v);
  }

  /// Returns true if this object is not equal to v.
  [[nodiscard]] HOST DEVICE
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
  auto& operator *=(R s)
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
  [[nodiscard]] HOST DEVICE
  auto& operator [](int i)
  {
    assert(i >= 0 && i < 2);
    return (&x)[i];
  }

  /// Returns the i-th coordinate of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int i) const
  {
    assert(i >= 0 && i < 2);
    return (&x)[i];
  }

  /// Returns a pointer to the elements of this object.
  [[nodiscard]] HOST DEVICE
  explicit operator const R*() const
  {
    return &x;
  }

  /// Returns this object + v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const type& v) const
  {
    return type{x + v.x, y + v.y};
  }

  /// Returns this object - v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const type& v) const
  {
    return type{x - v.x, y - v.y};
  }

  /// Returns a vector in the direction opposite to this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -() const
  {
    return type{-x, -y};
  }

  /// Returns the multiplication of this object and s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{x * s, y * s};
  }

  /// Returns the multiplication of this object and v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& v) const
  {
    return type{x * v.x, y * v.y};
  }

  /// Returns true if this object is null.
  [[nodiscard]] HOST DEVICE
  bool isNull(R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x, y, eps);
  }

  /// Returns the squared norm of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto squaredNorm() const
  {
    return math::sqr(x) + math::sqr(y);
  }

  /// Returns the length of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto length() const
  {
    return R(sqrt(squaredNorm()));
  }

  /// Returns the maximum coordinate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto max() const
  {
    return math::max(x, y);
  }

  /// Returns the minimum coordinate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto min() const
  {
    return math::min(x, y);
  }

  /// Returns the inverse of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto inverse() const
  {
    return type{1 / x, 1 / y};
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
  auto& normalize(R eps = math::Limits<R>::eps())
  {
    const auto s = length();

    if (!math::isZero(s, eps))
      operator *=(math::inverse(s));
    return *this;
  }

  /// Returns the unit vector of this this object.
  [[nodiscard]] HOST DEVICE
  auto versor(R eps = math::Limits<R>::eps()) const
  {
    return type{*this}.normalize(eps);
  }

  /// Returns the unit vector of v.
  [[nodiscard]] HOST DEVICE
  static auto versor(const type& v, R eps = math::Limits<R>::eps())
  {
    return v.versor(eps);
  }

  /// Returns the dot product of this object and v.
  [[nodiscard]] HOST DEVICE
  constexpr auto dot(const type& v) const
  {
    return x * v.x + y * v.y;
  }

  /// Returns the dot product of v and w.
  [[nodiscard]] HOST DEVICE
  static constexpr auto dot(const type& v, const type& w)
  {
    return v.dot(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%g,%g>\n", s, x, y);
  }

}; // Vector2

template <IsReal R> using Vector2 = Vector<R, 2>;

/// Returns the multiplication of s and v.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Vector2<R>& v)
{
  return v * s;
}

namespace math
{ // begin namespace math

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector2<R>
min(const Vector2<R>& a, const Vector2<R>& b)
{
  return {min(a.x, b.x), min(a.y, b.y)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector2<R>
max(const Vector2<R>& a, const Vector2<R>& b)
{
  return {max(a.x, b.x), max(a.y, b.y)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector2<R>
abs(const Vector2<R>& v)
{
  return {abs(v.x), abs(v.y)};
}

} // end namespace math

using vec2f = Vector2<float>;
using vec2d = Vector2<double>;

} // end namespace cg

#endif // __Vector2_h
