//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: NamedObject.h
// ========
// Class definition for named object.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __NamedObject_h
#define __NamedObject_h

#include "core/SharedObject.h"
#include <string>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// NamedObject: named object class
// ===========
class NamedObject
{
public:
  /// Destructor.
  virtual ~NamedObject() = default;

  /// Returns the name of this object.
  auto name() const
  {
    return _name.c_str();
  }

  /// Sets the name of this object.
  void setName(const char* format, ...);

protected:
  NamedObject() = default;

  NamedObject(const char* name):
    _name{name}
  {
    // do nothing
  }

private:
  std::string _name;

}; // NamedObject


/////////////////////////////////////////////////////////////////////
//
// SharedNamedObject: shared named object class
// =================
class SharedNamedObject: public NamedObject, public SharedObject
{
protected:
  SharedNamedObject() = default;

  SharedNamedObject(const char* name):
    NamedObject{name}
  {
    // do nothing
  }

}; // SharedNamedObject

} // end namespace cg

#endif // __NamedObject_h
