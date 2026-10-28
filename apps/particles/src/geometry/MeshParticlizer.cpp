//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: MeshParticlizer.cpp
// ========
// Source file for mesh particlizer.
//
// Author: Paulo Pagliosa
// Last revision: 28/10/2026

#include "geometry/MeshParticlizer.h"

namespace cg
{ // begin namespace cg

namespace triangle
{ // begin namespace cg::triangle

class LocalFrame
{
public:
  LocalFrame(const vec3f& p0, const vec3f& p1, const vec3f& p2)
  {
    o = p0;
    u = p1 - p0;

    auto t = u.squaredNorm();
    auto e = p2 - p1;
    auto s = e.squaredNorm();

    if (s > t)
      o = p1, u = e, t = s;
    e = p0 - p2;
    s = e.squaredNorm();
    if (s > t)
      o = p2, u = e, t = s;
    u *= math::inverse(sqrt(t));
    n = normal(p0, p1, p2);
    v = n.cross(u);
  }

  [[nodiscard]] auto worldToLocal(const vec3f& p) const
  {
    auto q = p - o;
    return vec3f{u.dot(q), v.dot(q), n.dot(q)};
  }

  [[nodiscard]] auto localToWorld(const vec3f& p) const
  {
    auto x = u.x * p.x + v.x * p.y + n.x * p.z + o.x;
    auto y = u.y * p.x + v.y * p.y + n.y * p.z + o.y;
    auto z = u.z * p.x + v.z * p.y + n.z * p.z + o.z;

    return vec3f{x, y, z};
  }

private:
  vec3f o;
  vec3f u;
  vec3f v;
  vec3f n;

}; // LocalFrame

} // end namespace cg::triangle

class EdgeTable
{
public:
  EdgeTable(const vec3f& p0, const vec3f& p1, const vec3f& p2, float d):
    _d{d}
  {
    _entries[0].x = _entries[1].x = _y = math::Limits<float>::inf();
    if (makeEdge(p2, p0, makeEdge(p1, p2, makeEdge(p0, p1))) != 2)
      throw std::runtime_error("EdgeTable: bad triangle");
  }

  [[nodiscard]] bool hasNextSpan()
  {
    return (_y += _d) < _entries->maxY;
  }

  [[nodiscard]] auto nextSpan(float& x0, float& x1)
  {
    x0 = _entries[0].update();
    x1 = _entries[1].update();
    return _y;
  }

private:
  struct Entry
  {
    float maxY;
    float deltaX;
    float x;

    float update()
    {
      return x += deltaX;
    }

  }; // Entry

  Entry _entries[2];
  float _d;
  float _y;
  
  int makeEdge(const vec3f& p0, const vec3f& p1, int index = 0)
  {
    if (index > 1)
      return index;

    const auto y0 = p0.y;
    const auto y1 = p1.y;
    const auto dy = y1 - y0;

    if (math::isZero(dy))
      return index;

    const auto x0 = p0.x;
    const auto x1 = p1.x;
    auto& edge = _entries[index];

    edge.deltaX = (x1 - x0) / dy * _d;

    float minY;

    if (dy < 0)
      minY = y1, edge.maxY = y0, edge.x = x1;
    else
      minY = y0, edge.maxY = y1, edge.x = x0;
    if (minY < _y)
      _y = minY;
    return index + 1;
  }

}; // EdgeTable


/////////////////////////////////////////////////////////////////////
//
// MeshParticlizer implementation
// ===============
void
MeshParticlizer::particlizeEdge(int v0, int v1, float d)
{
  if (auto [it, inserted] = _edges.emplace(v0, v1); inserted)
  {
    const auto& p = _points[v0];
    auto u = _points[v1] - p;
    auto t = u.length();

    if (auto n = (int)(t / d - 1); n > 0)
    {
      u *= 1 / t;
      t = (t - d) / n;
      for (int i = 0; i < n; i++, d += t)
        _points.add(p + u * d);
    }
  }
}

void
MeshParticlizer::particlizeFace(int v0, int v1, int v2, float d)
{
  auto p0 = _points[v0];
  auto p1 = _points[v1];
  auto p2 = _points[v2];
  triangle::LocalFrame lf{p0, p1, p2};

  p0 = lf.worldToLocal(p0);
  p1 = lf.worldToLocal(p1);
  p2 = lf.worldToLocal(p2);

  EdgeTable et{p0, p1, p2, d};

  while (et.hasNextSpan())
  {
    float x0;
    float x1;
    float y = et.nextSpan(x0, x1);
    auto u = x1 - x0;
    auto t = float(fabs(u));
    auto n = (int)(t / d - 1);

    if (n <= 0)
      continue;
    u /= t;
    t = (t - d) / n;

    auto s = d;

    for (int i = 0; i < n; i++, s += t)
      _points.add(lf.localToWorld({x0 + u * s, y}));
  }
}

const MeshParticlizer::PA&
MeshParticlizer::particlize(const TriangleMesh& mesh, float radius)
{
  _points.clear();
  _points.reserve(0x4000);

  const auto& data = mesh.data();

  for (int i = 0; i < data.vertexCount; ++i)
    _points.add(data.vertices[i]);

  const auto d = radius * 2;
  auto t = data.triangles;

  for (int i = 0; i < data.triangleCount; ++i, ++t)
  {
    const auto v0 = t->v[0];
    const auto v1 = t->v[1];
    const auto v2 = t->v[2];

    particlizeEdge(v0, v1, d);
    particlizeEdge(v1, v2, d);
    particlizeEdge(v2, v0, d);
    particlizeFace(v0, v1, v2, d);
  }
  _edges.clear();
  return _points;
}

} // end namespace cg