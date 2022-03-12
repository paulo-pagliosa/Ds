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
// OVERVIEW: Mapper.cpp
// ========
// Source file for generic vis mapper.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#include "Mapper.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// AbstractMapper implementation
// ==============
void
AbstractMapper::setLookupTable(LookupTable* lookupTable)
{
  if (_lookupTable.get() != lookupTable)
  {
    _lookupTable = lookupTable;
    modified();
  }
}

Bounds3f
AbstractMapper::bounds() const
{
  return _bounds;
}

Primitive*
AbstractMapper::primitive() const
{
  return nullptr;
}

inline void
AbstractMapper::updateLookupTableRange(Scalars& scalars) const
{
  float min;
  float max;

  scalars.range(min, max);
  if (nullptr == _lookupTable)
    _lookupTable = LookupTable::jet();
  _lookupTable->setScalarRange(min, max);
}

bool
AbstractMapper::mapColors(Scalars* scalars) const
{
  if (scalars == nullptr)
    _colorMap.clear();
  else if (auto nv = scalars->size())
  {
    updateLookupTableRange(*scalars);
    _colorMap.resize(nv);
    for (decltype(nv) i = 0; i < nv; ++i)
      _colorMap[i] = _lookupTable->mapValue(scalars->get(i));
    return true;
  }
  return false;
}

} // end namespace cg::vis
