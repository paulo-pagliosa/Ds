//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Orthrus Group.                         |
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
// OVERVIEW: TriangleMeshBVH.h
// ========
// Class definition for triangle mesh BVH.
//
// Author: Paulo Pagliosa
// Last revision: 17/01/2022

#ifndef __TriangleMeshBVH_h
#define __TriangleMeshBVH_h

#include "graphics/BVH.h"
#include "graphics/TriangleMeshShape.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshBVH: triangle mesh BVH class
// ===============
class TriangleMeshBVH final: public BVHBase
{
public:
  TriangleMeshBVH(const Primitive&, uint32_t = 64);

  const TriangleMesh* mesh() const override;

private:
  Reference<Primitive> _primitive;
  Reference<TriangleMesh> _mesh;

  void intersectPrimitives(uint32_t first,
    uint32_t count,
    const Ray3f& ray,
    Intersection& hit) const override;

}; // TriangleMeshBVH

} // end namespace cg

#endif // __TriangleMeshBVH_h
