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
// OVERVIEW: Point2.h
// ========
// Class definition for 2D point.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Point2_h
#define __Point2_h

#include "math/Vector2.h"

namespace cg
{ // begin namespace cg

template <IsReal R, int N> class Point;


/////////////////////////////////////////////////////////////////////
//
// Point2: 2D point class
// ======
template <IsReal R>
class Point<R, 2>
{
public:
  using type = Point<R, 2>;
  using value_type = R;

  R x;
  R y;

  /// Default constructor.
  HOST DEVICE
  constexpr Point() = default;

  /// Constructs a Point2 from (x, y).
  HOST DEVICE
  constexpr Point(R x, R y)
  {
    set(x, y);
  }

  /// Constructs a Point2 from p.
  template <typename T>
  HOST DEVICE
  explicit constexpr Point(const T& p)
  {
    set(p);
  }

  /// Constructs a Point2 from p[2].
  HOST DEVICE
  explicit Point(const R p[])
  {
    assert(p);
    set(p[0], p[1]);
  }

  /// Sets the coordinates of this object to (x, y).
  HOST DEVICE
  void set(R x, R y)
  {
    this->x = x;
    this->y = y;
  }

  /// Sets the coordinates of this object to (p, p) or p.
  template <typename T>
  HOST DEVICE
  void set(const T& p)
  {
    if constexpr (std::is_arithmetic_v<T>)
      x = y = R(p);
    else
      set(R(p.x), R(p.y));
  }

  /// Returns ths size of this object.
  [[nodiscard]] HOST DEVICE
  constexpr int size() const
  {
    return 2;
  }

  /// Returns true if this object is equal to p.
  [[nodiscard]] HOST DEVICE
  bool equals(const type& p, R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x - p.x, y - p.y, eps);
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
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator +=(const Vector2<R>& v)
  {
    x += v.x;
    y += v.y;
    return *this;
  }

  /// Returns a reference to this object -= v.
  HOST DEVICE
  auto& operator -=(const Vector2<R>& v)
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
    return type{x + p.x, y + p.y};
  }

  /// Returns this object + v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const Vector2<R>& v) const
  {
    return type{x + v.x, y + v.y};
  }

  /// Returns this object - p.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const type& p) const
  {
    return Vector2<R>{x - p.x, y - p.y};
  }

  /// Returns this object - v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const Vector2<R>& v) const
  {
    return type{x - v.x, y - v.y};
  }

  /// Returns this object * -1.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -() const
  {
    return type{-x, -y};
  }

  /// Returns this object * s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{x * s, y * s};
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

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s(%g,%g)\n", s, x, y);
  }

}; // Point2

template <IsReal R> using Point2 = Point<R, 2>;

/// Returns s * p.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Point2<R>& p)
{
  return p * s;
}

using Point2f = Point2<float>;
using Point2d = Point2<double>;

} // end namespace cg

#endif // __Point2_h
