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
// OVERVIEW: TriangleMeshShape.h
// ========
// Class definition for triangle mesh shape.
//
// Author: Paulo Pagliosa
// Last revision: 21/06/2023

#ifndef __TriangleMeshShape_h
#define __TriangleMeshShape_h

#include "geometry/TriangleMeshBVH.h"
#include "graphics/Shape.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshShape: triangle mesh shape class
// =================
class TriangleMeshShape: public Shape
{
public:
  TriangleMeshShape(const TriangleMesh&);

  const TriangleMesh* tesselate() const override;
  bool canIntersect() const override;
  vec3f normal(const Intersection&) const override;
  Bounds3f bounds() const override;

  const TriangleMesh* mesh() const
  {
    return _mesh;
  }

  void setMesh(const TriangleMesh&);

protected:
  TriangleMeshBVH* bvh() const;

private:
  Reference<TriangleMesh> _mesh;
  mutable Reference<TriangleMeshBVH> _bvh;

  bool localIntersect(const Ray3f&) const final;
  bool localIntersect(const Ray3f&, Intersection&) const final;

}; // TriangleMeshShape

} // end namespace cg

#endif // __TriangleMeshShape_h
