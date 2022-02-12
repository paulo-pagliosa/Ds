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
// OVERVIEW: Matrix4x4.h
// ========
// Class definition for 4x4 matrix.
//
// Author: Paulo Pagliosa
// Last revision: 30/05/2020

#ifndef __Matrix4x4_h
#define __Matrix4x4_h

#include "math/Matrix3x3.h"
#include "math/Vector4.h"

namespace cg
{ // begin namespace cg

#define DET3(m0, m1, m2, m3, m4, m5, m6, m7, m8) ( \
  m0 * (m4 * m8 - m5 * m7) - \
  m1 * (m3 * m8 - m5 * m6) + \
  m2 * (m3 * m7 - m4 * m6))


/////////////////////////////////////////////////////////////////////
//
// Matrix4x4: 4x4 matrix class (column-major format)
// =========
template <typename real>
class Matrix<real, 4, 4>
{
public:
  ASSERT_REAL(real, "Matrix4x4: floating point type expected");

  using quat = Quaternion<real>;
  using vec3 = Vector3<real>;
  using mat3 = Matrix3x3<real>;
  using vec4 = Vector4<real>;
  using mat4 = Matrix<real, 4, 4>;
  using value_type = real;

  /// Default constructor.
  HOST DEVICE
  Matrix()
  {
    // do nothing
  }

  /// Constructs a Matrix4x4 object from [v0; v1; v2; v3].
  HOST DEVICE
  Matrix(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3)
  {
    set(v0, v1, v2, v3);
  }

  /// Constructs a Matrix4x4 object from v[16].
  HOST DEVICE
  explicit Matrix(const real v[])
  {
    set(v);
  }

  /// Constructs a Matrix4x4 object as a multiple s of the identity matrix.
  HOST DEVICE
  explicit Matrix(real s)
  {
    set(s);
  }

  /// Constructs a Matrix4x4 object from the diagonal d.
  HOST DEVICE
  explicit Matrix(const vec4& d)
  {
    set(d);
  }

  /// Constructs a Matrix4x4 object from q and p.
  HOST DEVICE
  explicit Matrix(const quat& q, const vec3& p = vec3::null())
  {
    set(q, p);
  }

  /// Constructs a Matrix4x4 object from r and p.
  HOST DEVICE
  explicit Matrix(const mat3& r, const vec3& p = vec3::null())
  {
    set(r, p);
  }

  /// Sets this object to m.
  HOST DEVICE
  void set(const mat4& m)
  {
    *this = m;
  }

  /// Sets the columns of this object to [v0; v1; v2; v3].
  HOST DEVICE
  void set(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3)
  {
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
  }

  /// Sets the elements of this object from v[16].
  HOST DEVICE
  void set(const real v[])
  {
    v0.set(&v[0x0]);
    v1.set(&v[0x4]);
    v2.set(&v[0x8]);
    v3.set(&v[0xc]);
  }

  /// Sets this object to a multiply s of the identity matrix.
  HOST DEVICE
  void set(real s)
  {
    v0.set(s, 0, 0, 0);
    v1.set(0, s, 0, 0);
    v2.set(0, 0, s, 0);
    v3.set(0, 0, 0, s);
  }

  /// Sets this object to a diagonal matrix d.
  HOST DEVICE
  void set(const vec4& d)
  {
    v0.set(d.x, 0, 0, 0);
    v1.set(0, d.y, 0, 0);
    v2.set(0, 0, d.z, 0);
    v3.set(0, 0, 0, d.w);
  }

  /// Sets the elements of this object from q and p.
  HOST DEVICE
  void set(const quat& q, const vec3& p = vec3::null())
  {
    set(mat3{q}, p);
  }

  /// Sets the elements of this object from r and p.
  HOST DEVICE
  void set(const mat3& r, const vec3& p = vec3::null())
  {
    v0 = r[0];
    v1 = r[1];
    v2 = r[2];
    v3.set(p, 1);
  }

