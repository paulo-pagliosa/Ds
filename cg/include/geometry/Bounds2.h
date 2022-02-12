//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2020 Paulo Pagliosa.                        |
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
// OVERVIEW: Bounds2.h
// ========
// Class definition for 2D axis-aligned bounding box.
//
// Author: Paulo Pagliosa
// Last revision: 28/01/2020

#ifndef __Bounds2_h
#define __Bounds2_h

#include "geometry/Ray.h"

namespace cg
{ // begin namespace cg

template <typename real, int D> class Bounds;


/////////////////////////////////////////////////////////////////////
//
// Bounds2: 2D axis-aligned bounding box class
// =======
template <typename real>
class Bounds<real, 2>
{
public:
  ASSERT_REAL(real, "Bounds2: floating point type expected");

  using vec2 = Vector2<real>;
  using mat3 = Matrix3x3<real>;

  /// Constructs an empty Bounds3 object.
  HOST DEVICE
  Bounds()
  {
    setEmpty();
  }

  HOST DEVICE
  Bounds(const vec2& min, const vec2& max)
  {
    set(min, max);
  }

  template <typename V>
  HOST DEVICE
  Bounds(const V& p1, const V& p2):
    Bounds{vec2(p1), vec2(p2)}
  {
    // do nothing
  }

  HOST DEVICE
  Bounds(const Bounds<real, 2>& b, const mat3& m = mat3::identity()):
    _p1{b._p1},
    _p2{b._p2}
  {
    transform(m);
  }

  HOST DEVICE
  vec2 center() const
  {
    return (_p1 + _p2) * real(0.5);
  }

  HOST DEVICE
  real diagonalLength() const
  {
    return size().length();
  }

  HOST DEVICE
  vec2 size() const
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
    return (_p2.x - _p1.x) * (_p2.y - _p1.y);
  }

  HOST DEVICE
  bool empty() const
  {
    return _p1.x >= _p2.x || _p1.y >= _p2.y;
  }

  HOST DEVICE
  const vec2& min() const
  {
    return _p1;
  }

  HOST DEVICE
  const vec2& max() const
  {
    return _p2;
  }

  HOST DEVICE
  const vec2& operator [](int i) const
  {
    return (&_p1)[i];
  }

  HOST DEVICE
  void setEmpty()
  {
    _p1.x = _p1.y = +math::Limits<real>::inf();
    _p2.x = _p2.y = -math::Limits<real>::inf();
  }

  HOST DEVICE
  void set(const vec2& min, const vec2& max)
  {
    _p1 = min;
    _p2 = max;
    if (max.x < min.x)
      math::swap(_p1.x, _p2.x);
    if (max.y < min.y)
      math::swap(_p1.y, _p2.y);
  }

  HOST DEVICE
  static void inflate(vec2& p1, vec2& p2, real x, real y)
  {
    if (x < p1.x)
      p1.x = x;
    if (x > p2.x)
      p2.x = x;
    if (y < p1.y)
      p1.y = y;
    if (y > p2.y)
      p2.y = y;
  }

  HOST DEVICE
  void inflate(real x, real y)
  {
    inflate(_p1, _p2, x, y);
  }

  template <typename V>
  HOST DEVICE
  void inflate(const V& p)
  {
    inflate(real(p.x), real(p.y));
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
  void inflate(const Bounds<real, 2>& b)
  {
    inflate(b._p1);
    inflate(b._p2);
  }

  HOST DEVICE
  void transform(const mat3& m)
  {
    const auto min = _p1;
    const auto max = _p2;

    setEmpty();
    for (int i = 0; i < 4; i++)
    {
      auto p = min;

      if (i & 1)
        p[0] = max[0];
      if (i & 2)
        p[1] = max[1];
      inflate(m.transform2x3(p));
    }
  }

  HOST DEVICE
  bool contains(const vec2& p) const
  {
    if (p.x < _p1.x || p.x > _p2.x)
      return false;
    if (p.y < _p1.y || p.y > _p2.y)
      return false;
    return true;
  }

  HOST DEVICE
  bool intersect(const Ray<real, 2>& ray, real& tMin, real& tMax) const
  {
    tMin = -math::Limits<real>::inf();
    tMax = +math::Limits<real>::inf();
    for (int i = 0; i < 2; i++)
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

  /**
   * \brief Returns true if the line segment from \c p1 to \c p2
   * intersects this bounding box. The coordinates of the
   * intersection points are returned in \c q1 and \c q2 (if not
   * null). The method implements the Cohen-Sutherland algorithm.
   */
  HOST DEVICE
  bool intersectLine(const vec2 p1,
    const vec2 p2,
    vec2* q1 = nullptr,
    vec2* q2 = nullptr) const;

  void print(const char* s, FILE* f = stdout) const
  {
    fprintf(f, "%s\n", s);
    _p1.print("min: ", f);
    _p2.print("max: ", f);
  }

private:
  vec2 _p1;
  vec2 _p2;

  HOST DEVICE
  int outcode(const vec2 &p) const
  {
    return ((p.x < _p1.x)) |
      ((p.x > _p2.x) << 1) |
      ((p.y < _p1.y) << 2) |
      ((p.y > _p2.y) << 3);
  }

}; // Bounds2

template <typename real>
bool Bounds<real, 2>::intersectLine(const vec2 p1,
  const vec2 p2,
  vec2* q1,
  vec2* q2) const
{
  for (;;)
  {
    auto out1 = outcode(p1);
    auto out2 = outcode(p2);

    if ((out1 | out2) == 0)
    {
      if (q1 != nullptr)
        *q1 = p1;
      if (q2 != nullptr)
        *q2 = p2;
      return true;
    }
    if ((out1 & out2) != 0)
      return false;

    int code;
    vec2 *p;

    if (!out1)
    {
      code = out2;
      p = &p2;
    }
    else
    {
      code = out1;
      p = &p1;
    }

    if (code & 8)
    {
      p->x = p1.x + (p2.x - p1.x) * (_p2.y - p1.y) / (p2.y - p1.y);
      p->y = _p2.y;
      continue;
    }
    if (code & 4)
    {
      p->x = p1.x + (p2.x - p1.x) * (_p1.y - p1.y) / (p2.y - p1.y);
      p->y = _p1.y;
      continue;
    }
    if (code & 2)
    {
      p->y = p1.y + (p2.y - p1.y) * (_p2.x - p1.x) / (p2.x - p1.x);
      p->x = _p2.x;
      continue;
    }
    if (code & 1)
    {
      p->y = p1.y + (p2.y - p1.y) * (_p1.x - p1.x) / (p2.x - p1.x);
      p->x = _p1.x;
    }
  }
}

template <typename real> using Bounds2 = Bounds<real, 2>;

using Bounds2f = Bounds2<float>;
using Bounds2d = Bounds2<double>;

} // end namespace cg

#endif // __Bounds2_h
