//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: Shape.cpp
// ========
// Source file for generic shape.
//
// Author: Paulo Pagliosa
// Last revision: 08/12/2023

#include "graphics/Shape.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Shape implementation
// =====
const TriangleMesh*
Shape::tesselate() const
{
  return nullptr;
}

bool
Shape::canIntersect() const
{
  return true;
}

bool
Shape::localIntersect(const Ray3f&) const
{
  throw bad_invocation("Shape", __func__);
}

bool
Shape::localIntersect(const Ray3f&, Intersection&) const
{
  throw bad_invocation("Shape", __func__);
}

vec3f
Shape::normal(const Intersection&) const
{
  throw bad_invocation("Shape", __func__);
}

Bounds3f
Shape::bounds() const
{
  throw bad_invocation("Shape", __func__);
}

} // end namespace cg
