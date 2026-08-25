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
// OVERVIEW: Bounds3.h
// ========
// Class definition for 3D axis-aligned bounding box.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Bounds3_h
#define __Bounds3_h

#include "geometry/Bounds2.h"
#include "math/Matrix4x4.h"

namespace cg
{ // begin namespace cg

template <IsReal R>
HOST DEVICE constexpr void
extendBounds(Vector3<R>& p1, Vector3<R>& p2, R x, R y, R z)
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


/////////////////////////////////////////////////////////////////////
//
// Bounds3: 3D axis-aligned bounding box class
// =======
template <IsReal R>
class Bounds<R, 3>
{
public:
  ASSERT_REAL(R, "Bounds3: floating point type expected");

  using vec3 = Vector3<R>;
  using mat4 = Matrix4x4<R>;

  /// Constructs an empty Bounds3.
  HOST DEVICE
  constexpr Bounds()
  {
    setEmpty();
  }

  /// Constructs a Bounds3 from p1 and p2.
  HOST DEVICE
  constexpr Bounds(const vec3& p1, const vec3& p2)
  {
    set(p1, p2);
  }

  /// Constructs a Bounds3 from p1 and p2.
  template <typename V>
  HOST DEVICE
  constexpr Bounds(const V& p1, const V& p2):
    Bounds{vec3{p1}, vec3{p2}}
  {
    // do nothing
  }

  /// Constructs a Bounds3 from b and m.
  HOST DEVICE
  Bounds(const Bounds& b, const mat4& m):
    _p1{b._p1},
    _p2{b._p2}
  {
    transform(m);
  }

  /// Returns the center of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto center() const
  {
    return (_p1 + _p2) * R(0.5);
  }

  /// Returns the diagonal length this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto diagonalLength() const
  {
    return size().length();
  }

  /// Returns the extents of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto size() const
  {
    return _p2 - _p1;
  }

  /// Returns the maximum extent of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto maxExtent() const
  {
    return size().max();
  }

  /// Returns the area of this object.
  [[nodiscard]] HOST DEVICE
  constexpr auto area() const
  {
    const auto s = size();
    const auto a = s.x * (s.y + s.z) + s.y * s.z;

    return a + a;
  }

  /// Returns true if this object is empty.
  [[nodiscard]] HOST DEVICE
  constexpr bool empty() const
  {
    return _p1.x >= _p2.x || _p1.y >= _p2.y || _p1.z >= _p2.z;
  }

  /// Returns a reference to the min point of this object.
  [[nodiscard]] HOST DEVICE
  const auto& min() const
  {
    return _p1;
  }

  /// Returns a reference to the max point of this object.
  [[nodiscard]] HOST DEVICE
  const auto& max() const
  {
    return _p2;
  }

  /// Returns a reference to the min (0) ou max (1) point of this object.
  [[nodiscard]] HOST DEVICE
  const auto& operator [](int i) const
  {
    assert(i >= 0 && i < 2);
    return (&_p1)[i];
  }

  /// Returns the union of this object and b.
  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const Bounds& b) const
  {
    return Bounds{math::min(_p1, b._p1), math::max(_p2, b._p2)};
  }

  /// Sets this object to empty.
  HOST DEVICE
  constexpr void setEmpty()
  {
    _p1.x = _p1.y = _p1.z = +math::Limits<R>::inf();
    _p2.x = _p2.y = _p2.z = -math::Limits<R>::inf();
  }

  /// Sets this object from p1 and p2.
  HOST DEVICE
  constexpr void set(const vec3& p1, const vec3& p2)
  {
    _p1 = p1;
    _p2 = p2;
    if (p2.x < p1.x)
      math::swap(_p1.x, _p2.x);
    if (p2.y < p1.y)
      math::swap(_p1.y, _p2.y);
    if (p2.z < p1.z)
      math::swap(_p1.z, _p2.z);
  }

  /// Extends this object to contain (x, y).
  HOST DEVICE
  constexpr void extend(R x, R y, R z)
  {
    extendBounds(_p1, _p2, x, y, z);
  }

  /// Extends this object to contain p.
  template <typename V>
  HOST DEVICE
  constexpr void extend(const V& p)
  {
    extend(p.x, p.y, p.z);
  }

  /// Extends this object to contain b.
  HOST DEVICE
  constexpr void extend(const Bounds& b)
  {
    extend(b._p1);
    extend(b._p2);
  }

  /// Scales this object from its center by factor s.
  HOST DEVICE
  constexpr void scale(R s)
  {
    if (math::isPositive(s))
    {
      const auto c = center() * (1 - s);

      _p1 = _p1 * s + c;
      _p2 = _p2 * s + c;
    }
  }

  /// Transforms this object by m.
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
      extend(m.transform3x4(p));
    }
  }

  /// Returns true if this object contains p.
  [[nodiscard]] HOST DEVICE
  constexpr bool contains(const vec3& p) const
  {
    if (p.x < _p1.x || p.x > _p2.x)
      return false;
    if (p.y < _p1.y || p.y > _p2.y)
      return false;
    if (p.z < _p1.z || p.z > _p2.z)
      return false;
    return true;
  }

  /// Returns true if ray intercepts this object.
  [[nodiscard]] HOST DEVICE
  constexpr bool intersect(const Ray3<R>& ray, R& tMin, R& tMax) const
  {
    tMin = -math::Limits<R>::inf();
    tMax = +math::Limits<R>::inf();
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

  /// Returns true if this object overlaps b.
  [[nodiscard]] HOST DEVICE
  constexpr bool overlap(const Bounds& b) const
  {
    if (_p2.x < b._p1.x || _p1.x > b._p2.x)
      return false;
    if (_p2.y < b._p1.y || _p1.y > b._p2.y)
      return false;
    if (_p2.z < b._p1.z || _p1.z > b._p2.z)
      return false;
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

template <IsReal R> using Bounds3 = Bounds<R, 3>;

using Bounds3f = Bounds3<float>;
using Bounds3d = Bounds3<double>;

} // end namespace cg

#endif // __Bounds3_h
