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
// OVERVIEW: Vector3.h
// ========
// Class definition for 3D vector.
//
// Author: Paulo Pagliosa
// Last revision: 19/08/2026

#ifndef __Vector3_h
#define __Vector3_h

#include "math/Vector2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Vector3: 3D vector class
// =======
template <IsReal R>
class Vector<R, 3>
{
public:
  using type = Vector<R, 3>;
  using value_type = R;
  using vec2 = Vector2<R>;

  R x;
  R y;
  R z;

  /// Default constructor.
  HOST DEVICE
  constexpr Vector() = default;

  /// Constructs a Vector3 from (x, y, z).
  HOST DEVICE
  constexpr Vector(R x, R y, R z = 0)
  {
    set(x, y, z);
  }

  /// Constructs a Vector3 object from (v, z).
  HOST DEVICE
  explicit constexpr Vector(const vec2& v, R z = 0)
  {
    set(v, z);
  }

  /// Constructs a Vector3 from v.
  template <typename T>
  HOST DEVICE
  explicit constexpr Vector(const T& v)
  {
    set(v);
  }

  /// Constructs a Vector3 from v[3].
  HOST DEVICE
  explicit Vector(const R v[])
  {
    assert(v);
    set(v[0], v[1], v[2]);
  }

  /// Sets the coordinates of this object to (x, y, z).
  HOST DEVICE
  constexpr void set(R x, R y, R z = 0)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  /// Sets the coordinates of this object to (v, z).
  HOST DEVICE
  constexpr void set(const vec2& v, R z = 0)
  {
    x = v.x;
    y = v.y;
    this->z = z;
  }

  /// Sets the coordinates of this object to (v, v, v) or v.
  template <typename T>
  HOST DEVICE
  constexpr void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = z = R(v);
    else
      set(R(v.x), R(v.y), R(v.z));
  }

  /// Returns a null vector.
  [[nodiscard]] HOST DEVICE
  static constexpr auto null()
  {
    return type{R(0)};
  }

  /// Returns the up vector.
  [[nodiscard]] HOST DEVICE
  static constexpr auto up()
  {
    return type{R(0), R(1), R(0)};
  }

  /// Returns ths size of this object.
  [[nodiscard]] HOST DEVICE
  constexpr int size() const
  {
    return 3;
  }

  /// Returns true if this object is equal to v.
  [[nodiscard]] HOST DEVICE
  bool equals(const type& v, R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x - v.x, y - v.y, z - v.z, eps);
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
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator -=(const type& v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(R s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  /// Returns a reference to this object *= v.
  HOST DEVICE
  auto& operator *=(const type& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  /// Returns a reference to the i-th coordinate of this object.
  [[nodiscard]] HOST DEVICE
  auto& operator [](int i)
  {
    return (&x)[i];
  }

  /// Returns the i-th coordinate of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int i) const
  {
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
    return type{x + v.x, y + v.y, z + v.z};
  }

  /// Returns this object - v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const type& v) const
  {
    return type{x - v.x, y - v.y, z - v.z};
  }

  /// Returns a vector in the direction opposite to this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -() const
  {
    return type{-x, -y, -z};
  }

  /// Returns the multiplication of this object and s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{x * s, y * s, z * s};
  }

  /// Returns the multiplication of this object and v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& v) const
  {
    return type{x * v.x, y * v.y, z * v.z};
  }

  /// Returns true if this object is null.
  [[nodiscard]] HOST DEVICE
  bool isNull(R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x, y, z, eps);
  }

  /// Returns the squared norm of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto squaredNorm() const
  {
    return math::sqr(x) + math::sqr(y) + math::sqr(z);
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
    return math::max(x, math::max(y, z));
  }

  /// Returns the minimum coordinate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto min() const
  {
    return math::min(x, math::min(y, z));
  }

  /// Returns the inverse of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto inverse() const
  {
    return type{1 / x, 1 / y, 1 / z};
  }

  /// Inverts and returns a reference to this object.
  HOST DEVICE
  auto& invert()
  {
    x = 1 / x;
    y = 1 / y;
    z = 1 / z;
    return *this;
  }

  /// Negates and returns a reference to this object.
  HOST DEVICE
  auto& negate()
  {
    x = -x;
    y = -y;
    z = -z;
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
    return x * v.x + y * v.y + z * v.z;
  }

  /// Returns the dot product of v and w.
  [[nodiscard]] HOST DEVICE
  static constexpr auto dot(const type& v, const type& w)
  {
    return v.dot(w);
  }

  /// Returns the cross product of this object and v.
  [[nodiscard]] HOST DEVICE
  constexpr auto cross(const type& v) const
  {
    const auto cx = y * v.z - z * v.y;
    const auto cy = z * v.x - x * v.z;
    const auto cz = x * v.y - y * v.x;

    return type{cx, cy, cz};
  }

  /// Returns the cross product of v and w.
  [[nodiscard]] HOST DEVICE
  static constexpr auto cross(const type& v, const type& w)
  {
    return v.cross(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%g,%g,%g>\n", s, x, y, z);
  }

}; // Vector3

template <IsReal R> using Vector3 = Vector<R, 3>;

/// Returns the multiplication of s and v.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Vector3<R>& v)
{
  return v * s;
}

namespace math
{ // begin namespace math

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector3<R>
min(const Vector3<R>& a, const Vector3<R>& b)
{
  return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector3<R>
max(const Vector3<R>& a, const Vector3<R>& b)
{
  return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr Vector3<R>
abs(const Vector3<R>& v)
{
  return {abs(v.x), abs(v.y), abs(v.z)};
}

} // end namespace math

using vec3f = Vector3<float>;
using vec3d = Vector3<double>;

} // end namespace cg

#endif // __Vector3_h
