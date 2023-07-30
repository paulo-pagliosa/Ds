//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: TriangleMesh.cpp
// ========
// Source file for simple triangle mesh.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "geometry/MeshSweeper.h"
#include <cstring>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMesh implementation
// ============
static uint32_t nextMeshId;

TriangleMesh::TriangleMesh(Data&& data):
  id{++nextMeshId},
  _data{data}
{
  memset(&data, 0, sizeof(Data));
}

TriangleMesh::~TriangleMesh()
{
  delete []_data.vertices;
  delete []_data.vertexNormals;
  delete []_data.uv;
  delete []_data.triangles;
}

const Bounds3f&
TriangleMesh::bounds() const
{
  if (_bounds.empty())
    for (int i = 0; i < _data.vertexCount; i++)
      _bounds.inflate(_data.vertices[i]);
  return _bounds;
}

void
TriangleMesh::computeNormals()
{
  auto nv = _data.vertexCount;

  if (_data.vertexNormals == nullptr)
    _data.vertexNormals = new vec3f[nv];

  auto t = _data.triangles;

  memset(_data.vertexNormals, 0, nv * sizeof(vec3f));
  for (int i = 0; i < _data.triangleCount; ++i, ++t)
  {
    auto v0 = t->v[0];
    auto v1 = t->v[1];
    auto v2 = t->v[2];
    auto normal = triangle::normal(_data.vertices, v0, v1, v2);

    _data.vertexNormals[v0] += normal;
    _data.vertexNormals[v1] += normal;
    _data.vertexNormals[v2] += normal;
  }
  for (int i = 0; i < nv; ++i)
    _data.vertexNormals[i].normalize();
}

void
TriangleMesh::TRS(const mat4f& trs)
{
  auto nv = _data.vertexCount;

  for (int i = 0; i < nv; ++i)
    _data.vertices[i] = trs.transform3x4(_data.vertices[i]);
  _bounds.setEmpty();
  if (_data.vertexNormals == nullptr)
    return;

  auto r = normalTRS(trs);

  for (int i = 0; i < nv; ++i)
    _data.vertexNormals[i] = (r * _data.vertexNormals[i]).versor();
}

void
TriangleMesh::normalize()
{
  auto b = bounds();
  auto c = b.center();
  auto s = b.size();
  auto m = 1 / s.max();
  auto v = _data.vertices;

  for (int i = 0; i < _data.vertexCount; ++i, ++v)
    *v = (*v - c) * m;
  s *= m * 0.5f;
  _bounds.set(-s, s);
}

static inline void
printv(const vec3f& p, FILE* f)
{
  fprintf(f, "%g,%g,%g", p.x, p.y, p.z);
}

static inline void
printv(const vec2f& p, FILE* f)
{
  fprintf(f, "%g,%g", p.x, p.y);
}

void
TriangleMesh::print(const char* s, FILE* f) const
{
  fprintf(f, "%s mesh\n{\n", s);
  fprintf(f, "  vertices %d\n  {\n",  _data.vertexCount);
  for (int i = 0; i < _data.vertexCount; ++i)
  {
    fprintf(f, "    %d ", i);
    printv(_data.vertices[i], f);
    if (_data.vertexNormals != nullptr)
    {
      fputc('/', f);
      printv(_data.vertexNormals[i], f);
    }
    if (_data.uv != nullptr)
    {
      fputc('/', f);
      printv(_data.uv[i], f);
    }
    fputc('\n', f);
  }
  fprintf(f, "  }\n  triangles %d\n  {\n", _data.triangleCount);

  auto t = _data.triangles;

  for (int i = 0; i < _data.triangleCount; ++i, ++t)
    fprintf(f, "    %d %d,%d,%d\n", i, t->v[0], t->v[1], t->v[2]);
  fprintf(f, "  }\n}\n");
}

} // end namespace cg
