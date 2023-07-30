//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
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
// Last revision: 30/07/2023

#ifndef __Quaternion_h
#define __Quaternion_h

#include "math/Vector3.h"

namespace cg
{ // begin namespace cg

// Forward definition
template <typename real, int M, int N> class Matrix;

template <typename real>
HOST DEVICE
inline constexpr Vector3<real>
toRadians3(const Vector3<real>& v)
{
  const auto x = math::toRadians(v.x);
  const auto y = math::toRadians(v.y);
  const auto z = math::toRadians(v.z);

  return {x, y, z};
}

template <typename real>
HOST DEVICE
inline constexpr Vector3<real>
toDegrees3(const Vector3<real>& v)
{
  const auto x = math::toDegrees(v.x);
  const real y = math::toDegrees(v.y);
  const real z = math::toDegrees(v.z);

  return {x, y, z};
}

template <typename real>
HOST DEVICE
inline Vector3<real>
cos3(const Vector3<real>& v)
{
  return {real(cos(v.x)), real(cos(v.y)), real(cos(v.z))};
}

template <typename real>
HOST DEVICE
inline Vector3<real>
sin3(const Vector3<real>& v)
{
  return {real(sin(v.x)), real(sin(v.y)), real(sin(v.z))};
}


/////////////////////////////////////////////////////////////////////
//
// Quaternion: quaternion class
// ==========
template <typename real>
class Quaternion
{
public:
  ASSERT_REAL(real, "Quaternion: floating point type expected");

  using quat = Quaternion;
  using vec3 = Vector3<real>;
  using mat3 = Matrix<real, 3, 3>;
  using value_type = real;

  real x;
  real y;
  real z;
  real w;

  /// Default constructor.
  HOST DEVICE
  Quaternion()
  {
    // do nothing
  }

  /// Constructs a Quaternion object from [(x, y, z), w].
  HOST DEVICE
  Quaternion(real x, real y, real z, real w)
  {
    set(x, y, z, w);
  }

  /// Constructs a Quaternion object from q[4].
  HOST DEVICE
  explicit Quaternion(const real q[])
  {
    set(q);
  }

  /// Constructs a Quaternion object from [v, w].
  HOST DEVICE
  explicit Quaternion(const vec3& v, real w = 0)
  {
    set(v, w);
  }

  /// Constructs a Quaternion object from angle (in degrees) and axis.
  HOST DEVICE
  Quaternion(real angle, const vec3& axis)
  {
    set(angle, axis);
  }

  /// Constructs a Quaternion object from m.
  HOST DEVICE
  explicit Quaternion(const mat3& m)
  {
    set(m);
  }

  /// Constructs a Quaternion object from [(0, 0, 0), v] or v.
  template <typename T>
  HOST DEVICE
  explicit Quaternion(const T& v)
  {
    set(v);
  }

  /// Sets this object to q.
  HOST DEVICE
  void set(const quat& q)
  {
    *this = q;
  }

  /// Sets the coordinates of this object to [(x, y, z), w].
  HOST DEVICE
  void set(real x, real y, real z, real w)
  {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  /// Sets the coordinates of this object to q[4].
  HOST DEVICE
  void set(const real q[])
  {
    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }

  /// Sets the coordinates of this object to [v, w].
  HOST DEVICE
  void set(const vec3& v, real w = 0)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    this->w = w;
  }

  /// Sets the coordinates of this object from angle (in degress) and axis.
  HOST DEVICE
  void set(real angle, const vec3& axis)
  {
    const auto a = math::toRadians(angle) * real(0.5);
    const auto v = axis.versor() * sin(a);

    set(v, cos(a));
  }

  /// Sets the coordinates of this object from m.
  HOST DEVICE
  void set(const mat3& m); // implemented in Matrix3x3.h

  /// Sets the coordinates of this object to [(0, 0, 0), v] or v.
  template <typename T>
  HOST DEVICE
  void set(const T& v)
  {
    if constexpr (std::is_arithmetic_v<T>)
      set(vec3::null(), real(v));
    else
      set(real(v.x), real(v.y), real(v.z), real(v.w));
  }

  template <typename T>
  HOST DEVICE
  auto& operator =(const T& v)
  {
    set(v);
    return *this;
  }

  /// Returns a pointer to the elements of this object.
  HOST DEVICE
  explicit operator const real*() const
  {
    return &x;
  }

  /// Returns an identity quaternion.
  HOST DEVICE
  static auto identity()
  {
    return quat{real(1)};
  }

  /// \brief Returns a quaternion that rotates z radians around the
  /// z axis, x radians around the x axis, and y radians around the
  /// y axis (IN THAT ORDER).
  HOST DEVICE
  static quat eulerAngles(real x, real y, real z)
  {
    return eulerAngles(vec3{x, y, z});
  }

  /// Returns a quaternion from Euler angles.
  HOST DEVICE
  static quat eulerAngles(const vec3& angles)
  {
    const auto a = toRadians3(angles) * real(0.5);
    const auto c = cos3(a);
    const auto s = sin3(a);
    const auto x = c.y * s.x * c.z + s.y * c.x * s.z;
    const auto y = s.y * c.x * c.z - c.y * s.x * s.z;
    const auto z = c.y * c.x * s.z - s.y * s.x * c.z;
    const auto w = c.y * c.x * c.z + s.y * s.x * s.z;

    return quat{x, y, z, w};
  }

  /// Returns the Euler angles (in degress) from this object.
  HOST DEVICE
  vec3 eulerAngles() const;

