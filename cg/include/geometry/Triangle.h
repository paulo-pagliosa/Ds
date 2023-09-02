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
// OVERVIEW: Triangle.h
// ========
// Class definition for triangle functions.
//
// Author: Paulo Pagliosa
// Last revision: 01/09/2023

#ifndef __Triangle_h
#define __Triangle_h

#include "geometry/Ray.h"

namespace cg
{ // begin namespace cg

namespace triangle
{ // begin namespace triangle

template <typename vec3>
HOST DEVICE inline vec3
normal(const vec3& v0, const vec3& v1, const vec3& v2)
{
  return ((v1 - v0).cross(v2 - v0)).versor();
}

template <typename vec3>
HOST DEVICE inline vec3
normal(const vec3* v)
{
  return normal(v[0], v[1], v[2]);
}

template <typename vec3>
HOST DEVICE inline vec3
normal(const vec3* v, int i, int j, int k)
{
  return normal(v[i], v[j], v[k]);
}

template <typename vec3>
HOST DEVICE inline vec3
normal(const vec3* v, const int i[3])
{
  return normal(v[i[0]], v[i[1]], v[i[2]]);
}

template <typename vec3>
HOST DEVICE inline vec3
center(const vec3& v0, const vec3& v1, const vec3& v2)
{
  return (v0 + v1 + v2) * math::inverse(3.0f);
}

template <typename vec3>
HOST DEVICE inline vec3
center(const vec3* v)
{
  return center(v[0], v[1], v[2]);
}

template <typename vec3>
HOST DEVICE inline vec3
center(const vec3* v, int i, int j, int k)
{
  return center(v[i], v[j], v[k]);
}

template <typename vec3>
HOST DEVICE inline vec3
center(const vec3* v, const int i[3])
{
  return center(v[i[0]], v[i[1]], v[i[2]]);
}

template <typename T, typename vec3>
HOST DEVICE inline T
interpolate(const vec3& b, const T& t0, const T& t1, const T& t2)
{
  return t0 * b.x + t1 * b.y + t2 * b.z;
}

template <typename T, typename vec3>
HOST DEVICE inline T
interpolate(const vec3& b, const T t[3])
{
  return interpolate(b, t[0], t[1], t[2]);
}

template <typename real>
HOST DEVICE inline bool
intersect(const Ray3<real>& ray,
  const Vector3<real>& p0,
  const Vector3<real>& p1,
  const Vector3<real>& p2,
  Vector3<real>& b,
  real& t)
{
  auto e1 = p1 - p0;
  auto e2 = p2 - p0;
  auto s1 = ray.direction.cross(e2);
  auto invDet = s1.dot(e1);

  if (math::isZero(invDet))
    return false;
  invDet = math::inverse(invDet);

  // Compute the first barycentric coordinate
  auto s = ray.origin - p0;
  auto b1 = s.dot(s1) * invDet;

  if (b1 < 0 || b1 > 1)
    return false;

  // Compute the second barycentric coordinate
  auto s2 = s.cross(e1);
  auto b2 = ray.direction.dot(s2) * invDet;

  if (b2 < 0 || b1 + b2 > 1)
    return false;
  // Compute the distance to the intersection point
  t = e2.dot(s2) * invDet;
  if (t < ray.tMin || t > ray.tMax)
    return false;
  b.set(1 - b1 - b2, b1, b2);
  return true;
}

} // end namespace triangle

} // end namespace cg

#endif // __Triangle_h
