//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: Vector4.h
// ========
// Class definition for 4D vector.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Vector4_h
#define __Vector4_h

#include "math/Vector3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Vector4: 4D vector class
// =======
template <IsReal R>
class Vector<R, 4>
{
public:
  using type = Vector<R, 4>;
  using value_type = R;
  using vec3 = Vector3<R>;

  R x;
  R y;
  R z;
  R w;

  /// Default constructor.
  constexpr Vector() = default;

  /// Constructs a Vector4 from (x, y, z, w).
  HOST DEVICE
  constexpr Vector(R x, R y, R z, R w = 0)
  {
    set(x, y, z, w);
  }

  /// Constructs a Vector4 from (v, w).
  HOST DEVICE
  explicit constexpr Vector(const vec3& v, R w = 0)
  {
    set(v, w);
  }

  /// Constructs a Vector4 from v.
  template <typename T>
  HOST DEVICE
  explicit constexpr Vector(const T& v)
  {
    set(v);
  }

  /// Constructs a Vector4 from v[4].
  HOST DEVICE
  explicit Vector(const R v[])
  {
    assert(v);
    set(v[0], v[1], v[2], v[3]);
  }

  /// Sets the coordinates of this object to (x, y, z, w).
  HOST DEVICE
  constexpr void set(R x, R y, R z, R w = 0)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  /// Sets the coordinates of this object to (v, w).
  HOST DEVICE
  constexpr void set(const vec3& v, R w = 0)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
  }

  /// Sets the coordinates of this object to (v, v, v, v) or v.
  template <typename T>
  HOST DEVICE
  constexpr void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = z = w = R(v);
    else
      set(R(v.x), R(v.y), R(v.z), R(v.w));
  }


  /// Returns a null vector.
  [[nodiscard]] HOST DEVICE
  static constexpr auto null()
  {
    return type(R(0));
  }

  /// Returns ths size of this object.
  [[nodiscard]] HOST DEVICE
  constexpr int size() const
  {
    return 4;
  }

  /// Returns true if this object is equal to v.
  [[nodiscard]] HOST DEVICE
  bool equals(const type& v, R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x - v.x, y - v.y, z - v.z, w - v.w, eps);
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
    z += v.z;
    w += v.w;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator -=(const type& v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(R s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /// Returns a reference to this object *= v.
  HOST DEVICE
  auto& operator *=(const type& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
  }

  /// Returns a reference to the i-th coordinate of this object.
  [[nodiscard]] HOST DEVICE
  auto& operator [](int i)
  {
    assert(i >= 0 && i < 4);
    return (&x)[i];
  }

  /// Returns the i-th coordinate of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int i) const
  {
    assert(i >= 0 && i < 4);
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
    return type{x + v.x, y + v.y, z + v.z, w + v.w};
  }

  /// Returns this object - v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const type& v) const
  {
    return type{x - v.x, y - v.y, z - v.z, w - v.w};
  }

  /// Returns a vector in the direction opposite to this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -() const
  {
    return type{-x, -y, -z, -w};
  }

  /// Returns the scalar multiplication of this object and s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{x * s, y * s, z * s, w * s};
  }

  /// Returns the multiplication of this object and v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& v) const
  {
    return type{x * v.x, y * v.y, z * v.z, w * v.w};
  }

  /// Returns true if this object is null.
  [[nodiscard]] HOST DEVICE
  bool isNull(R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x, y, z, w, eps);
  }

  /// Returns the squared norm of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto squaredNorm() const
  {
    return math::sqr(x) + math::sqr(y) + math::sqr(z) + math::sqr(w);
  }

  /// Returns the length of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto length() const
  {
    return sqrt(squaredNorm());
  }

  /// Returns the maximum coordinate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto max() const
  {
    return math::max(x, math::max(y, math::max(z, w)));
  }

  /// Returns the minimum coordinate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto min() const
  {
    return math::min(x, math::min(y, math::min(z, w)));
  }

  /// Returns the inverse of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto inverse() const
  {
    return type{1 / x, 1 / y, 1 / z, 1 / w};
  }

  /// Inverts and returns a reference to this object.
  HOST DEVICE
  auto& invert()
  {
    x = 1 / x;
    y = 1 / y;
    z = 1 / z;
    w = 1 / w;
    return *this;
  }

  /// Negates and returns a reference to this object.
  HOST DEVICE
  auto& negate()
  {
    x = -x;
    y = -y;
    z = -z;
    w = -w;
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
    return x * v.x + y * v.y + z * v.z + w * v.w;
  }

  /// Returns the dot product of v and w.
  [[nodiscard]] HOST DEVICE
  static auto dot(const type& v, const type& w)
  {
    return v.dot(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%g,%g,%g,%g>\n", s, x, y, z, w);
  }

}; // Vector4

template <IsReal R> using Vector4 = Vector<R, 4>;

/// Returns the scalar multiplication of s and v.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Vector4<R>& v)
{
  return v * s;
}

namespace math
{ // begin namespace math

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector4<R>
min(const Vector4<R>& a, const Vector4<R>& b)
{
  return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector4<R>
max(const Vector4<R>& a, const Vector4<R>& b)
{
  return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector4<R>
abs(const Vector4<R>& v)
{
  return {abs(v.x), abs(v.y), abs(v.z), abs(v.w)};
}

} // end namespace math

using vec4f = Vector4<float>;
using vec4d = Vector4<double>;

} // end namespace cg

#endif // __Vector4_h
