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
// OVERVIEW: Matrix3x3.h
// ========
// Class definition for 3x3 matrix.
//
// Author: Paulo Pagliosa
// Last revision: 01/09/2026

#ifndef __Matrix3x3_h
#define __Matrix3x3_h

#include "math/Quaternion.h"

namespace cg
{ // begin namespace cg

template <IsReal R, int M, int N> class Matrix;


/////////////////////////////////////////////////////////////////////
//
// Matrix3x3: 3x3 matrix class (column-major format)
// =========
template <IsReal R>
class Matrix<R, 3, 3>
{
public:
  using type = Matrix<R, 3, 3>;
  using value_type = R;
  using quat = Quaternion<R>;
  using vec2 = Vector2<R>;
  using vec3 = Vector3<R>;
  using mat4 = Matrix<R, 4, 4>;

  /// Default constructor.
  constexpr Matrix() = default;

  /// Constructs a Matrix3x3 from [v0; v1; v2].
  HOST DEVICE
  constexpr Matrix(const vec3& v0, const vec3& v1, const vec3& v2)
  {
    set(v0, v1, v2);
  }

  /// Constructs a Matrix3x3 as a multiple s of the identity matrix.
  HOST DEVICE
  explicit constexpr Matrix(R s)
  {
    set(s);
  }

  /// Constructs a Matrix3x3 from the diagonal d.
  HOST DEVICE
  explicit constexpr Matrix(const vec3& d)
  {
    set(d);
  }

  /// Constructs a Matrix3x3 from q.
  HOST DEVICE
  explicit constexpr Matrix(const quat& q)
  {
    set(q);
  }

  /// Constructs a Matrix3x3 from m.
  HOST DEVICE
  explicit constexpr Matrix(const mat4& m)
  {
    set(m);
  }

  /// Constructs a Matrix3x3 from v[9].
  HOST DEVICE
  explicit Matrix(const R v[])
  {
    assert(v);
    set(vec3{&v[0]}, vec3{&v[3]}, vec3{&v[6]});
  }

  /// Sets the columns of this object to [v0; v1; v2].
  HOST DEVICE
  constexpr void set(const vec3& v0, const vec3& v1, const vec3& v2)
  {
    _v0 = v0;
    _v1 = v1;
    _v2 = v2;
  }

  /// Sets this object to a multiple s of the identity matrix.
  HOST DEVICE
  constexpr void set(R s)
  {
    _v0.set(s, 0, 0);
    _v1.set(0, s, 0);
    _v2.set(0, 0, s);
  }

  /// Sets this object to a diagonal matrix d.
  HOST DEVICE
  constexpr void set(const vec3& d)
  {
    _v0.set(d.x, 0, 0);
    _v1.set(0, d.y, 0);
    _v2.set(0, 0, d.z);
  }

  /// Sets the elements of this object from q.
  HOST DEVICE
  constexpr void set(const quat& q)
  {
    const auto qx = q.x;
    const auto qy = q.y;
    const auto qz = q.z;
    const auto qw = q.w;
    const auto x2 = qx + qx;
    const auto y2 = qy + qy;
    const auto z2 = qz + qz;
    const auto xx = qx * x2;
    const auto yy = qy * y2;
    const auto zz = qz * z2;
    const auto xy = qy * x2;
    const auto xz = qz * x2;
    const auto xw = qw * x2;
    const auto yz = qz * y2;
    const auto yw = qw * y2;
    const auto zw = qw * z2;

    _v0.set((R)1 - (yy + zz), xy + zw, xz - yw);
    _v1.set(xy - zw, (R)1 - (xx + zz), yz + xw);
    _v2.set(xz + yw, yz - xw, (R)1 - (xx + yy));
  }

  /// Sets the elements of this object from m.
  HOST DEVICE
  constexpr void set(const mat4& m); // implemented in Matrix4x4.h

  /// Returns a zero matrix.
  [[nodiscard]] HOST DEVICE
  static constexpr auto zero()
  {
    return type((R)0);
  }