  /// Returns a quaternion from forward and up.
  HOST DEVICE
  static auto lookAt(const vec3& forward, const vec3& up = vec3::up())
  {
    mat3 m;

    m[2] = forward.versor();
    m[0] = up.cross(forward).versor();
    m[1] = m[2].cross(m[0]);
    return quat{m};
  }

  /// Returns true if this object is equals to q.
  HOST DEVICE
  bool equals(const quat& q, real eps = math::Limits<real>::eps()) const
  {
    return math::isNull(x - q.x, y - q.y, z - q.z, w - q.w, eps);
  }

  HOST DEVICE
  bool operator ==(const quat& q) const
  {
    return equals(q);
  }

  HOST DEVICE
  bool operator !=(const quat& q) const
  {
    return !operator ==(q);
  }

  /// Returns a reference to this object += q.
  HOST DEVICE
  auto& operator +=(const quat& q)
  {
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
  }

  /// Returns a reference to this object -= q.
  HOST DEVICE
  auto& operator -=(const quat& q)
  {
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(real s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /// Returns a reference to this object *= q.
  HOST DEVICE
  auto& operator *=(const quat& q)
  {
    return *this = operator *(q);
  }

  /// Returns this object + q.
  HOST DEVICE
  auto operator +(const quat& q) const
  {
    return quat{x + q.x, y + q.y, z + q.z, w + q.w};
  }

  /// Returns this object + q.
  HOST DEVICE
  auto operator -(const quat& q) const
  {
    return quat{x - q.x, y - q.y, z - q.z, w - q.w};
  }

  /// Returns this object * s.
  HOST DEVICE
  auto operator *(real s) const
  {
    return quat{x * s, y * s, z * s, w * s};
  }

  /// Returns this object * q.
  HOST DEVICE
  auto operator *(const quat& q) const
  {
    const auto cx = w * q.x + q.w * x + y * q.z - q.y * z;
    const auto cy = w * q.y + q.w * y + z * q.x - q.z * x;
    const auto cz = w * q.z + q.w * z + x * q.y - q.x * y;
    const auto cw = w * q.w - q.x * x - y * q.y - q.z * z;

    return quat{cx, cy, cz, cw};
  }

  /// Returns this object * v.
  HOST DEVICE
  vec3 operator *(const vec3& v) const
  {
    return rotate(v);
  }

  /// Returns this object * -1.
  HOST DEVICE
  auto operator -() const
  {
    return quat{-x, -y, -z, -w};
  }

  /// Returns the conjugate of this object.
  HOST DEVICE
  auto operator ~() const
  {
    return quat{-x, -y, -z, +w};
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
    return real(sqrt(squaredNorm()));
  }

  /// Returns true if length of this object is close to unit.
  HOST DEVICE
  bool isUnit(real eps = math::Limits<real>::eps()) const
  {
    return math::isEqual(squaredNorm(), 1, eps);
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
  HOST DEVICE
  auto conjugate() const
  {
    return operator ~();
  }

  /// Returns the inverse of this object.
  HOST DEVICE
  auto inverse() const
  {
    return conjugate().normalize();
  }

  /// Returns the point p rotated by this object.
  HOST DEVICE
  vec3 rotate(const vec3& p) const
  {
    const auto vx = real(2) * p.x;
    const auto vy = real(2) * p.y;
    const auto vz = real(2) * p.z;
    const auto w2 = w * w - real(0.5);
    const auto d2 = x * vx + y * vy + z * vz;
    const auto px = x * d2 + w * (y * vz - z * vy) + vx * w2;
    const auto py = y * d2 + w * (z * vx - x * vz) + vy * w2;
    const auto pz = z * d2 + w * (x * vy - y * vx) + vz * w2;

    return {px, py, pz};
  }

  /// Returns the point p rotated by the inverse of this object.
  HOST DEVICE
  vec3 inverseRotate(const vec3& p) const
  {
    const auto vx = real(2) * p.x;
    const auto vy = real(2) * p.y;
    const auto vz = real(2) * p.z;
    const auto w2 = w * w - real(0.5);
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

template  <typename real>
HOST DEVICE Vector3<real>
Quaternion<real>::eulerAngles() const
{
  const auto sqx = math::sqr(x);
  const auto sqy = math::sqr(y);
  const auto sqz = math::sqr(z);
  const auto sqw = math::sqr(w);
  const auto one = sqx + sqy + sqz + sqw;
  const auto eps = real(0.4995) * one;
  const auto tol = x * w - y * z;
  vec3 angles;

  if (tol > eps)
  {
    angles.y = real(+2 * atan2(y, x));
    angles.x = +math::pi<real> / 2;
    angles.z = 0;
  }
  else if (tol < -eps)
  {
    angles.y = real(-2 * atan2(y, x));
    angles.x = -math::pi<real> / 2;
    angles.z = 0;
  }
  else
  {
    angles.y = real(atan2(2 * (y * w + x * z), sqw - sqx - sqy + sqz));
    angles.x = real(asin(2 * tol / one));
    angles.z = real(atan2(2 * (z * w + x * y), sqw - sqx + sqy - sqz));
  }
  return toDegrees3(angles);
}

/// Returns the scalar multiplication of s and q.
template <typename real>
HOST DEVICE inline Quaternion<real>
operator *(real s, const Quaternion<real>& q)
{
  return q * s;
}

using quatf = Quaternion<float>;
using quatd = Quaternion<double>;

} // end namespace cg

#endif // __Quaternion_h
