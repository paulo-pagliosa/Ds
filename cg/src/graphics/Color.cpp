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
// OVERVIEW: Color.cpp
// ========
// Source file for RGB color.
//
// Author: Paulo Pagliosa
// Last revision: 01/08/2018

#include "graphics/Color.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Color implementation
// =====
Color Color::black(0, 0, 0);
Color Color::red(255, 0, 0);
Color Color::green(0, 255, 0);
Color Color::blue(0, 0, 255);
Color Color::cyan(0, 255, 255);
Color Color::magenta(255, 0, 255);
Color Color::yellow(255, 255, 0);
Color Color::white(255, 255, 255);
Color Color::darkGray(136, 136, 136);
Color Color::gray(194, 194, 194);

Color
Color::HSV2RGB(float hue, float saturation, float value)
{
  hue = hue - 360 * int(hue * math::inverse<float>(360));

  Color color;
  auto f = int(hue * math::inverse<float>(120));
  auto d = (hue - 120 * f) * math::inverse<float>(60);

  if (f == 3)
    --f;
  color[f] = value;
  if (d > 1)
  {
    color[(f + 1) % 3] = value * (1 - saturation * (2 - d));
    color[(f + 2) % 3] = value * (1 - saturation);
  }
  else
  {
    color[(f + 1) % 3] = value * (1 - saturation);
    color[(f + 2) % 3] = value * (1 - saturation * d);
  }
  return color;
}

} // end namespace cg
