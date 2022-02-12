//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020 Paulo Pagliosa.                              |
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
// OVERVIEW: Point3.h
// ========
// Class definition for 3D point.
//
// Author: Paulo Pagliosa
// Last revision: 29/05/2020

#ifndef __Point3_h
#define __Point3_h

#include "geometry/Point2.h"
#include "math/Vector3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Point3: 3D point class
// ======
template <typename real>
class Point<real, 3>
{
public:
  ASSERT_REAL(real, "Point3: floating point type expected");

  using Point3 = Point<real, 3>;
  using value_type = real;

  real x;
  real y;
  real z;

  /// Default constructor.
  HOST DEVICE
  Point()
  {
    // do nothing
  }

  /// Constructs a Point3 object from (x, y, z).
  HOST DEVICE
  Point(real x, real y, real z)
  {
    set(x, y, z);
  }

  /// Constructs a Point3 object from p[3].
  HOST DEVICE
  explicit Point(const real p[])
  {
    set(p);
  }

  /// Constructs a Point3 object with (p, p, p) or (p.x, p.y, p.z)
  template <typename T>
  HOST DEVICE
  explicit Point(const T& p)
  {
    set(p);
  }

  /// Sets this object to p.
  HOST DEVICE
  void set(const Point3& p)
  {
    *this = p;
  }

  /// Sets the coordinates of this object to (x, y, z).
  HOST DEVICE
  void set(real x, real y, real z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  /// Sets the coordinates of this object to p[3].
  HOST DEVICE
  void set(const real p[])
  {
    x = p[0];
    y = p[1];
    z = p[2];
  }

  /// Sets the coordinates of this object to (p, p, p) or (p.x, p.y, p.z).
  template <typename T>
  HOST DEVICE
  void set(const T& p)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = z = real(p);
    else
      set(real(p.x), real(p.y), real(p.z));
  }

  template <typename T>
  HOST DEVICE
  Point3& operator =(const T& p)
  {
    set(p);
    return *this;
  }

  /// Returns true if this object is equal to p.
  HOST DEVICE
  bool equals(const Point3& p, real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x - p.x, y - p.y, z - p.z, eps);
  }

  HOST DEVICE
  bool operator ==(const Point3& p) const
  {
    return equals(p);
  }

  /// Returns true if this object is not equal to p.
  HOST DEVICE
  bool operator !=(const Point3& p) const
  {
    return !operator ==(p);
  }

  /// Returns a reference to this object += p.
  HOST DEVICE
  Point3& operator +=(const Point3& p)
  {
    x += p.x;
    y += p.y;
    z += p.z;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  Point3& operator +=(const Vector3<real>& v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  Point3& operator -=(const Vector3<real>& v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  Point3& operator *=(real s)
  {
    x *= s;
    y *= s;
    z *= s;
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

  /// Returns this object + p.
  HOST DEVICE
  Point3 operator +(const Point3& p) const
  {
    return Point3{x + p.x, y + p.y, z + p.z};
  }

  /// Returns this object + v.
  HOST DEVICE
  Point3 operator +(const Vector3<real>& v) const
  {
    return Point3{x + v.x, y + v.y, z + v.z};
  }

  /// Returns this object - p.
  HOST DEVICE
  Vector3<real> operator -(const Point3& p) const
  {
    return Vector3<real>{x - p.x, y - p.y, z - p.z};
  }

  /// Returns this object - v.
  HOST DEVICE
  Point3 operator -(const Vector3<real>& v) const
  {
    return Point3{x - v.x, y - v.y, z - v.z};
  }

  /// Returns this object * -1.
  HOST DEVICE
  Point3 operator -() const
  {
    return Point3{-x, -y, -z};
  }

  /// Returns this object * s.
  HOST DEVICE
  Point3 operator *(real s) const
  {
    return Point3{x * s, y * s, z * s};
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

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s(%g,%g,%g)\n", s, x, y, z);
  }

}; // Point3

template <typename real> using Point3 = Point<real, 3>;

/// Returns s * p.
template <typename real>
HOST DEVICE inline Point3<real>
operator *(real s, const Point3<real>& p)
{
  return p * s;
}

using Point3f = Point3<float>;
using Point3d = Point3<double>;

} // end namespace cg

#endif // __Point3_h
