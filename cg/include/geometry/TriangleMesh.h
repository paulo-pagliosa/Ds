//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2020 Orthrus Group.                         |
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
// Last revision: 15/06/2020

#ifndef __TriangleMesh_h
#define __TriangleMesh_h

#include "core/SharedObject.h"
#include "geometry/Bounds3.h"
#include "graphics/Color.h"
#include <cstdint>

namespace cg
{ // begin namespace cg

namespace triangle
{ // begin namespace triangle

template <typename vec3f>
HOST DEVICE inline vec3f
normal(const vec3f& v0, const vec3f& v1, const vec3f& v2)
{
  return ((v1 - v0).cross(v2 - v0)).versor();
}

template <typename vec3f>
HOST DEVICE inline vec3f
normal(const vec3f* v)
{
  return normal(v[0], v[1], v[2]);
}

template <typename vec3f>
HOST DEVICE inline vec3f
normal(const vec3f* v, int i, int j, int k)
{
  return normal(v[i], v[j], v[k]);
}

template <typename vec3f>
HOST DEVICE inline vec3f
normal(const vec3f* v, const int i[3])
{
  return normal(v[i[0]], v[i[1]], v[i[2]]);
}

template <typename vec3f>
HOST DEVICE inline vec3f
center(const vec3f& v0, const vec3f& v1, const vec3f& v2)
{
  return (v0 + v1 + v2) * math::inverse(3.0f);
}

template <typename vec3f>
HOST DEVICE inline vec3f
center(const vec3f* v)
{
  return center(v[0], v[1], v[2]);
}

template <typename vec3f>
HOST DEVICE inline vec3f
center(const vec3f* v, int i, int j, int k)
{
  return center(v[i], v[j], v[k]);
}

template <typename vec3f>
HOST DEVICE inline vec3f
center(const vec3f* v, const int i[3])
{
  return center(v[i[0]], v[i[1]], v[i[2]]);
}

template <typename vec3f, typename T>
HOST DEVICE inline T
interpolate(const vec3f& p, const T& v0, const T& v1, const T& v2)
{
  return v0 * p.x + v1 * p.y + v2 * p.z;
}

template <typename vec3f, typename T>
HOST DEVICE inline T
interpolate(const vec3f& p, const T v[3])
{
  return interpolate(p, v[0], v[1], v[2]);
}

} // end namespace triangle

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
  Reference<SharedObject> userData;

  /// Constructs a triangle mesh from data.
  TriangleMesh(Data&& data);

  /// Destructor.
  ~TriangleMesh();

  Bounds3f bounds() const;

  void computeNormals();
  void TRS(const mat4f& trs);

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

}; // TriangleMesh

} // end namespace cg

#endif // __TriangleMesh_h
