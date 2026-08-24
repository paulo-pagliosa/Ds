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
// OVERVIEW: Quaternion.h
// ========
// Class definition for quaternion.
//
// Author: Paulo Pagliosa
// Last revision: 19/08/2026

#ifndef __Quaternion_h
#define __Quaternion_h

#include "math/Vector3.h"

namespace cg
{ // begin namespace cg

// Forward definition
template <IsReal R, int M, int N> class Matrix;

namespace math
{ // begin namespace math

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
toRadians(const Vector3<R>& v) -> Vector3<R>
{
  return {toRadians(v.x), toRadians(v.y), toRadians(v.z)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
toDegrees(const Vector3<R>& v) -> Vector3<R>
{
  return {toDegrees(v.x), toDegrees(v.y), toDegrees(v.z)};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
cos3(const Vector3<R>& v) -> Vector3<R>
{
  return {R(cos(v.x)), R(cos(v.y)), R(cos(v.z))};
}

template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
sin3(const Vector3<R>& v) -> Vector3<R>
{
  return {R(sin(v.x)), R(sin(v.y)), R(sin(v.z))};
}

} // end namespace math


/////////////////////////////////////////////////////////////////////
//
// Quaternion: quaternion class
// ==========
template <IsReal R>
class Quaternion
{
public:
  using type = Quaternion;
  using value_type = R;
  using vec3 = Vector3<R>;
  using mat3 = Matrix<R, 3, 3>;

  R x;
  R y;
  R z;
  R w;

  /// Default constructor.
  HOST DEVICE
  Quaternion() = default;

  /// Constructs a Quaternion from [(x, y, z), w].
  HOST DEVICE
  constexpr Quaternion(R x, R y, R z, R w)
  {
    set(x, y, z, w);
  }

  /// Constructs a Quaternion from [v, w].
  HOST DEVICE
  explicit constexpr Quaternion(const vec3& v, R w = 0)
  {
    set(v, w);
  }

  /// Constructs a Quaternion from angle (in degrees) and axis.
  HOST DEVICE
  constexpr Quaternion(R angle, const vec3& axis)
  {
    set(angle, axis);
  }

  /// Constructs a Quaternion from m.
  HOST DEVICE
  explicit Quaternion(const mat3& m)
  {
    set(m);
  }

  /// Constructs a Quaternion from v.
  template <typename T>
  HOST DEVICE
  explicit constexpr Quaternion(const T& v)
  {
    set(v);
  }

  /// Constructs a Quaternion from q[4].
  HOST DEVICE
  explicit Quaternion(const R q[])
  {
    assert(q);
    set(q[0], q[1], q[2], q[3]);
  }

  /// Sets the coordinates of this object to [(x, y, z), w].
  HOST DEVICE
  constexpr void set(R x, R y, R z, R w)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  /// Sets the coordinates of this object to [v, w].
  HOST DEVICE
  constexpr void set(const vec3& v, R w = 0)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
  }

  /// Sets the coordinates of this object from angle (in degress) and axis.
  HOST DEVICE
  constexpr void set(R angle, const vec3& axis)
  {
    const auto a = math::toRadians(angle) * R(0.5);
    const auto v = axis.versor() * sin(a);

    set(v, cos(a));
  }

  /// Sets the coordinates of this object from m.
  HOST DEVICE
  void set(const mat3& m); // implemented in Matrix3x3.h

  /// Sets the coordinates of this object to [(0, 0, 0), v] or v.
  template <typename T>
  HOST DEVICE
  constexpr void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      set(vec3::null(), R(v));
    else
      set(R(v.x), R(v.y), R(v.z), R(v.w));
  }

  /// Returns a pointer to the elements of this object.
  [[nodiscard]] HOST DEVICE
  explicit operator const R*() const
  {
    return &x;
  }

  /// Returns an identity quaternion.
  [[nodiscard]] HOST DEVICE
  static constexpr auto identity()
  {
    return type{R(1)};
  }

  /// \brief Returns a quaternion that rotates z radians around the
  /// z axis, x radians around the x axis, and y radians around the
  /// y axis (IN THAT ORDER).
  [[nodiscard]] HOST DEVICE
  static constexpr auto eulerAngles(R x, R y, R z)
  {
    return eulerAngles(vec3{x, y, z});
  }

  /// Returns a quaternion from Euler angles.
  [[nodiscard]] HOST DEVICE
  static constexpr auto eulerAngles(const vec3& angles)
  {
    const auto a = math::toRadians(angles) * R(0.5);
    const auto c = math::cos3(a);
    const auto s = math::sin3(a);
    const auto x = c.y * s.x * c.z + s.y * c.x * s.z;
    const auto y = s.y * c.x * c.z - c.y * s.x * s.z;
    const auto z = c.y * c.x * s.z - s.y * s.x * c.z;
    const auto w = c.y * c.x * c.z + s.y * s.x * s.z;

    return type{x, y, z, w};
  }

  /// Returns the Euler angles (in degress) from this object.
  [[nodiscard]] HOST DEVICE
  vec3 eulerAngles() const;

  /// Returns a quaternion from forward and up.
  [[nodiscard]] HOST DEVICE
  static auto lookAt(const vec3& forward, const vec3& up = vec3::up())
  {
    mat3 m;

    m[2] = forward.versor();
    m[0] = up.cross(forward).versor();
    m[1] = m[2].cross(m[0]);
    return type{m};
  }

  /// Returns true if this object is equals to q.
  [[nodiscard]] HOST DEVICE
  bool equals(const type& q, R eps = math::Limits<R>::eps()) const
  {
    return math::isNull(x - q.x, y - q.y, z - q.z, w - q.w, eps);
  }

  [[nodiscard]] HOST DEVICE
  bool operator ==(const type& q) const
  {
    return equals(q);
  }

  [[nodiscard]] HOST DEVICE
  bool operator !=(const type& q) const
  {
    return !operator ==(q);
  }

  /// Returns a reference to this object += q.
  HOST DEVICE
  auto& operator +=(const type& q)
  {
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
  }

  /// Returns a reference to this object -= q.
  HOST DEVICE
  auto& operator -=(const type& q)
  {
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
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

  /// Returns a reference to this object *= q.
  HOST DEVICE
  auto& operator *=(const type& q)
  {
    return *this = operator *(q);
  }

  /// Returns this object + q.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const type& q) const
  {
    return type{x + q.x, y + q.y, z + q.z, w + q.w};
  }

  /// Returns this object + q.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const type& q) const
  {
    return type{x - q.x, y - q.y, z - q.z, w - q.w};
  }

  /// Returns this object * s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{x * s, y * s, z * s, w * s};
  }

  /// Returns this object * q.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& q) const
  {
    const auto cx = w * q.x + q.w * x + y * q.z - q.y * z;
    const auto cy = w * q.y + q.w * y + z * q.x - q.z * x;
    const auto cz = w * q.z + q.w * z + x * q.y - q.x * y;
    const auto cw = w * q.w - q.x * x - y * q.y - q.z * z;

    return type{cx, cy, cz, cw};
  }

  /// Returns this object * v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const vec3& v) const
  {
    return rotate(v);
  }

  /// Returns this object * -1.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator -() const
  {
    return type{-x, -y, -z, -w};
  }

  /// Returns the conjugate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator ~() const
  {
    return type{-x, -y, -z, +w};
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
    return R(sqrt(squaredNorm()));
  }

  /// Returns true if length of this object is close to unit.
  [[nodiscard]] HOST DEVICE
  bool isUnit(R eps = math::Limits<R>::eps()) const
  {
    return math::isEqual(squaredNorm(), 1, eps);
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

  /// Inverts and returns a reference to this object.
  HOST DEVICE
  auto& invert()
  {
    x = -x;
    y = -y;
    z = -z;
    return normalize();
  }

  /// Returns the conjugate of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto conjugate() const
  {
    return operator ~();
  }

  /// Returns the inverse of this object.
  [[nodiscard]] HOST DEVICE
  auto inverse() const
  {
    return conjugate().normalize();
  }

  /// Returns the point p rotated by this object.
  [[nodiscard]] HOST DEVICE
  constexpr vec3 rotate(const vec3& p) const
  {
    const auto vx = R(2) * p.x;
    const auto vy = R(2) * p.y;
    const auto vz = R(2) * p.z;
    const auto w2 = w * w - R(0.5);
    const auto d2 = x * vx + y * vy + z * vz;
    const auto px = x * d2 + w * (y * vz - z * vy) + vx * w2;
    const auto py = y * d2 + w * (z * vx - x * vz) + vy * w2;
    const auto pz = z * d2 + w * (x * vy - y * vx) + vz * w2;

    return {px, py, pz};
  }

  /// Returns the point p rotated by the inverse of this object.
  [[nodiscard]] HOST DEVICE
  constexpr vec3 inverseRotate(const vec3& p) const
  {
    const auto vx = R(2) * p.x;
    const auto vy = R(2) * p.y;
    const auto vz = R(2) * p.z;
    const auto w2 = w * w - R(0.5);
    const auto d2 = x * vx + y * vy + z * vz;
    const auto px = x * d2 - w * (y * vz - z * vy) + vx * w2;
    const auto py = y * d2 - w * (z * vx - x * vz) + vy * w2;
    const auto pz = z * d2 - w * (x * vy - y * vx) + vz * w2;

    return {px, py, pz};
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s[<%g,%g,%g>,%g]\n", s, x, y, z, w);
  }

}; // Quaternion

template <IsReal R>
HOST DEVICE Vector3<R>
Quaternion<R>::eulerAngles() const
{
  const auto sqx = math::sqr(x);
  const auto sqy = math::sqr(y);
  const auto sqz = math::sqr(z);
  const auto sqw = math::sqr(w);
  const auto one = sqx + sqy + sqz + sqw;
  const auto eps = R(0.4995) * one;
  const auto tol = x * w - y * z;
  vec3 a;

  if (tol > eps)
  {
    a.y = R(+2 * atan2(y, x));
    a.x = +math::pi<R> / 2;
    a.z = 0;
  }
  else if (tol < -eps)
  {
    a.y = R(-2 * atan2(y, x));
    a.x = -math::pi<R> / 2;
    a.z = 0;
  }
  else
  {
    a.y = R(atan2(2 * (y * w + x * z), sqw - sqx - sqy + sqz));
    a.x = R(asin(2 * tol / one));
    a.z = R(atan2(2 * (z * w + x * y), sqw - sqx + sqy - sqz));
  }
  return math::toDegrees(a);
}

/// Returns the scalar multiplication of s and q.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Quaternion<R>& q)
{
  return q * s;
}

using quatf = Quaternion<float>;
using quatd = Quaternion<double>;

} // end namespace cg

#endif // __Quaternion_h
