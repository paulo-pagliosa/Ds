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
// OVERVIEW: TriangleMeshMapper.h
// ========
// Class definition for triangle mesh mapper.
//
// Author: Paulo Pagliosa
// Last revision: 16/07/2022

#ifndef __TriangleMeshMapper_h
#define __TriangleMeshMapper_h

#include "graphics/PrimitiveMapper.h"
#include "graphics/TriangleMeshShape.h"

namespace cg
{ // begin namespace cg

class TriangleMesh;


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshMapper: triangle mesh mapper class
// ==================
class TriangleMeshMapper: public PrimitiveMapper
{
public:
  TriangleMeshMapper(const TriangleMesh& mesh);

  bool render(GLRenderer&) const override;
  Bounds3f bounds() const override;
  Primitive* primitive() const override;

  const TriangleMeshShape* shape() const
  {
    return _shape;
  }

  const TriangleMesh* mesh() const
  {
    return _shape->mesh();
  }

  void setMesh(const TriangleMesh& mesh);

private:
  Reference<TriangleMeshShape> _shape;
  Reference<Primitive> _primitive;

}; // TriangleMeshMapper

} // end namespace cg

#endif // __TriangleMeshMapper_h
