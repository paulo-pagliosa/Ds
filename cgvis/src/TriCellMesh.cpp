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
// OVERVIEW: TriCellMesh.cpp
// ========
// Soure file for vis triangle mesh.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#include "TriCellMesh.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// TriCellMesh implementation
// ===========
Reference<TriCellMesh::cell_type>
TriCellMesh::cell(int i)
{
  const auto& t = mesh()->data().triangles[i];
  return new cell_type{*this, t.v[0], t.v[1], t.v[2]};
}

const TriangleMesh*
TriCellMesh::tesselate() const
{
  return mesh();
}

vec3f
TriCellMesh::normal(const Intersection& hit) const
{
  return _normalMatrix.transform(_geometry->normal(hit)).versor();
}

Bounds3f
TriCellMesh::bounds() const
{
  return {_bounds, _localToWorld};
}

bool
TriCellMesh::localIntersect(const Ray3f& ray) const
{
  return _geometry->intersect(ray);
}

bool
TriCellMesh::localIntersect(const Ray3f& ray, Intersection& hit) const
{
  return _geometry->intersect(ray, hit) ?
    void(hit.object = this), true :
    false;
}

} // end namespace cg::vis
