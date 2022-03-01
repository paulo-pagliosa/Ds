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
// OVERVIEW: DataSet.h
// ========
// Class definition for generic vis data set.
//
// Author: Paulo Pagliosa
// Last revision: 17/02/2022

#ifndef __DataSet_h
#define __DataSet_h

#include "Scalars.h"

namespace cg
{ // begin namespace cg

namespace vis
{ // begin namespace vis


/////////////////////////////////////////////////////////////////////
//
// DataSet: generic vis data set class
// =======
class DataSet: public Object
{
public:
  Scalars* scalars() const
  {
    return _scalars;
  }

  void setScalars(const Scalars* scalars)
  {
    _scalars = scalars;
    modified();
  }

protected:
  Reference<Scalars> _scalars;

}; // DataSet

} // end namespace vis

} // end namespace cg

#endif // __DataSet_h
