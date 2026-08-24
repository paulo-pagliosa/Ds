//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2026 Paulo Pagliosa.                        |
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
// Last revision: 24/08/2026

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
template <IsReal R>
class Point<R, 3>
{
public:
  using type = Point<R, 3>;
  using value_type = R;

  R x;
  R y;
  R z;

  /// Default constructor.
  HOST DEVICE
  constexpr Point() = default;

  /// Constructs a Point3 from (x, y, z).
  HOST DEVICE
  constexpr Point(R x, R y, R z)
  {
    set(x, y, z);
  }

  /// Constructs a Point3 from p.
  template <typename T>
  HOST DEVICE
  constexpr explicit Point(const T& p)
  {
    set(p);
  }

  /// Constructs a Point3 from p[3].
  HOST DEVICE
  explicit Point(const R p[])
  {
    assert(p);
    set(p[0], p[1], p[2]);
  }

  /// Sets the coordinates of this object to (x, y, z).
  HOST DEVICE
  void set(R x, R y, R z)
  {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  /// Sets the coordinates of this object to (p, p, p) or p.
  template <typename T>
  HOST DEVICE
  void set(const T& p)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = z = R(p);
    else
      set(R(p.x), R(p.y), R(p.z));
  }

  /// Returns ths size of this object.
  [[nodiscard]] HOST DEVICE
  constexpr int size() const
  {
    return 3;
  }

  /// Returns true if this object is equal to p.
  [[nodiscard]] HOST DEVICE
  bool equals(const type& p, R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x - p.x, y - p.y, z - p.z, eps);
  }

  [[nodiscard]] HOST DEVICE
  bool operator ==(const type& p) const
  {
    return equals(p);
  }

  /// Returns true if this object is not equal to p.
  [[nodiscard]] HOST DEVICE
  bool operator !=(const type& p) const
  {
    return !operator ==(p);
  }

  /// Returns a reference to this object += p.
  HOST DEVICE
  auto& operator +=(const type& p)
  {
    x += p.x;
    y += p.y;
    z += p.z;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator +=(const Vector3<R>& v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator -=(const Vector3<R>& v)
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

  /// Returns this object + p.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const type& p) const
  {
    return type{x + p.x, y + p.y, z + p.z};
  }

  /// Returns this object + v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const Vector3<R>& v) const
  {
    return type{x + v.x, y + v.y, z + v.z};
  }

  /// Returns this object - p.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const type& p) const
  {
    return Vector3<R>{x - p.x, y - p.y, z - p.z};
  }

  /// Returns this object - v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const Vector3<R>& v) const
  {
    return type{x - v.x, y - v.y, z - v.z};
  }

  /// Returns this object * -1.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -() const
  {
    return type{-x, -y, -z};
  }

  /// Returns this object * s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{x * s, y * s, z * s};
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

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s(%g,%g,%g)\n", s, x, y, z);
  }

}; // Point3

template <IsReal R> using Point3 = Point<R, 3>;

/// Returns s * p.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr  auto
operator *(R s, const Point3<R>& p)
{
  return p * s;
}

using Point3f = Point3<float>;
using Point3d = Point3<double>;

} // end namespace cg

#endif // __Point3_h
