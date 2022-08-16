//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: TriangleMesh.h
// ========
// Class definition for simple triangle mesh.
//
// Author: Paulo Pagliosa
// Last revision: 16/08/2022

#ifndef __TriangleMesh_h
#define __TriangleMesh_h

#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include "geometry/Triangle.h"
#include "graphics/Color.h"
#include <cstdint>

namespace cg
{ // begin namespace cg

template <typename real>
inline Matrix3x3<real>
normalTRS(const Matrix4x4<real>& trs)
{
  Matrix3x3<real> r{trs};

  r[0] *= math::inverse(r[0].squaredNorm());
  r[1] *= math::inverse(r[1].squaredNorm());
  r[2] *= math::inverse(r[2].squaredNorm());
  return r;
}


/////////////////////////////////////////////////////////////////////
//
// TriangleMesh: simple triangle mesh class
// ============
class TriangleMesh: public SharedObject
{
public:
  struct Triangle
  {
    int v[3];

    void setVertices(int v0, int v1, int v2)
    {
      v[0] = v0;
      v[1] = v1;
      v[2] = v2;
    }

  }; // Triangle

  struct Data
  {
    int vertexCount;
    int triangleCount;
    vec3f* vertices;
    vec3f* vertexNormals;
    vec2f* uv{}; // TODO
    Triangle* triangles;

  }; // Data

  const uint32_t id;
  mutable Reference<SharedObject> userData;

  /// Constructs a triangle mesh from data.
  TriangleMesh(Data&& data);

  /// Destructor.
  ~TriangleMesh();

  const Bounds3f& bounds() const;

  void computeNormals();
  void TRS(const mat4f& trs);
  void normalize();

  const Data& data() const
  {
    return _data;
  }

  bool hasVertexNormals() const
  {
    return _data.vertexNormals != nullptr;
  }

  bool hasUV() const
  {
    return _data.uv != nullptr;
  }

  void print(const char* s, FILE* f = stdout) const;

private:
  Data _data;
  mutable Bounds3f _bounds;

}; // TriangleMesh

} // end namespace cg

#endif // __TriangleMesh_h
