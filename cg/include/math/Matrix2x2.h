//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2025, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: Matrix2x2.h
// ========
// Class definition for 2x2 matrix.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Matrix2x2_h
#define __Matrix2x2_h

#include "math/Vector2.h"

namespace cg
{ // begin namespace cg

template <IsReal R, int M, int N> class Matrix;


/////////////////////////////////////////////////////////////////////
//
// Matrix2x2: 2x2 matrix class (column-major format)
// =========
template <IsReal R>
class Matrix<R, 2, 2>
{
public:
  using type = Matrix<R, 2, 2>;
  using value_type = R;
  using vec2 = Vector2<R>;

  /// Default constructor.
  constexpr Matrix() = default;

  /// Constructs a Matrix2x2 from [v0; v1].
  HOST DEVICE
  constexpr Matrix(const vec2& v0, const vec2& v1)
  {
    set(v0, v1);
  }

  /// Constructs a Matrix2x2 as a multiple s of the identity matrix.
  HOST DEVICE
  explicit constexpr Matrix(R s)
  {
    set(s);
  }

  /// Constructs a Matrix2x2 from the diagonal d.
  HOST DEVICE
  explicit constexpr Matrix(const vec2& d)
  {
    set(d);
  }

  /// Constructs a Matrix2x2 from v[4].
  HOST DEVICE
  explicit Matrix(const R v[])
  {
    assert(v);
    set(vec2{&v[0]}, vec2{&v[2]});
  }

  /// Sets the columns of this object to [v0; v1].
  HOST DEVICE
  constexpr void set(const vec2& v0, const vec2& v1)
  {
    _v0 = v0;
    _v1 = v1;
  }

  /// Sets this object to a multiple s of the identity matrix.
  HOST DEVICE
  constexpr void set(R s)
  {
    _v0.set(s, 0);
    _v1.set(0, s);
  }

  /// Sets this object to a diagonal matrix d.
  HOST DEVICE
  constexpr void set(const vec2& d)
  {
    _v0.set(d.x, 0);
    _v1.set(0, d.y);
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
  static constexpr auto diagonal(const vec2& d)
  {
    return type{d};
  }

  /// Returns the diagonal of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto diagonal() const
  {
    return vec2{_v0.x, _v1.y};
  }

  /// Returns the trace of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto trace() const
  {
    return _v0.x + _v1.y;
  }

  /// Returns a reference to the j-th column of this object.
  [[nodiscard]] HOST DEVICE
  auto& operator [](int j)
  {
    assert(j >= 0 && j < 2);
    return (&_v0)[j];
  }

  /// Returns the j-th column of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int j) const
  {
    assert(j >= 0 && j < 2);
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
    return type{_v0 * s, _v1 * s};
  }

  /// Returns a reference to this object *= s.
  HOST DEVICE
  auto& operator *=(R s)
  {
    _v0 *= s;
    _v1 *= s;
    return *this;
  }

  /// Returns this object * m.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const type& m) const
  {
    const auto b0 = transform(m._v0);
    const auto b1 = transform(m._v1);

    return type{b0, b1};
  }

  /// Returns a reference to this object *= m.
  HOST DEVICE
  auto& operator *=(const type& m)
  {
    return *this = operator *(m);
  }

  /// Returns this object * v.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator *(const vec2& v) const
  {
    return transform(v);
  }

  /// Returns the transposed of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transposed() const
  {
    const vec2 b0{_v0.x, _v1.x};
    const vec2 b1{_v0.y, _v1.y};

    return type{b0, b1};
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
    auto d = _v0[0] * _v1[1] - _v0[1] * _v1[0];

    if (math::isZero(d, eps))
      return false;
    d = math::inverse(d);

    auto b0 = vec2{+_v1[1], -_v0[1]} * d;
    auto b1 = vec2{-_v1[0], +_v0[0]} * d;

    _v0 = b0;
    _v1 = b1;
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
  constexpr auto transform(const vec2& v) const
  {
    return _v0 * v.x + _v1 * v.y;
  }

  /// Returns v transformed by the transposed of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto transposeTransform(const vec2& v) const
  {
    return vec2{_v0.dot(v), _v1.dot(v)};
  }

  /// Returns a pointer to the elements of this object.
  [[nodiscard]] HOST DEVICE
  explicit operator const R* () const
  {
    return &_v0.x;
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    fprintf(f, "[%9.4g %9.4g]\n", _v0.x, _v1.x);
    fprintf(f, "[%9.4g %9.4g]\n", _v0.y, _v1.y);
  }

private:
  vec2 _v0; // column 0
  vec2 _v1; // column 1

}; // Matrix2x2

template <IsReal R> using Matrix2x2 = Matrix<R, 2, 2>;

/// Returns s * m.
template <IsReal R>
HOST DEVICE constexpr auto
operator *(R s, const Matrix2x2<R>& m)
{
  return m * s;
}

using mat2f = Matrix2x2<float>;
using mat2d = Matrix2x2<double>;

} // end namespace cg

#endif // __Matrix2x2_h
