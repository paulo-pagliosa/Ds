//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2026 Paulo Pagliosa.                        |
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
//  OVERVIEW: Quad.h
//  ========
//  Class definition for 2D/3D quad.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __Quad_h
#define __Quad_h

#include "geometry/Bounds3.h"
#include "geometry/Point3.h"

namespace cg
{ // begin namespace cg

template <int D, IsReal R> class Quad;


/////////////////////////////////////////////////////////////////////
//
// QuadBase: 2D/3D quad base class
// ========
template <int D, IsReal R>
class QuadBase
{
public:
  using Point = cg::Point<R, D>;

  Point p0;
  Point p1;
  Point p2;
  Point p3;

  auto operator ()(R u, R v) const
  {
    const auto u1 = R(1 - u);
    const auto v1 = R(1 - v);

    return u1 * v1 * p0 + u * v1 * p1 + u * v * p2 + u1 * v * p3;
  }

  auto bounds() const
  {
    Bounds<R, D> b;
    
    b.extend(p0);
    b.extend(p1);
    b.extend(p2);
    b.extend(p3);
    return b;
  }

}; // QuadBase


/////////////////////////////////////////////////////////////////////
//
// Quad2: 2D quad class
// =====
template <IsReal R>
class Quad<2, R>: public QuadBase<2, R>
{
public:
  using Point = Point2<R>;

  Quad() = default;

  Quad(const Point& p0, const Point& p2)
  {
    set(p0, p2);
  }

  Quad(const Point& center, R w, R h)
  {
    set(center, w, h);
  }

  void set(const Point& p0, const Point& p2)
  {
    this->p0.set(p0);
    this->p1.set(p2.x, p0.y);
    this->p2.set(p2);
    this->p3.set(p0.x, p2.y);
  }

  void set(const Point& center, R w, R h)
  {
    Point d{R(0.5) * w, R(0.5) * h};
    set(Point{center - d}, center + d);
  }

}; // Quad2

template <IsReal R> using Quad2 = Quad<2, R>;

using Quad2f = Quad2<float>;
using Quad2d = Quad2<double>;


/////////////////////////////////////////////////////////////////////
//
// Quad3: 3D quad class
// =====
template <IsReal R>
class Quad<3, R>: public QuadBase<3, R>
{
public:
  enum class Plane: int
  {
    XY, YX,
    XZ, ZX,
    YZ, ZY
  };

  using Point = Point3<R>;

  Quad() = default;

  Quad(Plane plane, const Point& center, R w, R h)
  {
    set(plane, center, w, h);
  }

  void set(Plane plane, const Point& center, R w, R h);

}; // Quad3

template <IsReal R>
void
Quad<3, R>::set(Plane plane, const Point& center, R w, R h)
{
  w *= R(0.5);
  h *= R(0.5);
  switch (plane)
  {
    case Plane::XY:
      this->p3.x = this->p0.x = center.x - w;
      this->p1.y = this->p0.y = center.y - h;
      this->p1.x = this->p2.x = center.x + w;
      this->p3.y = this->p2.y = center.y + h;
      this->p0.z = this->p1.z = this->p2.z = this->p3.z = center.z;
      break;
    case Plane::YX:
      this->p3.y = this->p0.y = center.y - w;
      this->p1.x = this->p0.x = center.x - h;
      this->p1.y = this->p2.y = center.y + w;
      this->p3.x = this->p2.x = center.x + h;
      this->p0.z = this->p1.z = this->p2.z = this->p3.z = center.z;
      break;
    case Plane::XZ:
      this->p3.x = this->p0.x = center.x - w;
      this->p1.z = this->p0.z = center.z - h;
      this->p1.x = this->p2.x = center.x + w;
      this->p3.z = this->p2.z = center.z + h;
      this->p0.y = this->p1.y = this->p2.y = this->p3.y = center.y;
      break;
    case Plane::ZX:
      this->p3.z = this->p0.z = center.z - w;
      this->p1.x = this->p0.x = center.x - h;
      this->p1.z = this->p2.z = center.z + w;
      this->p3.x = this->p2.x = center.x + h;
      this->p0.y = this->p1.y = this->p2.y = this->p3.y = center.y;
      break;
    case Plane::YZ:
      this->p3.y = this->p0.y = center.y - w;
      this->p1.z = this->p0.z = center.z - h;
      this->p1.y = this->p2.y = center.y + w;
      this->p3.z = this->p2.z = center.z + h;
      this->p0.x = this->p1.x = this->p2.x = this->p3.x = center.x;
      break;
    case Plane::ZY:
      this->p3.z = this->p0.z = center.z - w;
      this->p1.y = this->p0.y = center.y - h;
      this->p1.z = this->p2.z = center.z + w;
      this->p3.y = this->p2.y = center.y + h;
      this->p0.x = this->p1.x = this->p2.x = this->p3.x = center.x;
  }
}

template <IsReal R> using Quad3 = Quad<3, R>;

using Quad3f = Quad3<float>;
using Quad3d = Quad3<double>;

} // end namespace cg

#endif // __Quad_h
