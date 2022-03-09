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
// OVERVIEW: PolyDataMapper.cpp
// ========
// Source file for vis poly data mapper.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#include "PolyDataMapper.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyDataMapper implementation
// ==============
const char*
PolyDataMapper::name() const
{
  return "PolyData Mapper";
}

bool
PolyDataMapper::draw(GLRenderer& renderer) const
{
  const auto& colorMap = this->colorMap();
  auto useColorMap = !colorMap.empty() && useVertexColors;
  auto input = this->input();

  for (const auto& triangle : input->_triangles)
  {
    if (!useColorMap)
      renderer.setTriangleColor(triangle.color);
    else
    {
      Color c[3];

      c[0] = colorMap[triangle.ids[0]];
      c[1] = colorMap[triangle.ids[1]];
      c[2] = colorMap[triangle.ids[2]];
      renderer.setTriangleColors(c);
    }

    const auto& p1 = input->vertex(triangle.ids[0]);
    const auto& p2 = input->vertex(triangle.ids[1]);
    const auto& p3 = input->vertex(triangle.ids[2]);

    renderer.drawTriangle(p1, p2, p3);
  }
  for (const auto& line : input->_lines)
  {
    if (!useColorMap)
      renderer.setLineColor(line.color);
    else
    {
      Color c[2];

      c[0] = colorMap[line.ids[0]];
      c[1] = colorMap[line.ids[1]];
      renderer.setLineColors(c);
    }

    const auto& p1 = input->vertex(line.ids[0]);
    const auto& p2 = input->vertex(line.ids[1]);

    renderer.drawLine(p1, p2);
  }
  for (const auto& point : input->_points)
  {
    if (!useColorMap)
      renderer.setPointColor(point.color);
    else
      renderer.setPointColor(colorMap[point.id]);

    const auto& p = input->vertex(point.id);

    renderer.setPointSize(point.size);
    renderer.drawPoint(p);
  }
  return true;
}

} // end namespace cg::vis
