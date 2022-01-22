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
// Last revision: 22/01/2022

#include "graphics/TriangleMeshShape.h"
#include <cassert>
#include <map>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshShape implementation
// =================
class BVHBuilder
{
public:
  using BVHMap = std::map<uint32_t, Reference<TriangleMeshBVH>>;

  TriangleMeshBVH* bvh(const TriangleMesh& mesh)
  {
    {
      auto mit = _map.find(mesh.id);

      if (_map.end() != mit)
        return mit->second;
    }
#ifdef _DEBUG
    printf("**Building BVH for mesh %d\n", mesh.id);
#endif // _DEBUG

    auto* bvh = new TriangleMeshBVH{mesh};

    _map.emplace(mesh.id, bvh);
    return bvh;
  }

private:
  BVHMap _map;

}; // BVHBuilder

static BVHBuilder _bvhBuilder;

TriangleMeshShape::TriangleMeshShape(const TriangleMesh& mesh):
  _bvh{_bvhBuilder.bvh(mesh)}
{
  // do nothing
}

void
TriangleMeshShape::setMesh(const TriangleMesh& mesh)
{
  if (this->mesh() != &mesh)
    _bvh = _bvhBuilder.bvh(mesh);
}

const TriangleMesh*
TriangleMeshShape::tesselate() const
{
  return mesh();
}

bool
TriangleMeshShape::canIntersect() const
{
  return true;
}

bool
TriangleMeshShape::localIntersect(const Ray3f& ray) const
{
  return _bvh->intersect(ray);
}

bool
TriangleMeshShape::localIntersect(const Ray3f& ray, Intersection& hit) const
{
  return _bvh->intersect(ray, hit) ? hit.object = this : false;
}

vec3f
TriangleMeshShape::normal(const Intersection& hit) const
{
  const auto& m = mesh()->data();
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
  return _bvh->bounds();
}

} // end namespace cg
