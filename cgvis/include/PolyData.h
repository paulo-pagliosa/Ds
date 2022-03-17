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
// OVERVIEW: PolyData.h
// ========
// Class definition for vis poly data.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#ifndef __PolyData_h
#define __PolyData_h

#include "DataSet.h"
#include "PointLocator.h"
#include "graphics/Color.h"

namespace cg::vis
{ // begin namespace cg::vis

//
// Forward definition
//
class PolyDataMapper;


/////////////////////////////////////////////////////////////////////
//
// PolyData: vis poly data class
// ========
class PolyData: public DataSet
{
public:
  static constexpr auto dflGridDivisions = 20;

  struct PointData
  {
    Color color;
    float size;
    const int id;

  }; // PointData

  struct LineData
  {
    Color color;
    const int ids[2];

  }; // LineData

  struct TriangleData
  {
    Color color;
    const int ids[3];

  }; // TriangleData

  Color pointColor{Color::red};
  Color lineColor{0.4f, 0.4f, 0.4f};
  Color triangleColor{Color::white};

  static Reference<PolyData> New()
  {
    return new PolyData;
  }

  auto& bounds() const
  {
    return _bounds;
  }

  auto pointSize() const
  {
    return _pointSize;
  }

  void setPointSize(float size)
  {
    _pointSize = size <= 0 ? 1 : size;
  }

  const auto* addPoint(const vec3f& p)
  {
    auto vid = addVertex(p);

    _points.push_back({pointColor, _pointSize, vid});
    return &_points.back();
  }

  const auto* addLine(const vec3f& p1, const vec3f& p2)
  {
    auto vid1 = addVertex(p1);
    auto vid2 = addVertex(p2);

    if (vid1 == vid2)
      return (LineData*)nullptr;
    _lines.push_back({lineColor, vid1, vid2});
    return &_lines.back();
  }

  const auto* addTriangle(const vec3f& p1, const vec3f& p2, const vec3f& p3)
  {
    auto vid1 = addVertex(p1);
    auto vid2 = addVertex(p2);
    auto vid3 = addVertex(p3);

    if (vid1 == vid2 || vid2 == vid3 || vid1 == vid2)
      return (TriangleData*)nullptr;
    _triangles.push_back({triangleColor, vid1, vid2, vid3});
    return &_triangles.back();
  }

  void deleteAll();

  auto vertexCount() const
  {
    return (uint32_t)_vertices.size();
  }

  const auto& vertex(int i) const
  {
    return _vertices[i];
  }

  void initializeLocator(const Bounds3f& bounds, int n = dflGridDivisions)
  {
    _locator = new Locator{bounds, n, _vertices};
  }

  void deleteLocator()
  {
    _locator = nullptr;
  }

private:
  using Locator = PointLocator<vec3f>;

  std::vector<PointData> _points;
  std::vector<LineData> _lines;
  std::vector<TriangleData> _triangles;
  std::vector<vec3f> _vertices;
  float _pointSize{4};
  Bounds3f _bounds;
  Reference<Locator> _locator;

  PolyData() = default;

  int addVertex(const vec3f& p);

  friend PolyDataMapper;

}; // PolyData

} // end namespace cg::vis

#endif // __PolyData_h
