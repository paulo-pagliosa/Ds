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
// OVERVIEW: Matrix4x4.h
// ========
// Class definition for 4x4 matrix.
//
// Author: Paulo Pagliosa
// Last revision: 01/09/2026

#ifndef __Matrix4x4_h
#define __Matrix4x4_h

#include "math/Matrix3x3.h"
#include "math/Vector4.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Matrix4x4: 4x4 matrix class (column-major format)
// =========
template <IsReal R>
class Matrix<R, 4, 4>
{
public:
  using type = Matrix<R, 4, 4>;
  using value_type = R;
  using vec3 = Vector3<R>;
  using vec4 = Vector4<R>;
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;

  /// Default constructor.
  constexpr Matrix() = default;

  /// Constructs a Matrix4x4 from [v0; v1; v2; v3].
  HOST DEVICE
  constexpr Matrix(const vec4& v0,
    const vec4& v1,
    const vec4& v2,
    const vec4& v3)
  {
    set(v0, v1, v2, v3);
  }

  /// Constructs a Matrix4x4 as a multiple s of the identity matrix.
  HOST DEVICE
  explicit constexpr Matrix(R s)
  {
    set(s);
  }

  /// Constructs a Matrix4x4 from the diagonal d.
  HOST DEVICE
  explicit constexpr Matrix(const vec4& d)
  {
    set(d);
  }

  /// Constructs a Matrix4x4 from q and p.
  HOST DEVICE
  explicit constexpr Matrix(const quat& q, const vec3& p = vec3::null())
  {
    set(q, p);
  }

  /// Constructs a Matrix4x4 from r and p.
  HOST DEVICE
  explicit constexpr Matrix(const mat3& r, const vec3& p = vec3::null())
  {
    set(r, p);
  }

  /// Constructs a Matrix4x4 from v[16].
  HOST DEVICE
  explicit Matrix(const R v[])
  {
    assert(v);
    set(vec4{&v[0]}, vec4{&v[4]}, vec4{&v[8]}, vec4{&v[12]});
  }

  /// Sets the columns of this object to [v0; v1; v2; v3].
  HOST DEVICE
  constexpr void set(const vec4& v0,
    const vec4& v1,
    const vec4& v2,
    const vec4& v3)
  {
    _v0 = v0;
    _v1 = v1;
    _v2 = v2;
    _v3 = v3;
  }

  /// Sets this object to a multiply s of the identity matrix.
  HOST DEVICE
  constexpr void set(R s)
  {
    _v0.set(s, 0, 0, 0);
    _v1.set(0, s, 0, 0);
    _v2.set(0, 0, s, 0);
    _v3.set(0, 0, 0, s);
  }

  /// Sets this object to a diagonal matrix d.
  HOST DEVICE
  constexpr void set(const vec4& d)
  {
    _v0.set(d.x, 0, 0, 0);
    _v1.set(0, d.y, 0, 0);
    _v2.set(0, 0, d.z, 0);
    _v3.set(0, 0, 0, d.w);
  }

  /// Sets the elements of this object from q and p.
  HOST DEVICE
  constexpr void set(const quat& q, const vec3& p = vec3::null())
  {
    set(mat3{q}, p);
  }

  /// Sets the elements of this object from r and p.
  HOST DEVICE
  constexpr void set(const mat3& r, const vec3& p = vec3::null())
  {
    _v0.set(r[0]);
    _v1.set(r[1]);
    _v2.set(r[2]);
    _v3.set(p, 1);
  }

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
  static constexpr auto diagonal(const vec4& d)
  {
    return type{d};
  }

  /// Returns the diagonal of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto diagonal() const
  {
    return vec4{_v0.x, _v1.y, _v2.z, _v3.w};
  }

  /// Returns a reference to the j-th column of this object.
  [[nodiscard]] HOST DEVICE
  auto& operator [](int j)
  {
    assert(j >= 0 && j < 4);
    return (&_v0)[j];
  }

