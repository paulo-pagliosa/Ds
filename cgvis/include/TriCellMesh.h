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
// Class definition for vis tri cell mesh.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

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
class TriCellMeshMapper;


/////////////////////////////////////////////////////////////////////
//
// TriCellMesh: vis tri cell mesh class
// ===========
class TriCellMesh: public Primitive, public DataSet
{
public:
  using cell_type = TriCell<TriCellMesh>;

  TriCellMesh(const TriangleMesh& mesh):
    _geometry{new TriangleMeshShape{mesh}},
    _bounds{mesh.bounds()}
  {
    setMaterial(new Material(*material()));
  }

  auto mesh() const
  {
    return _geometry->mesh();
  }

  auto vertexCount() const
  {
    return (uint32_t)mesh()->data().vertexCount;
  }

  const auto& vertex(int i) const
  {
    return mesh()->data().vertices[i];
  }

  auto cellCount() const
  {
    return (uint32_t)mesh()->data().triangleCount;
  }

  Reference<cell_type> cell(int i);

  const TriangleMesh* tesselate() const override;
  virtual vec3f normal(const Intersection&) const override;;
  Bounds3f bounds() const override;

private:
  Reference<TriangleMeshShape> _geometry;
  Bounds3f _bounds;

  bool localIntersect(const Ray3f&) const override;
  bool localIntersect(const Ray3f&, Intersection&) const override;
  Object* clone() const override;

}; // TriCellMesh

} // end namespace cg::vis

#endif // __TriCellMesh_h
