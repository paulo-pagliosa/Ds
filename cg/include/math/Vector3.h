//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// Last revision: 15/12/2022

#ifndef __Vector3_h
#define __Vector3_h

#include "math/Vector2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Vector3: 3D vector class
// =======
template <typename real>
class Vector<real, 3>
{
public:
  ASSERT_REAL(real, "Vector3: floating point type expected");

  using type = Vector<real, 3>;
  using value_type = real;
  using vec2 = Vector2<real>;

  real x;
  real y;
  real z;

  /// Default constructor.
  HOST DEVICE
  Vector()
  {
    // do nothing
  }

  /// Constructs a Vector3 object from (x, y, z).
  HOST DEVICE
  Vector(real x, real y, real z = 0)
  {
    set(x, y, z);
  }

  /// Constructs a Vector3 object from v[3].
  HOST DEVICE
  explicit Vector(const real v[])
  {
    set(v);
  }

  /// Constructs a Vector3 object from (v, z).
  HOST DEVICE
  explicit Vector(const vec2& v, real z = 0)
  {
    set(v, z);
  }

  /// Constructs a Vector3 object with (v, v, v) or v.
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

  /// Sets the coordinates of this object to (x, y, z).
  HOST DEVICE
  void set(real x, real y, real z = 0)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  /// Sets the coordinates of this object to v[3].
  HOST DEVICE
  void set(const real v[])
  {
    x = v[0];
    y = v[1];
    z = v[2];
  }

  /// Sets the coordinates of this object to (v, z).
  HOST DEVICE
  void set(const vec2& v, real z = 0)
  {
    x = v.x;
    y = v.y;
    this->z = z;
  }

  /// Sets the coordinates of this object to (v, v, v) or v.
  template <typename T>
  HOST DEVICE
  void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = z = real(v);
    else
      set(real(v.x), real(v.y), real(v.z));
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

  /// Returns the up vector.
  HOST DEVICE
  static type up()
  {
    return {real(0), real(1), real(0)};
  }

  /// Returns ths size of this object.
  HOST DEVICE
  constexpr int size() const
  {
    return 3;
  }

  /// Returns true if this object is equal to v.
  HOST DEVICE
  bool equals(const type& v, real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x - v.x, y - v.y, z - v.z, eps);
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
  auto& operator *=(real s)
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
  HOST DEVICE
  real& operator [](int i)
  {
    return (&x)[i];
  }

  /// Returns the i-th coordinate of this object.
  HOST DEVICE
  const real& operator [](int i) const
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
    return {x + v.x, y + v.y, z + v.z};
  }

  /// Returns this object - v.
  HOST DEVICE
  type operator -(const type& v) const
  {
    return {x - v.x, y - v.y, z - v.z};
  }

  /// Returns a vector in the direction opposite to this object.
  HOST DEVICE
  type operator -() const
  {
    return {-x, -y, -z};
  }

  /// Returns the scalar multiplication of this object and s.
  HOST DEVICE
  type operator *(real s) const
  {
    return {x * s, y * s, z * s};
  }

  /// Returns the multiplication of this object and v.
  HOST DEVICE
  type operator *(const type& v) const
  {
    return {x * v.x, y * v.y, z * v.z};
  }

  /// Returns true if this object is null.
  HOST DEVICE
  bool isNull(real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x, y, z, eps);
  }

  /// Returns the squared norm of this object.
  HOST DEVICE
  real squaredNorm() const
  {
    return math::sqr(x) + math::sqr(y) + math::sqr(z);
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
    return math::max(x, math::max(y, z));
  }

  /// Returns the minimum coordinate of this object.
  HOST DEVICE
  real min() const
  {
    return math::min(x, math::min(y, z));
  }

  /// Returns the inverse of this object.
  HOST DEVICE
  type inverse() const
  {
    return {1 / x, 1 / y, 1 / z};
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
    return x * v.x + y * v.y + z * v.z;
  }

  /// Returns the dot product of this object and (x, y, z).
  HOST DEVICE
  real dot(real x, real y, real z) const
  {
    return dot({x, y, z});
  }

  /// Returns the dot product of v and w.
  HOST DEVICE
  static real dot(const type& v, const type& w)
  {
    return v.dot(w);
  }

  /// Returns the cross product of this object and v.
  HOST DEVICE
  type cross(const type& v) const
  {
    const auto cx = y * v.z - z * v.y;
    const auto cy = z * v.x - x * v.z;
    const auto cz = x * v.y - y * v.x;

    return {cx, cy, cz};
  }

  /// Returns the cross product of this object and (x, y, z).
  HOST DEVICE
  type cross(real x, real y, real z) const
  {
    return cross({x, y, z});
  }

  /// Returns the cross product of v and w.
  HOST DEVICE
  static type cross(const type& v, const type& w)
  {
    return v.cross(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%g,%g,%g>\n", s, x, y, z);
  }

}; // Vector3

template <typename real> using Vector3 = Vector<real, 3>;

/// Returns the scalar multiplication of s and v.
template <typename real>
HOST DEVICE inline auto
operator *(real s, const Vector3<real>& v)
{
  return v * s;
}

namespace math
{ // begin namespace math

template <typename real>
inline Vector3<real>
min(const Vector3<real>& a, const Vector3<real>& b)
{
  return {math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z)};
}

template <typename real>
inline Vector3<real>
max(const Vector3<real>& a, const Vector3<real>& b)
{
  return {math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z)};
}

} // end namespace math

using vec3f = Vector3<float>;
using vec3d = Vector3<double>;

} // end namespace cg

#endif // __Vector3_h
