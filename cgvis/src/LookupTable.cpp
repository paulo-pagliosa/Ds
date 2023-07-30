//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: LookupTable.cpp
// ========
// Source file for lookup table.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "LookupTable.h"
#include <cassert>

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// LookupTable implementation
// ===========
Reference<LookupTable>
LookupTable::jet(int size)
{
  assert(size > 0);
  return new LookupTable{size, float(2) / 3, 0};
}

Reference<LookupTable>
LookupTable::winter(int size)
{
  assert(size > 0);
  return new LookupTable{size, float(2) / 3, float(1) / 3};
}

Reference<LookupTable>
LookupTable::gray(const Color& scale, int size)
{
  assert(size > 0);

  auto l = new LookupTable{size};
  auto d = 1 / (float)math::max(size - 1, 1);

  for (int i = 0; i < size; ++i)
  {
    auto c = scale * (i * d);

    if (c.r > 1)
      c.r = 1;
    if (c.g > 1)
      c.g = 1;
    if (c.b > 1)
      c.b = 1;
    l->_colors[i] = c;
  }
  return l;
}

Reference<LookupTable>
LookupTable::cooper(int size)
{
  return gray(Color{1.2500f, 0.7812f, 0.4975f}, size);
}

LookupTable::LookupTable(int size):
  _colors(size)
{
  _scalarRange[0] = 0;
  _scalarRange[1] = 1;
  _hueRange[0] = 0;
  _hueRange[1] = 0;
  _saturationRange[0] = 1;
  _saturationRange[1] = 1;
  _valueRange[0] = 1;
  _valueRange[1] = 1;
}

LookupTable::LookupTable(int size, float minHue, float maxHue):
  _colors(size)
{
  _scalarRange[0] = 0;
  _scalarRange[1] = 1;
  _hueRange[0] = minHue;
  _hueRange[1] = maxHue;
  _saturationRange[0] = 1;
  _saturationRange[1] = 1;
  _valueRange[0] = 1;
  _valueRange[1] = 1;
  modified();
  build();
}

void
LookupTable::setScalarRange(float min, float max)
{
  if (min == max)
  {
    min -= 0.5;
    max += 0.5;
  }
  else if (min > max)
    math::swap(min, max);
  _scalarRange[0] = min;
  _scalarRange[1] = max;
}

#define incof(x, nc)	((x[1] - x[0]) / (nc - 1))

void
LookupTable::build()
{
  if (_buildTime > modifiedTime())
    return;

  auto nc = size();
  auto hinc = incof(_hueRange, nc);
  auto sinc = incof(_saturationRange, nc);
  auto vinc = incof(_valueRange, nc);

  for (int i = 0; i < nc; i++)
  {
    float rgb[3];
    auto hue = i * hinc + _hueRange[0];
    auto sat = i * sinc + _saturationRange[0];
    auto val = i * vinc + _valueRange[0];
    auto hueCase = (int)(hue * 6);
    auto frac = 6 * hue - hueCase;
    auto lx = val * (1 - sat);
    auto ly = val * (1 - sat * frac);
    auto lz = val * (1 - sat * (1 - frac));

    switch (hueCase)
    {
      // 0 < hue < 1/6
      case 0:
      case 6:
        rgb[0] = val;
        rgb[1] = lz;
        rgb[2] = lx;
        break;

        // 1/6 < hue < 2/6
      case 1:
        rgb[0] = ly;
        rgb[1] = val;
        rgb[2] = lx;
        break;

        // 2/6 < hue < 3/6
      case 2:
        rgb[0] = lx;
        rgb[1] = val;
        rgb[2] = lz;
        break;

        // 3/6 < hue < 4/6
      case 3:
        rgb[0] = lx;
        rgb[1] = ly;
        rgb[2] = val;
        break;

        // 4/6 < hue < 5/6
      case 4:
        rgb[0] = lz;
        rgb[1] = lx;
        rgb[2] = val;
        break;

        // 5/6 < hue < 1
      case 5:
        rgb[0] = val;
        rgb[1] = lx;
        rgb[2] = ly;
        break;
    }
    _colors[i].r = (1 + cos((1 - rgb[0]) * math::pi<float>)) * 0.5f;
    _colors[i].g = (1 + cos((1 - rgb[1]) * math::pi<float>)) * 0.5f;
    _colors[i].b = (1 + cos((1 - rgb[2]) * math::pi<float>)) * 0.5f;
  }
  _buildTime.modified();
}

const Color&
LookupTable::mapValue(float v) const
{
  auto n = size();
  auto s = _scalarRange[0];
  auto i = (int)((v - s) / (_scalarRange[1] - s) * n);

  i = (i < 0 ? 0 : (i >= n ? n - 1 : i));
  return _colors[i];
}

} // namespace cg::vis
