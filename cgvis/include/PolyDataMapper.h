//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Orthrus Group.                         |
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
// OVERVIEW: PolyDataMapper.h
// ========
// Class definition for vis poly data mapper.
//
// Author: Paulo Pagliosa
// Last revision: 01/03/2022

#ifndef __PolyDataMapper_h
#define __PolyDataMapper_h

#include "Mapper.h"
#include "PolyData.h"

namespace cg
{ // begin namespace cg

namespace vis
{ // begin namespace vis


/////////////////////////////////////////////////////////////////////
//
// PolyDataMapper: vis poly data mapper class
// ==============
class PolyDataMapper final: public Mapper<PolyData>
{
public:
  static Reference<PolyDataMapper> New()
  {
    return new PolyDataMapper;
  }

  const char* name() const override;

private:
  bool draw(GLRenderer&) const override;

}; // PolyDataMapper

} // end namespace vis

} // end namespace cg

#endif // __PolyDataMapper_h
