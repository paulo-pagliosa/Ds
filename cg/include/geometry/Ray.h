//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2026 Paulo Pagliosa.                        |
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
//  OVERVIEW: Ray.h
//  ========
//  Class definition for 2D/3D ray.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Ray_h
#define __Ray_h

#include "geometry/Point3.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Ray: ray class
// ===
template <IsReal R, int D>
class Ray
{
public:
  static_assert(D == 2 || D == 3);

  using vec_type = Vector<R, D>;

  vec_type origin;
  vec_type direction;
  mutable R tMin;
  mutable R tMax;

  /// Constructs an empty Ray.
  constexpr Ray() = default;

  /// Constructs a Ray from origin and direction.
  HOST DEVICE
  constexpr Ray(const vec_type& origin, const vec_type& direction):
    tMin{R(0)},
    tMax{math::Limits<R>::inf()}
  {
    set(origin, direction);
  }

  /// Sets this object from origin and direction.
  HOST DEVICE
  constexpr void set(const vec_type& origin, const vec_type& direction)
  {
    this->origin = origin;
    this->direction = direction.versor();
  }

  /// Returns the point on the ray at distance t.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator ()(R t) const
  {
    return origin + direction * t;
  }

}; // Ray

template <typename R> using Ray2 = Ray<R, 2>;
template <typename R> using Ray3 = Ray<R, 3>;

using Ray2f = Ray2<float>;
using Ray2d = Ray2<double>;
using Ray3f = Ray3<float>;
using Ray3d = Ray3<double>;

} // end namespace cg

#endif // __Ray_h
