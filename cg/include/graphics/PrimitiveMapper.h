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
// OVERVIEW: PrimitiveMapper.h
// ========
// Class definition for generic primitive mapper.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2022

#ifndef __PrimitiveMapper_h
#define __PrimitiveMapper_h

#include "graphics/Primitive.h"

namespace cg
{ // begin namespace cg

class GLRenderer;


/////////////////////////////////////////////////////////////////////
//
// PrimitiveMapper: generic primitive mapper class
// ===============
class PrimitiveMapper: public virtual SharedObject
{
public:
  virtual void update();
  virtual bool render(GLRenderer&) const;

  virtual Bounds3f bounds() const;
  virtual Primitive* primitive() const;

}; // PrimitiveMapper

} // end namespace cg

#endif // __PrimitiveMapper_h
