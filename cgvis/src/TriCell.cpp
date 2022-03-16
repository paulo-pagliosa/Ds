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
// OVERVIEW: TriCell.cpp
// ========
// Source file for vis linear triangular cell.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#include "PolyData.h"
#include "Scalars.h"
#include "TriCell.h"

namespace cg::vis
{ // begin namespace cg::vis

//
// Marching triangles
//
void contourTriangle(const vec3f* vertices,
  const float* scalars,
  float value,
  PolyData& output)
{
  struct LineCases
  {
    int edges[3];
  };

  static LineCases lineCases[]
  {
    {-1, -1, -1},
    { 0,  2, -1},
    { 1,  0, -1},
    { 1,  2, -1},
    { 2,  1, -1},
    { 0,  1, -1},
    { 2,  0, -1},
    {-1, -1, -1}
  };
  static int caseMask[3]{1, 2, 4};
  static int edges[3][2]{{0, 1}, {1, 2}, {2, 0}};

  int index{};

  // Build the case table
  for (int i = 0; i < 3; i++)
    if (scalars[i] >= value)
      index |= caseMask[i];

  auto lineCase = lineCases + index;
  auto edge = lineCase->edges;

  for (; edge[0] > -1; edge += 2)
  {
    vec3f p[2];

    for (int i = 0; i < 2; i++) // insert line
    {
      auto k = edges[edge[i]];
      auto v = vertices[k[1]] - vertices[k[0]];
      auto t = (value - scalars[k[0]]) / (scalars[k[1]] - scalars[k[0]]);

      p[i] = vertices[k[0]] + t * v;
      // TODO: interpolate vectors
    }
    if (const auto* line = output.addLine(p[0], p[1]))
    {
      output.vertexScalars()->set(line->ids[0], value);
      output.vertexScalars()->set(line->ids[1], value);
    }
  }
}

} // end namespace cg::vis
