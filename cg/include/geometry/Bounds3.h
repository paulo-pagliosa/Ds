//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2020 Orthrus Group.                         |
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
// OVERVIEW: Bounds3.h
// ========
// Class definition for 3D axis-aligned bounding box.
//
// Author: Paulo Pagliosa
// Last revision: 28/01/2020

#ifndef __Bounds3_h
#define __Bounds3_h

#include "geometry/Bounds2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Bounds3: 3D axis-aligned bounding box class
// =======
template <typename real>
class Bounds<real, 3>
{
public:
  ASSERT_REAL(real, "Bounds3: floating point type expected");

  using vec3 = Vector3<real>;
  using mat4 = Matrix4x4<real>;

  /// Constructs an empty Bounds3 object.
  HOST DEVICE
  Bounds()
  {
    setEmpty();
  }

  HOST DEVICE
  Bounds(const vec3& min, const vec3& max)
  {
    set(min, max);
  }

  HOST DEVICE
  Bounds(const Bounds<real, 3>& b, const mat4& m = mat4::identity()):
    _p1{b._p1},
    _p2{b._p2}
  {
    transform(m);
  }

  template <typename V>
  HOST DEVICE
  Bounds(const V& p1, const V& p2):
    Bounds{vec3(p1), vec3(p2)}
  {
    // do nothing
  }

  HOST DEVICE
  vec3 center() const
  {
    return (_p1 + _p2) * real(0.5);
  }

  HOST DEVICE
  real diagonalLength() const
  {
    return size().length();
  }

  HOST DEVICE
  vec3 size() const
  {
    return _p2 - _p1;
  }

  HOST DEVICE
  real maxSize() const
  {
    return size().max();
  }

  HOST DEVICE
  real area() const
  {
    const auto s = size();
    const auto a = s.x * (s.y + s.z) + s.y * s.z;

    return a + a;
  }

  HOST DEVICE
  bool empty() const
  {
    return _p1.x >= _p2.x || _p1.y >= _p2.y || _p1.z >= _p2.z;
  }

  HOST DEVICE
  const vec3& min() const
  {
    return _p1;
  }

  HOST DEVICE
  const vec3& max() const
  {
    return _p2;
  }

  HOST DEVICE
  const vec3& operator [](int i) const
  {
    return (&_p1)[i];
  }

  HOST DEVICE
  void setEmpty()
  {
    _p1.x = _p1.y = _p1.z = +math::Limits<real>::inf();
    _p2.x = _p2.y = _p2.z = -math::Limits<real>::inf();
  }

  HOST DEVICE
  void set(const vec3& min, const vec3& max)
  {
    _p1 = min;
    _p2 = max;
    if (max.x < min.x)
      math::swap(_p1.x, _p2.x);
    if (max.y < min.y)
      math::swap(_p1.y, _p2.y);
    if (max.z < min.z)
      math::swap(_p1.z, _p2.z);
  }

  HOST DEVICE
  static void inflate(vec3& p1, vec3& p2, real x, real y, real z)
  {
    if (x < p1.x)
      p1.x = x;
    if (x > p2.x)
      p2.x = x;
    if (y < p1.y)
      p1.y = y;
    if (y > p2.y)
      p2.y = y;
    if (z < p1.z)
      p1.z = z;
    if (z > p2.z)
      p2.z = z;
  }

  HOST DEVICE
  void inflate(real x, real y, real z = 0)
  {
    inflate(_p1, _p2, x, y, z);
  }

  template <typename V>
  HOST DEVICE
  void inflate(const V& p)
  {
    inflate(real(p.x), real(p.y), real(p.z));
  }

  HOST DEVICE
  void inflate(real s)
  {
    if (math::isPositive(s))
    {
      const auto c = center() * (1 - s);

      _p1 = _p1 * s + c;
      _p2 = _p2 * s + c;
    }
  }

  HOST DEVICE
  void inflate(const Bounds<real, 3>& b)
  {
    inflate(b._p1);
    inflate(b._p2);
  }

  HOST DEVICE
  void transform(const mat4& m)
  {
    const auto min = _p1;
    const auto max = _p2;

    setEmpty();
    for (int i = 0; i < 8; i++)
    {
      auto p = min;

      if (i & 1)
        p[0] = max[0];
      if (i & 2)
        p[1] = max[1];
      if (i & 4)
        p[2] = max[2];
      inflate(m.transform3x4(p));
    }
  }

  HOST DEVICE
  bool contains(const vec3& p) const
  {
    if (p.x < _p1.x || p.x > _p2.x)
      return false;
    if (p.y < _p1.y || p.y > _p2.y)
      return false;
    if (p.z < _p1.z || p.z > _p2.z)
      return false;
    return true;
  }

  HOST DEVICE
  bool intersect(const Ray3<real>& ray, real& tMin, real& tMax) const
  {
    tMin = -math::Limits<real>::inf();
    tMax = +math::Limits<real>::inf();
    for (int i = 0; i < 3; i++)
    {
      auto invDir = math::inverse(ray.direction[i]);
      auto t1 = (_p1[i] - ray.origin[i]) * invDir;
      auto t2 = (_p2[i] - ray.origin[i]) * invDir;

      if (t1 > t2)
        math::swap(t1, t2);
      tMin = t1 > tMin ? t1 : tMin;
      tMax = t2 < tMax ? t2 : tMax;
      if (tMin > tMax)
        return false;
    }
    return true;
  }

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    _p1.print("min: ", f);
    _p2.print("max: ", f);
  }

private:
  vec3 _p1;
  vec3 _p2;

}; // Bounds3

template <typename real> using Bounds3 = Bounds<real, 3>;

using Bounds3f = Bounds3<float>;
using Bounds3d = Bounds3<double>;

} // end namespace cg

#endif // __Bounds3_h
