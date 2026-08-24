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
// OVERVIEW: Real.h
// ========
// Math macros/static functions.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026


#ifndef __Real_h
#define __Real_h

#include "math/RealLimits.h"
#include <cmath>
#include <numbers>

namespace cg::math
{ // begin namespace cg::math

/// Returns the absolute value of x.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
abs(R x)
{
  return fabs(x);
}

/// Returns the signal of x.
template <typename T>
[[nodiscard]] HOST DEVICE constexpr auto
sign(T x)
{
  return static_cast<T>(x > 0 ? 1 : (x < 0 ? -1 : 0));
}

/// Returns the greater of x and y.
template <typename T>
[[nodiscard]] HOST DEVICE constexpr auto
max(T x, T y)
{
  return x > y ? x : y;
}

/// Returns the smaller of x and y.
template <typename T>
[[nodiscard]] HOST DEVICE constexpr auto
min(T x, T y)
{
  return x < y ? x : y;
}

/// Returns true if x is close to zero.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isZero(R x, R eps = Limits<R>::eps())
{
  return abs(x) <= eps;
}

/// Returns true if x is close to y.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isEqual(R x, R y, R eps = Limits<R>::eps())
{
  return isZero(x - y, eps);
}

/// Returns true if x is positive.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isPositive(R x, R eps = Limits<R>::eps())
{
  return x > +eps;
}

/// Returns true if x is negative.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isNegative(R x, R eps = Limits<R>::eps())
{
  return x < -eps;
}

/// Returns true if (x, y) is close to null.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isNull(R x, R y, R eps)
{
  return isZero(x, eps) && isZero(y, eps);
}

/// Returns true if (x, y, z) is close to null.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isNull(R x, R y, R z, R eps)
{
  return isNull(x, y, eps) && isZero(z, eps);
}

/// Returns true if (x, y, z, w) is close to null.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr bool
isNull(R x, R y, R z, R w, R eps)
{
  return isNull(x, y, z, eps) && isZero(w, eps);
}

/// Returns 1 / x.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
inverse(R x)
{
  return static_cast<R>(1 / x);
}

/// Returns pi.
template <IsReal R>
inline constexpr auto pi = std::numbers::pi_v<R>;

/// Returns x in radians.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
toRadians(R x)
{
  return static_cast<R>(x * pi<R> / 180);
}

/// Returns x in degrees.
template <IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
toDegrees(R x)
{
  return static_cast<R>(x * 180 / pi<R>);
}

/// Returns x ^ 2.
template <typename T>
[[nodiscard]] HOST DEVICE constexpr auto
sqr(T x)
{
  return x * x;
}

/// Returns x ^ 3.
template <typename T>
[[nodiscard]] HOST DEVICE constexpr auto
cube(T x)
{
  return x * x * x;
}

/// Returns a <= x <= b.
template <typename T>
[[nodiscard]] HOST DEVICE constexpr auto
clamp(T x, T a, T b)
{
  return x < a ? a : (x > b ? b : x);
}

/// Returns linearly interpolated value between x and y.
template <typename T, IsReal R>
[[nodiscard]] HOST DEVICE constexpr auto
lerp(const T& x, const T& y, R t)
{
  return x * (1 - t) + y * t;
}

/// Swaps the values of x and y.
template <typename T>
HOST DEVICE inline void
swap(T& x, T& y)
{
  auto t = x;

  x = y;
  y = t;
}

} // end namespace math::cg

#endif // __Real_h
