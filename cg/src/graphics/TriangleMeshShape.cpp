//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Orthrus Group.                               |
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
// Last revision: 18/01/2022

#include "graphics/TriangleMeshShape.h"
#include <cassert>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshShape implementation
// =================
const TriangleMesh*
TriangleMeshShape::mesh() const
{
  return _mesh;
}

bool
TriangleMeshShape::canIntersect() const
{
  return false;
}

vec3f
TriangleMeshShape::normal(const Intersection& hit) const
{
  const auto& m = _mesh->data();
  auto tidx = hit.triangleIndex;

  assert(tidx >= 0 && tidx < m.triangleCount);

  auto v = m.triangles[tidx].v;
  const auto normals = m.vertexNormals;

  if (normals == nullptr)
    return triangle::normal(m.vertices, v);

  const auto& N0 = normals[v[0]];
  const auto& N1 = normals[v[1]];
  const auto& N2 = normals[v[2]];

  return triangle::interpolate(hit.p, N0, N1, N2).versor();
}

Bounds3f
TriangleMeshShape::bounds() const
{
  return _bounds;
}

} // end namespace cg
