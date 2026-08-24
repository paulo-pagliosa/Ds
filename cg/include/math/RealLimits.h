//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: RealLimits.h
// ========
// Class definition for real number limits.
//
// Author: Paulo Pagliosa
// Last revision: 19/08/2026

#ifndef __RealLimits_h
#define __RealLimits_h

#include "core/Globals.h"
#include <cfloat>
#include <concepts>

namespace cg
{ // begin namespace cg

template <typename T>
concept IsReal = std::floating_point<T>;

#define ASSERT_REAL(T, msg) static_assert(IsReal<T>, msg)

namespace math
{ // begin namespace math


/////////////////////////////////////////////////////////////////////
//
// Limits: real number limits class
// ======
template <IsReal R> struct Limits;

template <>
struct Limits<float>
{
  [[nodiscard]] HOST DEVICE
  static constexpr float eps()
  {
    return FLT_EPSILON;
  }

  [[nodiscard]] HOST DEVICE
  static constexpr float inf()
  {
    return FLT_MAX;
  }

}; // Limits

template <>
struct Limits<double>
{
  [[nodiscard]] HOST DEVICE
  static constexpr double eps()
  {
    return DBL_EPSILON;
  }

  [[nodiscard]] HOST DEVICE
  static constexpr double inf()
  {
    return DBL_MAX;
  }

}; // Limits

} // end namespace math

} // end namespace cg

#endif // __RealLimits_h
