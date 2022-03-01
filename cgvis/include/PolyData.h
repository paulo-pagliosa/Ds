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
// Last revision: 28/02/2022

#ifndef __PolyData_h
#define __PolyData_h

#include "DataSet.h"
#include "PointLocator.h"
#include "graphics/Color.h"

namespace cg
{ // begin namespace cg

namespace vis
{ // begin namespace vis

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
    int id;

  }; // PointData

  struct LineData
  {
    Color color;
    int ids[2];

  }; // LineData

  struct TriangleData
  {
    Color color;
    int ids[3];

  }; // TriangleData

  auto& bounds() const
  {
    return _bounds;
  }

  void setPointColor(const Color& color)
  {
    _pointColor = color;
  }

  void setPointSize(float size)
  {
    _pointSize = size;
  }

  const auto* addPoint(const vec3f& p)
  {
    auto vid = addVertex(p);

    _points.push_back({_pointColor, _pointSize, vid});
    return &_points.back();
  }

  void setLineColor(const Color& color)
  {
    _lineColor = color;
  }

  const auto* addLine(const vec3f& p1, const vec3f& p2)
  {
    auto vid1 = addVertex(p1);
    auto vid2 = addVertex(p2);

    if (vid1 == vid2)
      return (LineData*)nullptr;
    _lines.push_back({_lineColor, vid1, vid2});
    return &_lines.back();
  }

  void setTriangleColor(const Color& color)
  {
    _triangleColor = color;
  }

  const auto& addTriangle(const vec3f& p1, const vec3f& p2, const vec3f& p3)
  {
    auto vid1 = addVertex(p1);
    auto vid2 = addVertex(p2);
    auto vid3 = addVertex(p3);

    if (vid1 == vid2 || vid2 == vid3 || vid1 == vid2)
      return (TriangleData*)nullptr;
    _triangles.push_back({_triangleColor, vid1, vid2, vid3});
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
  Color _pointColor{Color::red};
  Color _lineColor{0.4f, 0.4f, 0.4f};
  Color _triangleColor{Color::white};
  Bounds3f _bounds;
  Reference<Locator> _locator;

  int addVertex(const vec3f& p);

  friend PolyDataMapper;

}; // PolyData

} // end namespace vis

} // end namespace cg

#endif // __PolyData_h
