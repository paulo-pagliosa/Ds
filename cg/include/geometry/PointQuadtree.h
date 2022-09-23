//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: PointQuadtree.h
// ========
// Class definition for generic point quadtree.
//
// Author: Paulo Pagliosa
// Last revision: 19/09/2022

#ifndef __PointQuadtree_h
#define __PointQuadtree_h

#include "geometry/PointTreeBase.h"
#include "geometry/Quadtree.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointQuadtree: generic point quadtree class
// =============
template <typename real, typename PA, typename IL = IndexList<>>
using PointQuadtree = PointTree<2, real, PA, IL>;

} // namespace cg

#endif // __PointQuadtree_h
