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
// OVERVIEW: Actor.h
// ========
// Class definition for actor.
//
// Author: Paulo Pagliosa
// Last revision: 19/01/2022

#ifndef __Actor_h
#define __Actor_h

#include "graphics/PrimitiveMapper.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Actor: actor class
// =====
class Actor: public NameableObject
{
public:
  bool visible{true};

  Actor(const PrimitiveMapper& mapper):
    _mapper{&mapper}
  {
    // do nothing
  }

  bool isVisible() const
  {
    return visible;
  }

  PrimitiveMapper* mapper() const
  {
    return _mapper;
  }

protected:
  Reference<PrimitiveMapper> _mapper;

}; // Actor

} // end namespace cg

#endif // __Actor_h
