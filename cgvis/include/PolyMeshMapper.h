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
// OVERVIEW: PolyMeshMapper.h
// ========
// Class definition for vis poly mesh mapper.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __PolyMeshMapper_h
#define __PolyMeshMapper_h

#include "Mapper.h"
#include "PolyMesh.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyMeshMapper: vis poly mesh mapper class
// ==============
class PolyMeshMapper final: public Mapper<PolyMesh>
{
public:
  static Reference<PolyMeshMapper> New()
  {
    return new PolyMeshMapper;
  }

  const char* name() const override;

private:
  bool draw(GLRenderer&) const override;

  static void draw(GLRenderer&, const PolyMesh::Instance&);

}; // PolyMeshMapper

} // end namespace cg::vis

#endif // __PolyMeshMapper_h
