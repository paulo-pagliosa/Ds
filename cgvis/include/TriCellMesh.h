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
// OVERVIEW: TriCellMesh.h
// ========
// Class definition for vis triangle mesh.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __TriCellMesh_h
#define __TriCellMesh_h

#include "graphics/Primitive.h"
#include "graphics/TriangleMeshShape.h"
#include "DataSet.h"
#include "TriCell.h"

namespace cg::vis
{ // begin namespace cg::vis

//
// Forward definition
//
class PolyMesh;


/////////////////////////////////////////////////////////////////////
//
// TriCellMesh: vis triangle mesh class
// ===========
class TriCellMesh: public Primitive, public DataSet
{
public:
  using cell_type = TriCell<TriCellMesh>;

  static Reference<TriCellMesh> New(const TriangleMesh& mesh, Material* m)
  {
    return new TriCellMesh{mesh, m};
  }

  static auto New(const TriangleMesh& mesh)
  {
    return New(mesh, nullptr);
  }

  auto mesh() const
  {
    return _geometry->mesh();
  }

  auto vertexCount() const
  {
    return (uint32_t)mesh()->data().vertexCount;
  }

  auto vertex(int i) const
  {
    return _localToWorld.transform3x4(meshVertex(i));
  }

  auto cellCount() const
  {
    return (uint32_t)mesh()->data().triangleCount;
  }

  Reference<cell_type> cell(int i);

  const TriangleMesh* tesselate() const override;
  virtual vec3f normal(const Intersection&) const override;;
  Bounds3f bounds() const override;

  auto clone() const
  {
    return new TriCellMesh{*this};
  }

private:
  Reference<TriangleMeshShape> _geometry;
  Bounds3f _bounds;

  TriCellMesh(const TriangleMesh& mesh, Material* m):
    _geometry{new TriangleMeshShape{mesh}},
    _bounds{mesh.bounds()}
  {
    setMaterial(m != nullptr ? m : material()->clone());
  }

  TriCellMesh(const TriCellMesh& other, Material* m = nullptr):
    _geometry{new TriangleMeshShape{*other.mesh()}},
    _bounds{other._bounds}
  {
    _localToWorld = other._localToWorld;
    _worldToLocal = other._worldToLocal;
    _normalMatrix = other._normalMatrix;
    setMaterial(m != nullptr ? m : other.material()->clone());
    cloneAttributes(other);
  }

  const vec3f& meshVertex(int i) const
  {
    return mesh()->data().vertices[i];
  }

  bool localIntersect(const Ray3f&) const override;
  bool localIntersect(const Ray3f&, Intersection&) const override;

  friend PolyMesh;

}; // TriCellMesh

} // end namespace cg::vis

#endif // __TriCellMesh_h
