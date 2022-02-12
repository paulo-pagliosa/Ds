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
// OVERVIEW: PrimitiveBVH.h
// ========
// Class definition for primitive BVH.
//
// Author: Paulo Pagliosa
// Last revision: 24/01/2022

#ifndef __PrimitiveBVH_h
#define __PrimitiveBVH_h

#include "geometry/BVH.h"
#include "graphics/Primitive.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PrimitiveBVH: primitive BVH class
// ============
class PrimitiveBVH final: public Aggregate
{
public:
  using PrimitiveArray = typename BVH<Primitive>::PrimitiveArray;

  PrimitiveBVH(PrimitiveArray&& primitives):
    _bvh{new BVH<Primitive>{std::move(primitives)}}
  {
    // do nothing
  }

  auto& primitives() const
  {
    return _bvh->primitives();
  }

  Bounds3f bounds() const override;

private:
  Reference<BVH<Primitive>> _bvh;

  bool localIntersect(const Ray3f&) const override;
  bool localIntersect(const Ray3f&, Intersection&) const override;

}; // PrimitiveBVH

} // end namespace cg

#endif // __PrimitiveBVH_h