  /// Returns an identity matrix.
  [[nodiscard]] HOST DEVICE
  static constexpr auto identity()
  {
    return type((R)1);
  }

  /// Returns a diagonal matrix d.
  [[nodiscard]] HOST DEVICE
  static constexpr auto diagonal(const vec3& d)
  {
    return type{d};
  }

  /// Returns the diagonal of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto diagonal() const
  {
    return vec3{_v0.x, _v1.y, _v2.z};
  }

  /// Returns the trace of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto trace() const
  {
    return _v0.x + _v1.y + _v2.z;
  }

  /// Returns a reference to the j-th column of this object.
  [[nodiscard]] HOST DEVICE
  auto& operator [](int j)
  {
    assert(j >= 0 && j < 3);
    return (&_v0)[j];
  }

  /// Returns the j-th column of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int j) const
  {
    assert(j >= 0 && j < 3);
    return (&_v0)[j];
  }

  /// Returns a reference to the element (i, j) of this object.
  [[nodiscard]] HOST DEVICE
  auto& operator ()(int i, int j)
  {
    return (*this)[j][i];
  }

  /// Returns the element (i, j) of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator ()(int i, int j) const
  {
    return (*this)[j][i];
  }

  /// Returns this object * s.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(R s) const
  {
    return type{_v0 * s, _v1 * s, _v2 * s};
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(R s)
  {
    _v0 *= s;
    _v1 *= s;
    _v2 *= s;
    return *this;
  }

  /// Returns this object * m.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& m) const
  {
    const auto b0 = transform(m._v0);
    const auto b1 = transform(m._v1);
    const auto b2 = transform(m._v2);

    return type{b0, b1, b2};
  }

  /// Returns a reference to this object *= m.
  HOST DEVICE
  auto& operator *=(const type& m)
  {
    return *this = operator *(m);
  }

  /// Returns this object * v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const vec3& v) const
  {
    return transform(v);
  }

  /// Returns the transposed of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transposed() const
  {
    const vec3 b0{_v0.x, _v1.x, _v2.x};
    const vec3 b1{_v0.y, _v1.y, _v2.y};
    const vec3 b2{_v0.z, _v1.z, _v2.z};

    return type{b0, b1, b2};
  }

  /// Transposes and returns a reference to this object.
  HOST DEVICE
  auto& transpose()
  {
    return *this = transposed();
  }

  /// \brief Tries to invert this object and returns true on success;
  /// otherwise, leaves this object unchanged and returns false.
  HOST DEVICE
  bool invert(R eps = math::Limits<R>::eps())
  {
    const auto b00 = _v1[1] * _v2[2] - _v1[2] * _v2[1];
    const auto b01 = _v0[2] * _v2[1] - _v0[1] * _v2[2];
    const auto b02 = _v0[1] * _v1[2] - _v0[2] * _v1[1];
    const auto b10 = _v1[2] * _v2[0] - _v1[0] * _v2[2];
    const auto b11 = _v0[0] * _v2[2] - _v0[2] * _v2[0];
    const auto b12 = _v0[2] * _v1[0] - _v0[0] * _v1[2];
    const auto b20 = _v1[0] * _v2[1] - _v1[1] * _v2[0];
    const auto b21 = _v0[1] * _v2[0] - _v0[0] * _v2[1];
    const auto b22 = _v0[0] * _v1[1] - _v0[1] * _v1[0];
    auto d = _v0[0] * b00 + _v1[0] * b01 + _v2[0] * b02;

    if (math::isZero(d, eps))
      return false;
    d = R(1 / d);
    _v0.set(d * b00, d * b01, d * b02);
    _v1.set(d * b10, d * b11, d * b12);
    _v2.set(d * b20, d * b21, d * b22);
    return true;
  }

  /// Assigns this object to m and tries to invert m.
  HOST DEVICE
  bool inverse(type& m, R eps = math::Limits<R>::eps()) const
  {
    return (m = *this).invert(eps);
  }

  /// Returns v transformed by this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transform(const vec3& v) const
  {
    return _v0 * v.x + _v1 * v.y + _v2 * v.z;
  }

  /// Returns v transformed by the transposed of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transposeTransform(const vec3& v) const
  {
    return vec3{_v0.dot(v), _v1.dot(v), _v2.dot(v)};
  }

  /// \brief Returns a 2D point p transformed by this object.
  /// This method is slower than transform3x4, but can handle
  /// projective transformations as well.
  [[nodiscard]] HOST DEVICE
  constexpr auto transform(const vec2& p) const
  {
    const auto r = transform(vec3{p, 1});
    return math::isZero(r.z) ? vec2{r} : vec2{r} * ((R)1 / r.z);
  }

  /// \brief Returns a 2D point p transformed by this object.
  /// This method is faster than transform, but it can solely
  /// handle affine 2D transformations.
  [[nodiscard]] HOST DEVICE
  constexpr auto transform2x3(const vec2& p) const
  {
    const auto x = _v0.x * p.x + _v1.x * p.y + _v2.x;
    const auto y = _v0.y * p.x + _v1.y * p.y + _v2.y;

    return vec2{x, y};
  }

  /// Returns a vector v transformed by this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transformVector(const vec2& v) const
  {
    return vec2{_v0} * v.x + vec2{_v1} * v.y;
  }

  /// Returns a pointer to the elements of this object.
  [[nodiscard]] HOST DEVICE
  explicit operator const R*() const
  {
    return &_v0.x;
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    fprintf(f, "[%9.4g %9.4g %9.4g]\n", _v0.x, _v1.x, _v2.x);
    fprintf(f, "[%9.4g %9.4g %9.4g]\n", _v0.y, _v1.y, _v2.y);
    fprintf(f, "[%9.4g %9.4g %9.4g]\n", _v0.z, _v1.z, _v2.z);
  }

private:
  vec3 _v0; // column 0
  vec3 _v1; // column 1
  vec3 _v2; // column 2

}; // Matrix3x3

template <IsReal R> using Matrix3x3 = Matrix<R, 3, 3>;

/// Sets the coordinates of this object from m.
template <IsReal R>
HOST DEVICE void
Quaternion<R>::set(const Matrix3x3<R>& m) // declared in Quaternion.h
{
  if (auto t = m.trace(); t >= 0)
  {
    auto s = sqrt(t + 1);

    w = R(0.5) * s;
    s = R(0.5) / s;
    x = (m(2, 1) - m(1, 2)) * s;
    y = (m(0, 2) - m(2, 0)) * s;
    z = (m(1, 0) - m(0, 1)) * s;
    return;
  }

  int i{0};

  if (m(1, 1) > m(0, 0))
    i = 1;
  if (m(2, 2) > m(i, i))
    i = 2;
  if (i == 1)
  {
    auto s = sqrt(m(1, 1) - (m(2, 2) + m(0, 0)) + 1);

    y = R(0.5) * s;
    s = R(0.5) / s;
    z = (m(1, 2) + m(2, 1)) * s;
    x = (m(0, 1) + m(1, 0)) * s;
    w = (m(0, 2) - m(2, 0)) * s;
  }
  else if (i == 2)
  {
    auto s = sqrt(m(2, 2) - (m(0, 0) + m(1, 1)) + 1);

    z = R(0.5) * s;
    s = R(0.5) / s;
    x = (m(2, 0) + m(0, 2)) * s;
    y = (m(1, 2) + m(2, 1)) * s;
    w = (m(1, 0) - m(0, 1)) * s;
  }
  else
  {
    auto s = sqrt(m(0, 0) - (m(1, 1) + m(2, 2)) + 1);

    x = R(0.5) * s;
    s = R(0.5) / s;
    y = (m(0, 1) + m(1, 0)) * s;
    z = (m(2, 0) + m(0, 2)) * s;
    w = (m(0, 1) - m(1, 2)) * s;
  }
}

/// Returns s * m.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Matrix3x3<R>& m)
{
  return m * s;
}

using mat3f = Matrix3x3<float>;
using mat3d = Matrix3x3<double>;

} // end namespace cg

#endif // __Matrix3x3_h
