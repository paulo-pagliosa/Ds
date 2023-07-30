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
// OVERVIEW: Real.h
// ========
// Math macros/static functions.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#ifndef __Real_h
#define __Real_h

#include "math/RealLimits.h"
#include <cmath>
#include <numbers>

namespace cg::math
{ // begin namespace cg::math

/// Returns the absolute value of x.
template <typename real>
HOST DEVICE inline real
abs(real x)
{
  return fabs(x);
}

/// Returns the signal of x.
template <typename real>
HOST DEVICE inline constexpr real
sign(real x)
{
  return static_cast<real>(x > 0 ? 1 : (x < 0 ? -1 : 0));
}

/// Returns the greater of x and y.
template <typename real>
HOST DEVICE inline constexpr real
max(real x, real y)
{
  return x > y ? x : y;
}

/// Returns the smaller of x and y.
template <typename real>
HOST DEVICE inline constexpr real
min(real x, real y)
{
  return x < y ? x : y;
}

/// Returns true if x is close to zero.
template <typename real>
HOST DEVICE inline constexpr bool
isZero(real x, real eps = Limits<real>::eps())
{
  return abs(x) <= eps;
}

/// Returns true if x is close to y.
template <typename real>
HOST DEVICE inline constexpr bool
isEqual(real x, real y, real eps = Limits<real>::eps())
{
  return isZero(x - y, eps);
}

/// Returns true if x is positive.
template <typename real>
HOST DEVICE inline constexpr bool
isPositive(real x, real eps = Limits<real>::eps())
{
  return x > +eps;
}

/// Returns true if x is negative.
template <typename real>
HOST DEVICE inline constexpr bool
isNegative(real x, real eps = Limits<real>::eps())
{
  return x < -eps;
}

/// Returns true if (x, y) is close to null.
template <typename real>
HOST DEVICE inline constexpr bool
isNull(real x, real y, real eps)
{
  return isZero(x, eps) && isZero(y, eps);
}

/// Returns true if (x, y, z) is close to null.
template <typename real>
HOST DEVICE inline constexpr bool
isNull(real x, real y, real z, real eps)
{
  return isNull(x, y, eps) && isZero(z, eps);
}

/// Returns true if (x, y, z, w) is close to null.
template <typename real>
HOST DEVICE inline constexpr bool
isNull(real x, real y, real z, real w, real eps)
{
  return isNull(x, y, z, eps) && isZero(w, eps);
}

/// Returns 1 / x.
template <typename real>
HOST DEVICE inline constexpr real
inverse(real x)
{
  return static_cast<real>(1 / x);
}

/// Returns pi.
template <typename real>
inline constexpr real pi = std::numbers::pi_v<real>;

/// Returns x in radians.
template <typename real>
HOST DEVICE inline constexpr real
toRadians(real x)
{
  return static_cast<real>(x * pi<real> / 180);
}

/// Returns x in degrees.
template <typename real>
HOST DEVICE inline constexpr real
toDegrees(real x)
{
  return static_cast<real>(x * 180 / pi<real>);
}

/// Returns x ^ 2.
template <typename real>
HOST DEVICE inline constexpr real
sqr(real x)
{
  return x * x;
}

/// Returns x ^ 3.
template <typename real>
HOST DEVICE inline constexpr real
cube(real x)
{
  return x * x * x;
}

/// Swaps the values of x and y.
template <typename real>
HOST DEVICE inline void
swap(real& x, real& y)
{
  auto t = x;

  x = y;
  y = t;
}

/// Returns a <= x <= b.
template <typename real>
HOST DEVICE inline constexpr real
clamp(real x, real a, real b)
{
  return x < a ? a : (x > b ? b : x);
}

} // end namespace math::cg

#endif // __Real_h
