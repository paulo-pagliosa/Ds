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
// OVERVIEW: Point2.h
// ========
// Class definition for 2D point.
//
// Author: Paulo Pagliosa
// Last revision: 29/05/2020

#ifndef __Point2_h
#define __Point2_h

#include "math/Vector2.h"

namespace cg
{ // begin namespace cg

template <typename real, int N> class Point;


/////////////////////////////////////////////////////////////////////
//
// Point2: 2D point class
// ======
template <typename real>
class Point<real, 2>
{
public:
  ASSERT_REAL(real, "Point2: floating point type expected");

  using Point2 = Point<real, 2>;
  using value_type = real;

  real x;
  real y;

  /// Default constructor.
  HOST DEVICE
  Point()
  {
    // do nothing
  }

  /// Constructs a Point2 object from (x, y).
  HOST DEVICE
  Point(real x, real y)
  {
    set(x, y);
  }

  /// Constructs a Point2 object from p[2].
  HOST DEVICE
  explicit Point(const real p[])
  {
    set(p);
  }

  /// Constructs a Point2 object with (s, s) or (p.x, p.y).
  template <typename T>
  HOST DEVICE
  explicit Point(const T& p)
  {
    set(p);
  }

  /// Sets this object to p.
  HOST DEVICE
  void set(const Point2& p)
  {
    *this = p;
  }

  /// Sets the coordinates of this object to (x, y).
  HOST DEVICE
  void set(real x, real y)
  {
    this->x = x;
    this->y = y;
  }

  /// Sets the coordinates of this object to p[2].
  HOST DEVICE
  void set(const real p[])
  {
    x = p[0];
    y = p[1];
  }

  /// Sets the coordinates of this object to (s, s) or (p.x, p.y).
  template <typename T>
  HOST DEVICE
  void set(const T& p)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = real(p);
    else
      set(real(p.x), real(p.y));
  }

  template <typename T>
  HOST DEVICE
  Point2& operator =(const T& p)
  {
    set(p);
    return *this;
  }

  /// Returns true if this object is equal to p.
  HOST DEVICE
  bool equals(const Point2& p, real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x - p.x, y - p.y, eps);
  }

  HOST DEVICE
  bool operator ==(const Point2& p) const
  {
    return equals(p);
  }

  /// Returns true if this object is not equal to p.
  HOST DEVICE
  bool operator !=(const Point2& p) const
  {
    return !operator ==(p);
  }

  /// Returns a reference to this object += p.
  HOST DEVICE
  Point2& operator +=(const Point2& p)
  {
    x += p.x;
    y += p.y;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  Point2& operator +=(const Vector2<real>& v)
  {
    x += v.x;
    y += v.y;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  Point2& operator -=(const Vector2<real>& v)
  {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  Point2& operator *=(real s)
  {
    x *= s;
    y *= s;
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
  Point2 operator +(const Point2& p) const
  {
    return Point2{x + p.x, y + p.y};
  }

  /// Returns this object + v.
  HOST DEVICE
  Point2 operator +(const Vector2<real>& v) const
  {
    return Point2{x + v.x, y + v.y};
  }

  /// Returns this object - p.
  HOST DEVICE
  Vector2<real> operator -(const Point2& p) const
  {
    return Vector2<real>{x - p.x, y - p.y};
  }

  /// Returns this object - v.
  HOST DEVICE
  Point2 operator -(const Vector2<real>& v) const
  {
    return Point2{x - v.x, y - v.y};
  }

  /// Returns this object * -1.
  HOST DEVICE
  Point2 operator -() const
  {
    return Point2{-x, -y};
  }

  /// Returns this object * s.
  HOST DEVICE
  Point2 operator *(real s) const
  {
    return Point2{x * s, y * s};
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

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s(%g,%g)\n", s, x, y);
  }

}; // Point2

template <typename real> using Point2 = Point<real, 2>;

/// Returns s * p.
template <typename real>
HOST DEVICE inline Point2<real>
operator *(real s, const Point2<real>& p)
{
  return p * s;
}

using Point2f = Point2<float>;
using Point2d = Point2<double>;

} // end namespace cg

#endif // __Point2_h
