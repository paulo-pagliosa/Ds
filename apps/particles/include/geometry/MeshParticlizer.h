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
// OVERVIEW: MeshParticlizer.h
// ========
// Class definition for mesh particlizer.
//
// Author: Paulo Pagliosa
// Last revision: 28/10/2026

#ifndef __MeshParticlizer_h
#define __MeshParticlizer_h

#include "core/DynamicArray.h"
#include "geometry/TriangleMesh.h"
#include <set>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// MeshParticlizer: mesh particlizer class
// ===============
class MeshParticlizer
{
public:
  using PA = DynamicArray<vec3f>;

  const PA& particlize(const TriangleMesh& mesh, float radius);

private:
  struct Edge
  {
    Edge() = default;

    Edge(int v0, int v1)
    {
      if (v0 < v1)
        v[0] = v0, v[1] = v1;
      else
        v[1] = v0, v[0] = v1;
    }

    [[nodiscard]] bool operator <(const Edge& e) const
    {
      if (v[0] != e.v[0])
        return v[0] < e.v[0];
      if (v[1] != e.v[1])
        return v[1] < e.v[1];
      return false;
    }

  private:
    int v[2];

  }; // Edge

  PA _points;
  std::set<Edge> _edges;

  void particlizeEdge(int, int, float);
  void particlizeFace(int, int, int, float);

}; // MeshParticlizer

} // end namespace cg

#endif // __MeshParticlizer_h