  /// Returns the j-th column of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int j) const
  {
    assert(j >= 0 && j < 4);
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
  auto operator *(R s) const
  {
    return type{_v0 * s, _v1 * s, _v2 * s, _v3 * s};
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(R s)
  {
    _v0 *= s;
    _v1 *= s;
    _v2 *= s;
    _v3 *= s;
    return *this;
  }

  /// Returns this object * m.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& m) const
  {
    const auto b0 = transform(m._v0);
    const auto b1 = transform(m._v1);
    const auto b2 = transform(m._v2);
    const auto b3 = transform(m._v3);

    return type{b0, b1, b2, b3};
  }

  /// Returns a reference to this object *= m.
  HOST DEVICE
  auto& operator *=(const type& m)
  {
    return *this = operator *(m);
  }

  /// Returns this object * v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const vec4& v) const
  {
    return transform(v);
  }

  /// Returns the transposed of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transposed() const
  {
    const vec4 b0{_v0.x, _v1.x, _v2.x, _v3.x};
    const vec4 b1{_v0.y, _v1.y, _v2.y, _v3.y};
    const vec4 b2{_v0.z, _v1.z, _v2.z, _v3.z};
    const vec4 b3{_v0.w, _v1.w, _v2.w, _v3.w};

    return type{b0, b1, b2, b3};
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
    auto det3 = [](R m0, R m1, R m2, R m3, R m4, R m5, R m6, R m7, R m8)
    {
      return
        m0 * (m4 * m8 - m5 * m7) -
        m1 * (m3 * m8 - m5 * m6) +
        m2 * (m3 * m7 - m4 * m6);
    };
    const auto a00 = _v0[0], a01 = _v0[1], a02 = _v0[2], a03 = _v0[3];
    const auto a10 = _v1[0], a11 = _v1[1], a12 = _v1[2], a13 = _v1[3];
    const auto a20 = _v2[0], a21 = _v2[1], a22 = _v2[2], a23 = _v2[3];
    const auto a30 = _v3[0], a31 = _v3[1], a32 = _v3[2], a33 = _v3[3];
    const auto b00 = +det3(a11, a21, a31, a12, a22, a32, a13, a23, a33);
    const auto b01 = -det3(a01, a21, a31, a02, a22, a32, a03, a23, a33);
    const auto b02 = +det3(a01, a11, a31, a02, a12, a32, a03, a13, a33);
    const auto b03 = -det3(a01, a11, a21, a02, a12, a22, a03, a13, a23);
    auto d = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;

    if (math::isZero(d, eps))
      return false;
    d = R(1 / d);

    const auto b10 = -det3(a10, a20, a30, a12, a22, a32, a13, a23, a33);
    const auto b11 = +det3(a00, a20, a30, a02, a22, a32, a03, a23, a33);
    const auto b12 = -det3(a00, a10, a30, a02, a12, a32, a03, a13, a33);
    const auto b13 = +det3(a00, a10, a20, a02, a12, a22, a03, a13, a23);
    const auto b20 = +det3(a10, a20, a30, a11, a21, a31, a13, a23, a33);
    const auto b21 = -det3(a00, a20, a30, a01, a21, a31, a03, a23, a33);
    const auto b22 = +det3(a00, a10, a30, a01, a11, a31, a03, a13, a33);
    const auto b23 = -det3(a00, a10, a20, a01, a11, a21, a03, a13, a23);
    const auto b30 = -det3(a10, a20, a30, a11, a21, a31, a12, a22, a32);
    const auto b31 = +det3(a00, a20, a30, a01, a21, a31, a02, a22, a32);
    const auto b32 = -det3(a00, a10, a30, a01, a11, a31, a02, a12, a32);
    const auto b33 = +det3(a00, a10, a20, a01, a11, a21, a02, a12, a22);

    _v0.set(d * b00, d * b01, d * b02, d * b03);
    _v1.set(d * b10, d * b11, d * b12, d * b13);
    _v2.set(d * b20, d * b21, d * b22, d * b23);
    _v3.set(d * b30, d * b31, d * b32, d * b33);
    return true;
  }

  /// Assigns this object to m and tries to invert m.
  HOST DEVICE
  bool inverse(type& m, R eps = math::Limits<R>::eps()) const
  {
    return (m = *this).invert(eps);
  }

  /// Returns a position p transformed by this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transform(const vec4& p) const
  {
    return _v0 * p.x + _v1 * p.y + _v2 * p.z + _v3 * p.w;
  }

  /// \brief Returns a 3D point p transformed by this object.
  /// This method is slower than transform3x4, but can handle
  /// projective transformations as well.
  [[nodiscard]] HOST DEVICE
  constexpr auto transform(const vec3& p) const
  {
    const auto r = transform(vec4{p, 1});
    return math::isZero(r.w) ? vec3{r} : vec3{r} * ((R)1 / r.w);
  }

  /// \brief Returns a 3D point p transformed by this object.
  /// This method is faster than transform, but it can solely
  /// handle affine 3D transformations.
  [[nodiscard]] HOST DEVICE
  constexpr auto transform3x4(const vec3& p) const
  {
    const auto x = _v0.x * p.x + _v1.x * p.y + _v2.x * p.z + _v3.x;
    const auto y = _v0.y * p.x + _v1.y * p.y + _v2.y * p.z + _v3.y;
    const auto z = _v0.z * p.x + _v1.z * p.y + _v2.z * p.z + _v3.z;

    return vec3{x, y, z};
  }

  /// Returns a vector v transformed by this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transformVector(const vec3& v) const
  {
    return vec3{_v0} * v.x + vec3{_v1} * v.y + vec3{_v2} * v.z;
  }

  /// Returns a translation, rotation, and scaling matrix.
  [[nodiscard]] HOST DEVICE
  static constexpr auto TRS(const vec3& p, const quat& q, const vec3& s)
  {
    type m{q, p};

    m[0] *= s.x;
    m[1] *= s.y;
    m[2] *= s.z;
    return m;
  }

  [[nodiscard]] HOST DEVICE
  static constexpr auto TRS(const vec3& p, const vec3& angles, const vec3& s)
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
  [[nodiscard]] HOST DEVICE
  static constexpr auto rotation(const quat& q, const vec3& p)
  {
    mat3 r{q};
    return type{r, p - r * p};
  }

  [[nodiscard]] HOST DEVICE
  static constexpr auto rotation(const vec3& axis, R angle, const vec3& p)
  {
    return rotation(quat{angle, axis}, p);
  }

  HOST DEVICE
  void setRotation(const quat& q, const vec3& p)
  {
    *this = rotation(q, p);
  }

  HOST DEVICE
  void setRotation(const vec3& axis, R angle, const vec3& p)
  {
    setRotation(quat{angle, axis}, p);
  }

  /// Returns an orthographic parallel projection matrix.
  [[nodiscard]] HOST DEVICE
  static constexpr auto ortho(R left,
    R right,
    R bottom,
    R top,
    R zNear,
    R zFar)
  {
    type m{(R)1};

    m[0][0] = +R(2) / (right - left);
    m[1][1] = +R(2) / (top - bottom);
    m[2][2] = -R(2) / (zFar - zNear);
    m[3][0] = -(right + left) / (right - left);
    m[3][1] = -(top + bottom) / (top - bottom);
    m[3][2] = -(zFar + zNear) / (zFar - zNear);
    return m;
  }

  /// Returns a perspective projection matrix.
  [[nodiscard]] HOST DEVICE
  static constexpr auto frustum(R left,
    R right,
    R bottom,
    R top,
    R zNear,
    R zFar)
  {
    type m{(R)0};

    m[0][0] = (R(2) * zNear) / (right - left);
    m[1][1] = (R(2) * zNear) / (top - bottom);
    m[2][0] = (right + left) / (right - left);
    m[2][1] = (top + bottom) / (top - bottom);
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -R(1);
    m[3][2] = -R(2) * zFar * zNear / (zFar - zNear);
    return m;
  }

  /// \brief Returns a perspective projection matrix.
  /// fovy is the vertical field-of-view angle, in degrees; aspect
  /// is the aspect ratio (width divided by height); zFar and zFar
  /// set up the depth clipping planes (always positive).
  [[nodiscard]] HOST DEVICE
  static constexpr auto perspective(R fovy, R aspect, R zNear, R zFar)
  {
    const auto t = tan(math::toRadians(fovy) * R(0.5));
    type m{(R)0};

    m[0][0] = R(1) / (aspect * t);
    m[1][1] = R(1) / t;
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -R(1);
    m[3][2] = -R(2) * zFar * zNear / (zFar - zNear);
    return m;
  }

  /// \brief Returns a view matrix.
  /// eye is the position of the camera; center is the focal point;
  /// up is the view up vector.
  [[nodiscard]] HOST DEVICE
  static constexpr auto lookAt(const vec3& eye,
    const vec3& center,
    const vec3& up)
  {
    const auto n = (eye - center).versor();
    const auto u = up.cross(n).versor();
    const auto v = n.cross(u);
    type m{(R)1};

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
  [[nodiscard]] HOST DEVICE
  explicit operator const R*() const
  {
    return &_v0.x;
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", _v0.x, _v1.x, _v2.x, _v3.x);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", _v0.y, _v1.y, _v2.y, _v3.y);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", _v0.z, _v1.z, _v2.z, _v3.z);
    fprintf(f, "[%9.4g %9.4g %9.4g %9.4g]\n", _v0.w, _v1.w, _v2.w, _v3.w);
  }

private:
  vec4 _v0; // column 0
  vec4 _v1; // column 1
  vec4 _v2; // column 2
  vec4 _v3; // column 3

}; // Matrix4x4

template <IsReal R> using Matrix4x4 = Matrix<R, 4, 4>;

/// Returns s * m.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
operator *(R s, const Matrix4x4<R>& m)
{
  return m * s;
}

/// Sets the elements of this object from m.
template <IsReal R>
HOST DEVICE constexpr void
Matrix<R, 3, 3>::set(const Matrix4x4<R>& m) // declared in Matrix3x3.h
{
  _v0.set(m[0]);
  _v1.set(m[1]);
  _v2.set(m[2]);
}

using mat4f = Matrix4x4<float>;
using mat4d = Matrix4x4<double>;

} // end namespace cg

#endif // __Matrix4x4_h
