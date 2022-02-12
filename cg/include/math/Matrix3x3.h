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
// OVERVIEW: Matrix3x3.h
// ========
// Class definition for 3x3 matrix.
//
// Author: Paulo Pagliosa
// Last revision: 30/05/2020

#ifndef __Matrix3x3_h
#define __Matrix3x3_h

#include "math/Quaternion.h"

namespace cg
{ // begin namespace cg

template <typename real, int M, int N> class Matrix;


/////////////////////////////////////////////////////////////////////
//
// Matrix3x3: 3x3 matrix class (column-major format)
// =========
template <typename real>
class Matrix<real, 3, 3>
{
public:
  ASSERT_REAL(real, "Matrix3x3: floating point type expected");

  using quat = Quaternion<real>;
  using vec2 = Vector2<real>;
  using vec3 = Vector3<real>;
  using mat3 = Matrix<real, 3, 3>;
  using mat4 = Matrix<real, 4, 4>;
  using value_type = real;

  /// Default constructor.
  HOST DEVICE
  Matrix()
  {
    // do nothing
  }

  /// Constructs a Matrix3x3 object from [v0; v1; v2].
  HOST DEVICE
  Matrix(const vec3& v0, const vec3& v1, const vec3& v2)
  {
    set(v0, v1, v2);
  }

  /// Constructs a Matrix3x3 object from v[9].
  HOST DEVICE
  explicit Matrix(const real v[])
  {
    set(v);
  }

  /// Constructs a Matrix3x3 object as a multiple s of the identity matrix.
  HOST DEVICE
  explicit Matrix(real s)
  {
    set(s);
  }

  /// Constructs a Matrix3x3 object from the diagonal d.
  HOST DEVICE
  explicit Matrix(const vec3& d)
  {
    set(d);
  }

  /// Constructs a Matrix3x3 object from q.
  HOST DEVICE
  explicit Matrix(const quat& q)
  {
    set(q);
  }

  /// Constructs a Matrix3x3 object from m.
  HOST DEVICE
  explicit Matrix(const mat4& m)
  {
    set(m);
  }

  /// Sets this object to m.
  HOST DEVICE
  void set(const mat3& m)
  {
    *this = m;
  }

  /// Sets the columns of this object to [v0; v1; v2].
  HOST DEVICE
  void set(const vec3& v0, const vec3& v1, const vec3& v2)
  {
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
  }

  /// Sets the elements of this object from v[9].
  HOST DEVICE
  void set(const real v[])
  {
    v0.set(&v[0]);
    v1.set(&v[3]);
    v2.set(&v[6]);
  }

  /// Sets this object to a multiple s of the identity matrix.
  HOST DEVICE
  void set(real s)
  {
    v0.set(s, 0, 0);
    v1.set(0, s, 0);
    v2.set(0, 0, s);
  }

  /// Sets this object to a diagonal matrix d.
  HOST DEVICE
  void set(const vec3& d)
  {
    v0.set(d.x, 0, 0);
    v1.set(0, d.y, 0);
    v2.set(0, 0, d.z);
  }

  /// Sets the elements of this object from q.
  HOST DEVICE
  void set(const quat& q)
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

    v0.set((real)1 - (yy + zz), xy + zw, xz - yw);
    v1.set(xy - zw, (real)1 - (xx + zz), yz + xw);
    v2.set(xz + yw, yz - xw, (real)1 - (xx + yy));
  }

  /// Sets the elements of this object from m.
  HOST DEVICE
  void set(const mat4& m); // implemented in Matrix4x4.h

  HOST DEVICE
  mat3& operator =(const mat4& m)
  {
    set(m);
    return *this;
  }

  /// Returns a zero matrix.
  HOST DEVICE
  static mat3 zero()
  {
    return mat3{real(0)};
  }

  /// Returns an identity matrix.
  HOST DEVICE
  static mat3 identity()
  {
    return mat3{(real)1};
  }

  /// Returns a diagonal matrix d.
  HOST DEVICE
  static mat3 diagonal(const vec3& d)
  {
    return mat3{d};
  }

  /// Returns the diagonal of this object.
  HOST DEVICE
  vec3 diagonal() const
  {
    return vec3{v0.x, v1.y, v2.z};
  }

  /// Returns the trace of this object.
  HOST DEVICE
  real trace() const
  {
    return v0.x + v1.y + v2.z;
  }

  /// Returns a reference to the j-th column of this object.
  HOST DEVICE
  vec3& operator [](int j)
  {
    return (&v0)[j];
  }

  /// Returns the j-th column of this object.
  HOST DEVICE
  const vec3& operator [](int j) const
  {
    return (&v0)[j];
  }

  /// Returns a reference to the element (i, j) of this object.
  HOST DEVICE
  real& operator ()(int i, int j)
  {
    return (*this)[j][i];
  }

  /// Returns the element (i, j) of this object.
  HOST DEVICE
  const real& operator ()(int i, int j) const
  {
    return (*this)[j][i];
  }

