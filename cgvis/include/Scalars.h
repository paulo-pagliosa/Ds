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
// OVERVIEW: Scalars.h
// ========
// Class definition for vis scalars.
//
// Author: Paulo Pagliosa
// Last revision: 17/02/2022

#ifndef __Scalars_h
#define __Scalars_h

#include "Object.h"
#include <vector>

namespace cg
{ // begin namespace cg

namespace vis
{ // begin namespace vis


/////////////////////////////////////////////////////////////////////
//
// Scalars: vis scalars class
// =======
class Scalars: public Object
{
public:
  Scalars(uint32_t size = 0):
    _data(size)
  {
    modified();
  }

  auto size() const
  {
    return (uint32_t)_data.size();
  }

  auto scalar(int i) const
  {
    return _data[i];
  }

  void addScalar(float s)
  {
    _data.push_back(s);
    _computeTime.reset();
  }

  void setScalar(int i, float s);

  void range(float& min, float& max);

private:
  std::vector<float> _data;
  float _range[2]{};
  Timestamp _computeTime;

}; // Scalars

} // end namespace vis

} // end namespace cg

#endif // __Scalars_h
