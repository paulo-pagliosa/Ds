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
// OVERVIEW: TriangleMeshBVH.cpp
// ========
// Source file for triangle mesh BVH.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#include "geometry/TriangleMeshBVH.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshBVH implementation
// ===============
TriangleMeshBVH::TriangleMeshBVH(const TriangleMesh& mesh, uint32_t maxt):
  BVHBase{maxt},
  _mesh{&mesh}
{
  const auto& m = _mesh->data();
  auto nt = (uint32_t)m.triangleCount;

  assert(nt > 0);
  _primitiveIds.resize(nt);

  PrimitiveInfoArray primitiveInfo(nt);

  for (uint32_t i = 0; i < nt; ++i)
  {
    _primitiveIds[i] = i;

    auto t = m.triangles + i;
    Bounds3f b;

    b.inflate(m.vertices[t->v[0]]);
    b.inflate(m.vertices[t->v[1]]);
    b.inflate(m.vertices[t->v[2]]);
    primitiveInfo[i] = {i, b};
  }
  build(primitiveInfo);
#ifdef _DEBUG
  if (true)
  {
    _mesh->bounds().print("Mesh bounds:");
    printf("Mesh triangles: %d\n", nt);
    bounds().print("BVH bounds:");
    printf("BVH nodes: %zd\n", size());
    /*
    iterate([this](const BVHNodeInfo& node)
    {
      if (!node.isLeaf)
        return;
      node.bounds.print("Leaf bounds:");
      printf("Leaf triangles: %d\n", node.count);
    });
    */
    putchar('\n');
  }
#endif // _DEBUG
}

bool
TriangleMeshBVH::intersectLeaf(uint32_t first,
  uint32_t count,
  const Ray3f& ray) const
{
  const auto& m = _mesh->data();

  for (auto i = first, e = i + count; i < e; ++i)
  {
    auto tid = _primitiveIds[i];
    auto v = m.triangles[tid].v;
    const auto& p0 = m.vertices[v[0]];
    const auto& p1 = m.vertices[v[1]];
    const auto& p2 = m.vertices[v[2]];
    vec3f b;
    float t;

    if (triangle::intersect(ray, p0, p1, p2, b, t))
      return true;
  }
  return false;
}

void
TriangleMeshBVH::intersectLeaf(uint32_t first,
  uint32_t count,
  const Ray3f& ray,
  Intersection& hit) const
{
  const auto& m = _mesh->data();
  auto hitCount = 0;

  for (auto i = first, e = i + count; i < e; ++i)
  {
    auto tid = _primitiveIds[i];
    auto v = m.triangles[tid].v;
    const auto& p0 = m.vertices[v[0]];
    const auto& p1 = m.vertices[v[1]];
    const auto& p2 = m.vertices[v[2]];
    vec3f b;
    float t;

    if (triangle::intersect(ray, p0, p1, p2, b, t) && t < hit.distance)
    {
      hit.triangleIndex = tid;
      hit.distance = t;
      hit.p = b;
      hitCount++;
    }
  }
  if (hitCount > 0)
    hit.object = _mesh;
}

} // end namespace cg