  /// Returns this object * s.
  HOST DEVICE
  mat3 operator *(real s) const
  {
    return mat3{v0 * s, v1 * s, v2 * s};
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  mat3& operator *=(real s)
  {
    v0 *= s;
    v1 *= s;
    v2 *= s;
    return *this;
  }

  /// Returns this object * m.
  HOST DEVICE
  mat3 operator *(const mat3& m) const
  {
    const auto b0 = transform(m.v0);
    const auto b1 = transform(m.v1);
    const auto b2 = transform(m.v2);

    return mat3{b0, b1, b2};
  }

  /// Returns a reference to this object *= m.
  HOST DEVICE
  mat3& operator *=(const mat3& m)
  {
    return *this = operator *(m);
  }

  /// Returns this object * v.
  HOST DEVICE
  vec3 operator *(const vec3& v) const
  {
    return transform(v);
  }

  /// Returns the transposed of this object.
  HOST DEVICE
  mat3 transposed() const
  {
    const vec3 b0{v0.x, v1.x, v2.x};
    const vec3 b1{v0.y, v1.y, v2.y};
    const vec3 b2{v0.z, v1.z, v2.z};

    return mat3{b0, b1, b2};
  }

  /// Transposes and returns a reference to this object.
  HOST DEVICE
  mat3& transpose()
  {
    return *this = transposed();
  }

  /// \brief Tries to invert this object and returns true on success;
  /// otherwise, leaves this object unchanged and returns false.
  HOST DEVICE
  bool invert(real eps = math::Limits<real>::eps())
  {
    const auto b00 = v1[1] * v2[2] - v1[2] * v2[1];
    const auto b01 = v0[2] * v2[1] - v0[1] * v2[2];
    const auto b02 = v0[1] * v1[2] - v0[2] * v1[1];
    const auto b10 = v1[2] * v2[0] - v1[0] * v2[2];
    const auto b11 = v0[0] * v2[2] - v0[2] * v2[0];
    const auto b12 = v0[2] * v1[0] - v0[0] * v1[2];
    const auto b20 = v1[0] * v2[1] - v1[1] * v2[0];
    const auto b21 = v0[1] * v2[0] - v0[0] * v2[1];
    const auto b22 = v0[0] * v1[1] - v0[1] * v1[0];
    auto d = v0[0] * b00 + v1[0] * b01 + v2[0] * b02;

    if (math::isZero(d, eps))
      return false;
    d = real(1 / d);
    v0.set(d * b00, d * b01, d * b02);
    v1.set(d * b10, d * b11, d * b12);
    v2.set(d * b20, d * b21, d * b22);
    return true;
  }

  /// Assigns this object to m and tries to invert m.
  HOST DEVICE
  bool inverse(mat3& m, real eps = math::Limits<real>::eps()) const
  {
    return (m = *this).invert(eps);
  }

  /// Returns v transformed by this object.
  HOST DEVICE
  vec3 transform(const vec3& v) const
  {
    return v0 * v.x + v1 * v.y + v2 * v.z;
  }

  /// Returns v transformed by the transposed of this object.
  HOST DEVICE
  vec3 transposeTransform(const vec3& v) const
  {
    return vec3{v0.dot(v), v1.dot(v), v2.dot(v)};
  }

  /// \brief Returns a 2D point p transformed by this object.
  /// This method is slower than transform3x4, but can handle
  /// projective transformations as well.
  HOST DEVICE
  vec2 transform(const vec2& p) const
  {
    const auto r = transform(vec3{p, 1});
    return math::isZero(r.z) ? vec2(r) : vec2(r) * ((real)1 / r.z);
  }

  /// \brief Returns a 2D point p transformed by this object.
  /// This method is faster than transform, but it can solely
  /// handle affine 2D transformations.
  HOST DEVICE
  vec2 transform2x3(const vec2& p) const
  {
    const auto x = v0.x * p.x + v1.x * p.y + v2.x;
    const auto y = v0.y * p.x + v1.y * p.y + v2.y;

    return vec2{x, y};
  }

  /// Returns a vector v transformed by this object.
  HOST DEVICE
  vec3 transformVector(const vec2& v) const
  {
    return vec2(v0) * v.x + vec2(v1) * v.y;
  }

  /// Returns a pointer to the elements of this object.
  HOST DEVICE
  explicit operator const real*() const
  {
    return &v0.x;
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    fprintf(f, "[%9.4g %9.4g %9.4g]\n", v0.x, v1.x, v2.x);
    fprintf(f, "[%9.4g %9.4g %9.4g]\n", v0.y, v1.y, v2.y);
    fprintf(f, "[%9.4g %9.4g %9.4g]\n", v0.z, v1.z, v2.z);
  }

private:
  vec3 v0; // column 0
  vec3 v1; // column 1
  vec3 v2; // column 2

}; // Matrix3x3

/// Sets the coordinates of this object from m.
template <typename real>
HOST DEVICE void
Quaternion<real>::set(const mat3& m) // declared in Quaternion.h
{
  auto t = m.trace();

  if (t >= 0)
  {
    auto s = sqrt(t + 1);

    w = real(0.5) * s;
    s = real(0.5) / s;
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

    y = real(0.5) * s;
    s = real(0.5) / s;
    z = (m(1, 2) + m(2, 1)) * s;
    x = (m(0, 1) + m(1, 0)) * s;
    w = (m(0, 2) - m(2, 0)) * s;
  }
  else if (i == 2)
  {
    auto s = sqrt(m(2, 2) - (m(0, 0) + m(1, 1)) + 1);

    z = real(0.5) * s;
    s = real(0.5) / s;
    x = (m(2, 0) + m(0, 2)) * s;
    y = (m(1, 2) + m(2, 1)) * s;
    w = (m(1, 0) - m(0, 1)) * s;
  }
  else
  {
    auto s = sqrt(m(0, 0) - (m(1, 1) + m(2, 2)) + 1);

    x = real(0.5) * s;
    s = real(0.5) / s;
    y = (m(0, 1) + m(1, 0)) * s;
    z = (m(2, 0) + m(0, 2)) * s;
    w = (m(0, 1) - m(1, 2)) * s;
  }
}

template <typename real> using Matrix3x3 = Matrix<real, 3, 3>;

/// Returns s * m.
template <typename real>
HOST DEVICE inline Matrix3x3<real>
operator *(real s, const Matrix3x3<real>& m)
{
  return m * s;
}

using mat3f = Matrix3x3<float>;
using mat3d = Matrix3x3<double>;

} // end namespace cg

#endif // __Matrix3x3_h
