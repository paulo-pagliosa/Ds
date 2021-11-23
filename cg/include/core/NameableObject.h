//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2018 Orthrus Group.                         |
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
// OVERVIEW: NameableObject.h
// ========
// Class definition for nameable object.
//
// Author: Paulo Pagliosa
// Last revision: 21/09/2018

#ifndef __NameableObject_h
#define __NameableObject_h

#include "core/SharedObject.h"
#include <string>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// NameableObject: nameable object class
// ==============
class NameableObject: public SharedObject
{
public:
  /// Returns the name of this object.
  auto name() const
  {
    return _name.c_str();
  }

  /// Sets the name of this object.
  void setName(const char* format, ...);

protected:
  NameableObject() = default;

  NameableObject(const char* name):
    _name{name}
  {
    // do nothing
  }

private:
  std::string _name;

}; // NameableObject

} // end namespace cg

#endif // __NameableObject_h
