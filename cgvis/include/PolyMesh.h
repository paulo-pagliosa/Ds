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
// Last revision: 11/03/2022

#ifndef __PolyMesh_h
#define __PolyMesh_h

#include "core/List.h"
#include "Transform.h"
#include "TriCellMesh.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyMesh: vis poly mesh class
// ========
class PolyMesh: public TransformableObject, public DataSet
{
public:
  static Reference<PolyMesh> New()
  {
    return new PolyMesh;
  }

  Material* material() const
  {
    return _material;
  }

  void add(const TriCellMesh& mesh)
  {
    _elements.add(&mesh);
  }

  auto add(const PolyMesh& mesh)
  {
    _elements.add(&mesh);
  }

protected:
  using Elements = List<Reference<TransformableObject>>;

  Reference<Material> _material;
  Elements _elements;

  PolyMesh():
    _material{new Material{Color::white}}
  {
    // do nothing
  }

  void updateMeshTransform(TriCellMesh*) const;

}; // PolyMesh

} // end namespace cg::vis

#endif // __PolyMesh_h
