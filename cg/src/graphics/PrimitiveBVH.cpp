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
// OVERVIEW: PrimitiveBVH.cpp
// ========
// Souce file for primitive BVH.
//
// Author: Paulo Pagliosa
// Last revision: 24/01/2022

#include "graphics/PrimitiveBVH.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PrimitiveBVH implementation
// ============
bool
PrimitiveBVH::localIntersect(const Ray3f& ray) const
{
  return _bvh->intersect(ray);
}
  
bool
PrimitiveBVH::localIntersect(const Ray3f& ray, Intersection& hit) const
{
  return _bvh->intersect(ray, hit);
}

Bounds3f
PrimitiveBVH::bounds() const
{
  return _bvh->bounds();
}

} // end namespace cg
