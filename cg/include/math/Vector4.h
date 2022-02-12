//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2020 Paulo Pagliosa.                        |
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
// Last revision: 02/06/2020

#ifndef __Vector4_h
#define __Vector4_h

#include "math/Vector3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Vector4: 4D vector class
// =======
template <typename real>
class Vector<real, 4>
{
public:
  ASSERT_REAL(real, "Vector4: floating point type expected");

  using vec3 = Vector3<real>;
  using vec4 = Vector<real, 4>;
  using value_type = real;

  real x;
  real y;
  real z;
  real w;

  /// Default constructor.
  HOST DEVICE
  Vector()
  {
    // do nothing
  }

  /// Constructs a Vector4 object from (x, y, z, w).
  HOST DEVICE
  Vector(real x, real y, real z, real w = 0)
  {
    set(x, y, z, w);
  }

  /// Constructs a Vector4 object from v[4].
  HOST DEVICE
  explicit Vector(const real v[])
  {
    set(v);
  }

  /// Constructs a Vector4 object from (v, w).
  HOST DEVICE
  explicit Vector(const vec3& v, real w = 0)
  {
    set(v, w);
  }

  /// Constructs a Vector4 object with (v, v, v, v) or v.
  template <typename T>
  HOST DEVICE
  explicit Vector(const T& v)
  {
    set(v);
  }

  /// Sets this object to v.
  HOST DEVICE
  void set(const vec4& v)
  {
    *this = v;
  }

  /// Sets the coordinates of this object to (x, y, z, w).
  HOST DEVICE
  void set(real x, real y, real z, real w = 0)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  /// Sets the coordinates of this object to v[4].
  HOST DEVICE
  void set(const real v[])
  {
    x = v[0];
    y = v[1];
    z = v[2];
    w = v[3];
  }

  /// Sets the coordinates of this object to (s, s, s, s).
  HOST DEVICE
  void set(real s)
  {
    x = y = z = w = s;
  }

  /// Sets the coordinates of this object to (v, w).
  HOST DEVICE
  void set(const vec3& v, real w = 0)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
  }

  /// Sets the coordinates of this object to (v, v, v, v) or v.
  template <typename T>
  HOST DEVICE
  void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = z = w = real(v);
    else
      set(real(v.x), real(v.y), real(v.z), real(v.w));
  }

  template <typename T>
  HOST DEVICE
  vec4& operator =(const T& v)
  {
    set(v);
    return *this;
  }

  /// Returns a null vector.
  HOST DEVICE
  static vec4 null()
  {
    return vec4(real(0));
  }

  /// Returns ths size of this object.
  HOST DEVICE
  constexpr int size() const
  {
    return 4;
  }

  /// Returns true if this object is equal to v.
  HOST DEVICE
  bool equals(const vec4& v, real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x - v.x, y - v.y, z - v.z, w - v.w, eps);
  }

  HOST DEVICE
  bool operator ==(const vec4& v) const
  {
    return equals(v);
  }

  /// Returns true if this object is not equal to v.
  HOST DEVICE
  bool operator !=(const vec4& v) const
  {
    return !operator ==(v);
  }

  /// Returns a reference to this object += v.
  HOST DEVICE
  vec4& operator +=(const vec4& v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  vec4& operator -=(const vec4& v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  vec4& operator *=(real s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /// Returns a reference to this object *= v.
  HOST DEVICE
  vec4& operator *=(const vec4& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
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
  vec4 operator +(const vec4& v) const
  {
    return vec4{x + v.x, y + v.y, z + v.z, w + v.w};
  }

  /// Returns this object - v.
  HOST DEVICE
  vec4 operator -(const vec4& v) const
  {
    return vec4{x - v.x, y - v.y, z - v.z, w - v.w};
  }

  /// Returns a vector in the direction opposite to this object.
  HOST DEVICE
  vec4 operator -() const
  {
    return vec4{-x, -y, -z, -w};
  }

  /// Returns the scalar multiplication of this object and s.
  HOST DEVICE
  vec4 operator *(real s) const
  {
    return vec4{x * s, y * s, z * s, w * s};
  }

  /// Returns the multiplication of this object and v.
  HOST DEVICE
  vec4 operator *(const vec4& v) const
  {
    return vec4{x * v.x, y * v.y, z * v.z, w * v.w};
  }

  /// Returns true if this object is null.
  HOST DEVICE
  bool isNull(real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x, y, z, w, eps);
  }

  /// Returns the squared norm of this object.
  HOST DEVICE
  real squaredNorm() const
  {
    return math::sqr(x) + math::sqr(y) + math::sqr(z) + math::sqr(w);
  }

  /// Returns the length of this object.
  HOST DEVICE
  real length() const
  {
    return sqrt(squaredNorm());
  }

  /// Returns the maximum coordinate of this object.
  HOST DEVICE
  real max() const
  {
    return math::max(x, math::max(y, math::max(z, w)));
  }

  /// Returns the minimum coordinate of this object.
  HOST DEVICE
  real min() const
  {
    return math::min(x, math::min(y, math::min(z, w)));
  }

  /// Returns the inverse of this object.
  HOST DEVICE
  vec4 inverse() const
  {
    return vec4{1 / x, 1 / y, 1 / z, 1 / w};
  }

  /// Inverts and returns a reference to this object.
  HOST DEVICE
  vec3& invert()
  {
    x = 1 / x;
    y = 1 / y;
    z = 1 / z;
    w = 1 / w;
    return *this;
  }

  /// Negates and returns a reference to this object.
  HOST DEVICE
  vec4& negate()
  {
    x = -x;
    y = -y;
    z = -z;
    w = -w;
    return *this;
  }

  /// Normalizes and returns a reference to this object.
  HOST DEVICE
  vec4& normalize(real eps = math::Limits<real>::eps())
  {
    const auto len = length();

    if (!math::isZero(len, eps))
      operator *=(math::inverse(len));
    return *this;
  }

  /// Returns the unit vector of this this object.
  HOST DEVICE
  vec4 versor(real eps = math::Limits<real>::eps()) const
  {
    return vec4{*this}.normalize(eps);
  }

  /// Returns the unit vector of v.
  HOST DEVICE
  static vec4 versor(const vec4& v, real eps = math::Limits<real>::eps())
  {
    return v.versor(eps);
  }

  /// Returns the dot product of this object and v.
  HOST DEVICE
  real dot(const vec4& v) const
  {
    return x * v.x + y * v.y + z * v.z + w * v.w;
  }

  /// Returns the dot product of this object and (x, y, z, w).
  HOST DEVICE
  real dot(real x, real y, real z, real w) const
  {
    return dot(vec4(x, y, z, w));
  }

  /// Returns the dot product of v and w.
  HOST DEVICE
  static real dot(const vec4& v, const vec4& w)
  {
    return v.dot(w);
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s<%g,%g,%g,%g>\n", s, x, y, z, w);
  }

}; // Vector4

template <typename real> using Vector4 = Vector<real, 4>;

/// Returns the scalar multiplication of s and v.
template <typename real>
HOST DEVICE inline Vector4<real>
operator *(real s, const Vector4<real>& v)
{
  return v * s;
}

using vec4f = Vector4<float>;
using vec4d = Vector4<double>;

} // end namespace cg

#endif // __Vector4_h
