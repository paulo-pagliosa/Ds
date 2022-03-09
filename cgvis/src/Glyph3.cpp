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
// OVERVIEW: Glyph3.cpp
// ========
// Source file for vis 3D glyph filter.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#include "Glyph3.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Glyph3Base implementation
// ==========
Glyph3Base::Glyph3Base():
  _scaleMode{ScaleMode::Scalar},
  _scaleFactor{1},
  _clamping{false}
{
  _range[0] = 0;
  _range[1] = 1;
}

void
Glyph3Base::setRange(float min, float max)
{
  if (min > max)
    math::swap(min, max);
  _range[0] = min;
  _range[1] = max;
  modified();
}

PolyData*
Glyph3Base::makeDefaultSource() const
{
  // TODO
  return nullptr;
}

void
Glyph3Base::execute(const Points& points, PolyData& output)
{
  if (_source == nullptr)
    _source = makeDefaultSource();
  // TODO
}

} // end namespace cg::vis
