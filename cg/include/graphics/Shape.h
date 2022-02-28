//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: Shape.h
// ========
// Class definition for generic shape.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

#ifndef __Shape_h
#define __Shape_h

#include "core/Array.h"
#include "core/Exception.h"
#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include "geometry/Intersection.h"
#include <cassert>

namespace cg
{ // begin namespace cg

class TriangleMesh;


/////////////////////////////////////////////////////////////////////
//
// Shape: generic shape class
// =====
class Shape: public virtual SharedObject
{
public:
  virtual const TriangleMesh* tesselate() const;
  virtual bool canIntersect() const;

  bool intersect(const Ray3f& ray) const
  {
    assert(canIntersect());
    return localIntersect(ray);
  }

  bool intersect(const Ray3f& ray, Intersection& hit) const
  {
    assert(canIntersect());
    return localIntersect(ray, hit);
  }

  virtual vec3f normal(const Intersection&) const;
  virtual Bounds3f bounds() const;

protected:
  virtual bool localIntersect(const Ray3f&) const;
  virtual bool localIntersect(const Ray3f&, Intersection&) const;

}; // Shape

} // end namespace cg

#endif // __Shape_h
