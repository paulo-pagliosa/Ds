//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: PolyMesh.h
// ========
// Class definition for vis poly mesh.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#ifndef __PolyMesh_h
#define __PolyMesh_h

#include "core/List.h"
#include "Transform.h"
#include "TriCellMesh.h"

namespace cg::vis
{ // begin namespace cg::vis

//
// Forward definition
//
class PolyMeshBuilder;

class PolyMeshHelper: public TransformableObject
{
public:
  Transform* transform()
  {
    return &_transform;
  }

protected:
  PolyMeshHelper():
    _transform{*this}
  {
    // do nothing
  }

  auto normalMatrix() const
  {
    return mat3f{_worldToLocal}.transposed();
  }

private:
  Transform _transform;

}; // PolyMeshHelper

class PolyMeshGeometry: public PolyMeshHelper
{
public:
  class Element
  {
  public:
    const Reference<TriangleMesh> mesh;
    const mat4f localToWorld;
    const mat3f normalMatrix;

    auto bounds() const
    {
      return Bounds3f{mesh->bounds(), localToWorld};
    }

  }; // Element

  static Reference<PolyMeshGeometry> New()
  {
    return new PolyMeshGeometry;
  }

  void addElement(const TriangleMesh& mesh)
  {
    _elements.add({&mesh, _localToWorld, normalMatrix()});
  }

  void clear()
  {
    _elements.clear();
  }

  auto elementCount() const
  {
    return (int)_elements.size();
  }

  const auto& elements() const
  {
    return _elements;
  }

private:
  List<Element> _elements;

  PolyMeshGeometry() = default;

}; // PolyMeshGeometry


/////////////////////////////////////////////////////////////////////
//
// PolyMesh: vis poly mesh class
// ========
class PolyMesh: public DataSet
{
public:
  class Instance
  {
  public:
    const Reference<PolyMeshGeometry> geometry;
    const mat4f localToWorld;
    const mat3f normalMatrix;
    Color color;

    Bounds3f bounds() const;

  }; // Instance

  Color instanceColor{Color::white};

  const auto& bounds() const
  {
    return _bounds;
  }

  auto instanceCount() const
  {
    return (int)_instances.size();
  }

  const auto& instances() const
  {
    return _instances;
  }

private:
  Bounds3f _bounds;
  List<Instance> _instances;

  PolyMesh() = default;

  friend PolyMeshBuilder;

}; // PolyMesh

} // end namespace cg::vis

#endif // __PolyMesh_h
