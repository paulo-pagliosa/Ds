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
// OVERVIEW: Scalars.cpp
// ========
// Source file for vis scalars.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#include "math/RealLimits.h"
#include "Scalars.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Scalars implementation
// =======
void
Scalars::range(float& min, float& max)
{
  if (modifiedTime() > _computeTime)
  {
    _range[0] = +math::Limits<float>::inf();
    _range[1] = -math::Limits<float>::inf();
    for (auto s : _data)
    {
      if (s < _range[0])
        _range[0] = s;
      if (s > _range[1])
        _range[1] = s;
    }
    _computeTime.modified();
  }
  min = _range[0];
  max = _range[1];
}

} // end namespace cg::vis
