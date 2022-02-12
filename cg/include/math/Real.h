//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// Last revision: 18/01/2022

#ifndef __Real_h
#define __Real_h

#include "math/RealLimits.h"
#include <cmath>

//
// Math macros
//
#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730950488016887242097
#endif
#define M_SQRT3 1.7320508075688772935274463415059
#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#define M_PI_2  (M_PI * 0.5)
#endif

namespace cg
{ // begin namespace cg

namespace math
{ // begin namespace math

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
  return abs<real>(x) <= eps;
}

/// Returns true if x is close to y.
template <typename real>
HOST DEVICE inline constexpr bool
isEqual(real x, real y, real eps = Limits<real>::eps())
{
  return isZero<real>(x - y, eps);
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
  return isZero<real>(x, eps) && isZero<real>(y, eps);
}

/// Returns true if (x, y, z) is close to null.
template <typename real>
HOST DEVICE inline constexpr bool
isNull(real x, real y, real z, real eps)
{
  return isNull<real>(x, y, eps) && isZero<real>(z, eps);
}

/// Returns true if (x, y, z, w) is close to null.
template <typename real>
HOST DEVICE inline constexpr bool
isNull(real x, real y, real z, real w, real eps)
{
  return isNull<real>(x, y, z, eps) && isZero<real>(w, eps);
}

/// Returns 1 / x.
template <typename real>
HOST DEVICE inline constexpr real
inverse(real x)
{
  return static_cast<real>(1 / x);
}

/// Returns x in radians.
template <typename real>
HOST DEVICE inline constexpr real
toRadians(real x)
{
  return static_cast<real>(x * M_PI / 180);
}

/// Returns x in degrees.
template <typename real>
HOST DEVICE inline constexpr real
toDegrees(real x)
{
  return static_cast<real>(x * 180 / M_PI);
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

/// Returns pi.
template <typename real>
HOST DEVICE inline constexpr real
pi()
{
  return (real)M_PI;
}

} // end namespace math

} // end namespace cg

#endif // __Real_h