  HOST DEVICE
  mat4& operator =(const mat3& m)
  {
    set(m);
    return *this;
  }

  /// Returns a zero matrix.
  HOST DEVICE
  static mat4 zero()
  {
    return mat4{(real)0};
  }

  /// Returns an identity matrix.
  HOST DEVICE
  static mat4 identity()
  {
    return mat4{(real)1};
  }

  /// Returns a diagonal matrix d.
  HOST DEVICE
  static mat4 diagonal(const vec4& d)
  {
    return mat4{d};
  }

  /// Returns the diagonal of this object.
  HOST DEVICE
  vec4 diagonal() const
  {
    return vec4{v0.x, v1.y, v2.z, v3.w};
  }

  /// Returns a reference to the j-th column of this object.
  HOST DEVICE
  vec4& operator [](int j)
  {
    return (&v0)[j];
  }

  /// Returns the j-th column of this object.
  HOST DEVICE
  const vec4& operator [](int j) const
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
  mat4 operator *(real s) const
  {
    return mat4{v0 * s, v1 * s, v2 * s, v3 * s};
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  mat4& operator *=(real s)
  {
    v0 *= s;
    v1 *= s;
    v2 *= s;
    v3 *= s;
    return *this;
  }

  /// Returns this object * m.
  HOST DEVICE
  mat4 operator *(const mat4& m) const
  {
    const auto b0 = transform(m.v0);
    const auto b1 = transform(m.v1);
    const auto b2 = transform(m.v2);
    const auto b3 = transform(m.v3);

    return mat4{b0, b1, b2, b3};
  }

  /// Returns a reference to this object *= m.
  HOST DEVICE
  mat4& operator *=(const mat4& m)
  {
    return *this = operator *(m);
  }

  /// Returns this object * v.
  HOST DEVICE
  vec4 operator *(const vec4& v) const
  {
    return transform(v);
  }

  /// Returns the transposed of this object.
  HOST DEVICE
  mat4 transposed() const
  {
    const vec4 b0{v0.x, v1.x, v2.x, v3.x};
    const vec4 b1{v0.y, v1.y, v2.y, v3.y};
    const vec4 b2{v0.z, v1.z, v2.z, v3.z};
    const vec4 b3{v0.w, v1.w, v2.w, v3.w};

    return mat4{b0, b1, b2, b3};
  }

  /// Transposes and returns a reference to this object.
  HOST DEVICE
  mat4& transpose()
  {
    return *this = transposed();
  }

  /// \brief Tries to invert this object and returns true on success;
  /// otherwise, leaves this object unchanged and returns false.
  HOST DEVICE
  bool invert(real eps = math::Limits<real>::eps())
  {
    const auto a00 = v0[0], a01 = v0[1], a02 = v0[2], a03 = v0[3];
    const auto a10 = v1[0], a11 = v1[1], a12 = v1[2], a13 = v1[3];
    const auto a20 = v2[0], a21 = v2[1], a22 = v2[2], a23 = v2[3];
    const auto a30 = v3[0], a31 = v3[1], a32 = v3[2], a33 = v3[3];
    const auto b00 = +DET3(a11, a21, a31, a12, a22, a32, a13, a23, a33);
    const auto b01 = -DET3(a01, a21, a31, a02, a22, a32, a03, a23, a33);
    const auto b02 = +DET3(a01, a11, a31, a02, a12, a32, a03, a13, a33);
    const auto b03 = -DET3(a01, a11, a21, a02, a12, a22, a03, a13, a23);
    auto d = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;

    if (math::isZero(d, eps))
      return false;
    d = real(1 / d);

    const auto b10 = -DET3(a10, a20, a30, a12, a22, a32, a13, a23, a33);
    const auto b11 = +DET3(a00, a20, a30, a02, a22, a32, a03, a23, a33);
    const auto b12 = -DET3(a00, a10, a30, a02, a12, a32, a03, a13, a33);
    const auto b13 = +DET3(a00, a10, a20, a02, a12, a22, a03, a13, a23);
    const auto b20 = +DET3(a10, a20, a30, a11, a21, a31, a13, a23, a33);
    const auto b21 = -DET3(a00, a20, a30, a01, a21, a31, a03, a23, a33);
    const auto b22 = +DET3(a00, a10, a30, a01, a11, a31, a03, a13, a33);
    const auto b23 = -DET3(a00, a10, a20, a01, a11, a21, a03, a13, a23);
    const auto b30 = -DET3(a10, a20, a30, a11, a21, a31, a12, a22, a32);
    const auto b31 = +DET3(a00, a20, a30, a01, a21, a31, a02, a22, a32);
    const auto b32 = -DET3(a00, a10, a30, a01, a11, a31, a02, a12, a32);
    const auto b33 = +DET3(a00, a10, a20, a01, a11, a21, a02, a12, a22);

    v0.set(d * b00, d * b01, d * b02, d * b03);
    v1.set(d * b10, d * b11, d * b12, d * b13);
    v2.set(d * b20, d * b21, d * b22, d * b23);
    v3.set(d * b30, d * b31, d * b32, d * b33);
    return true;
  }

  /// Assigns this object to m and tries to invert m.
  HOST DEVICE
  bool inverse(mat4& m, real eps = math::Limits<real>::eps()) const
  {
    return (m = *this).invert(eps);
  }

  /// Returns a position p transformed by this object.
  HOST DEVICE
  vec4 transform(const vec4& p) const
  {
    return v0 * p.x + v1 * p.y + v2 * p.z + v3 * p.w;
  }

  /// \brief Returns a 3D point p transformed by this object.
  /// This method is slower than transform3x4, but can handle
  /// projective transformations as well.
  HOST DEVICE
  vec3 transform(const vec3& p) const
  {
    const auto r = transform(vec4{p, 1});
    return math::isZero(r.w) ? vec3(r) : vec3(r) * ((real)1 / r.w);
  }

  /// \brief Returns a 3D point p transformed by this object.
  /// This method is faster than transform, but it can solely
  /// handle affine 3D transformations.
  HOST DEVICE
  vec3 transform3x4(const vec3& p) const
  {
    const auto x = v0.x * p.x + v1.x * p.y + v2.x * p.z + v3.x;
    const auto y = v0.y * p.x + v1.y * p.y + v2.y * p.z + v3.y;
    const auto z = v0.z * p.x + v1.z * p.y + v2.z * p.z + v3.z;

    return vec3{x, y, z};
  }

  /// Returns a vector v transformed by this object.
  HOST DEVICE
  vec3 transformVector(const vec3& v) const
  {
    return vec3(v0) * v.x + vec3(v1) * v.y + vec3(v2) * v.z;
  }

  /// Returns a translation, rotation, and scaling matrix.
  HOST DEVICE
  static mat4 TRS(const vec3& p, const quat& q, const vec3& s)
  {
    mat4 m{q, p};

    m[0] *= s.x;
    m[1] *= s.y;
    m[2] *= s.z;
    return m;
  }

  HOST DEVICE
  static mat4 TRS(const vec3& p, const vec3& angles, const vec3& s)
  {
    return TRS(p, quat::eulerAngles(angles), s);
  }

  /// Sets this object as a TRS matrix.
  HOST DEVICE
  void setTRS(const vec3& p, const quat& q, const vec3& s)
  {
    *this = TRS(p, q, s);
  }

  HOST DEVICE
  void setTRS(const vec3& p, const vec3& angles, const vec3& s)
  {
    setTRS(p, quat::eulerAngles(angles), s);
  }

  /// Returns a rotation matrix.
  HOST DEVICE
  static mat4 rotation(const quat& q, const vec3& p)
  {
    mat3 r{q};
    return mat4{r, p - r * p};
  }

  HOST DEVICE
  static mat4 rotation(const vec3& axis, real angle, const vec3& p)
  {
    return rotation(quat{angle, axis}, p);
  }

  HOST DEVICE
  void setRotation(const quat& q, const vec3& p)
  {
    *this = rotation(q, p);
  }

  HOST DEVICE
  void setRotation(const vec3& axis, real angle, const vec3& p)
  {
    setRotation(quat{angle, axis}, p);
  }

  /// Returns an orthographic parallel projection matrix.
  HOST DEVICE
  static mat4 ortho(real left,
    real right,
    real bottom,
    real top,
    real zNear,
    real zFar)
  {
    mat4 m{(real)1};

    m[0][0] = +real(2) / (right - left);
    m[1][1] = +real(2) / (top - bottom);
    m[2][2] = -real(2) / (zFar - zNear);
    m[3][0] = -(right + left) / (right - left);
    m[3][1] = -(top + bottom) / (top - bottom);
    m[3][2] = -(zFar + zNear) / (zFar - zNear);
    return m;
  }

  /// Returns a perspective projection matrix.
  HOST DEVICE
  static mat4 frustum(real left,
    real right,
    real bottom,
    real top,
    real zNear,
    real zFar)
  {
    mat4 m{(real)0};

    m[0][0] = (real(2) * zNear) / (right - left);
    m[1][1] = (real(2) * zNear) / (top - bottom);
    m[2][0] = (right + left) / (right - left);
    m[2][1] = (top + bottom) / (top - bottom);
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -real(1);
    m[3][2] = -real(2) * zFar * zNear / (zFar - zNear);
    return m;
  }

  /// \brief Returns a perspective projection matrix.
  /// fovy is the vertical field-of-view angle, in degrees; aspect
  /// is the aspect ratio (width divided by height); zFar and zFar
  /// set up the depth clipping planes (always positive).
  HOST DEVICE
  static mat4 perspective(real fovy, real aspect, real zNear, real zFar)
  {
    const auto t = tan(math::toRadians(fovy) * real(0.5));
    mat4 m{(real)0};

    m[0][0] = real(1) / (aspect * t);
    m[1][1] = real(1) / t;
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -real(1);
    m[3][2] = -real(2) * zFar * zNear / (zFar - zNear);
    return m;
  }

  /// \brief Returns a view matrix.
  /// eye is the position of the camera; center is the focal point;
  /// up is the view up vector.
  HOST DEVICE
  static mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up)
  {
    const auto n = (eye - center).versor();
    const auto u = up.cross(n).versor();
    const auto v = n.cross(u);
    mat4 m{(real)1};

    m[0][0] = u.x;
    m[1][0] = u.y;
    m[2][0] = u.z;
    m[0][1] = v.x;
    m[1][1] = v.y;
    m[2][1] = v.z;
    m[0][2] = n.x;
    m[1][2] = n.y;
    m[2][2] = n.z;
    m[3][0] = -u.dot(eye);
    m[3][1] = -v.dot(eye);
    m[3][2] = -n.dot(eye);
    return m;
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
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", v0.x, v1.x, v2.x, v3.x);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", v0.y, v1.y, v2.y, v3.y);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", v0.z, v1.z, v2.z, v3.z);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", v0.w, v1.w, v2.w, v3.w);
  }

private:
  vec4 v0; // column 0
  vec4 v1; // column 1
  vec4 v2; // column 2
  vec4 v3; // column 3

}; // Matrix4x4

template <typename real> using Matrix4x4 = Matrix<real, 4, 4>;

/// Returns s * m.
template <typename real>
HOST DEVICE inline Matrix4x4<real>
operator *(real s, const Matrix4x4<real>& m)
{
  return m * s;
}

/// Sets the elements of this object from m.
template <typename real>
HOST DEVICE inline void
Matrix<real, 3, 3>::set(const mat4& m) // declared in Matrix3x3.h
{
  v0 = m[0];
  v1 = m[1];
  v2 = m[2];
}

using mat4f = Matrix4x4<float>;
using mat4d = Matrix4x4<double>;

} // end namespace cg

#endif // __Matrix4x4_h
