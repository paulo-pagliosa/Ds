//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Intersection.h
// ========
// Class definition for intersection ray/object.
//
// Author: Paulo Pagliosa
// Last revision: 21/01/2022

#ifndef __Intersection_h
#define __Intersection_h

#include "geometry/Ray.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Intersection: intersection ray/object class
// ============
struct Intersection
{
  const void* object; // object intercepted by the ray
  int triangleIndex; // index of the triangle intercepted by the ray
  float distance; // distance from the ray's origin to the intersection point
  vec3f p; // barycentric coordinates of the intersection point
  void* userData; // any user data

}; // Intersection

} // end namespace cg

#endif // __Intersection_h
