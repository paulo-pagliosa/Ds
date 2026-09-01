//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLGraphicsBase.h
// ========
// Class definition for OpenGL graphics base.
//
// Author: Paulo Pagliosa
// Last revision: 31/08/2026

#ifndef __GLGraphicsBase_h
#define __GLGraphicsBase_h

#include "Color.h"
#include "GLProgram.h"
#include <algorithm>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLGraphicsBase: OpenGL graphics base class
// ==============
class GLGraphicsBase
{
public:
  enum PolygonMode
  {
    LINE = GL_LINE,
    FILL = GL_FILL
  };

  /// Destructs this GL graphics base
  ~GLGraphicsBase();

  /// Returns the point color.
  [[nodiscard]] const auto& pointColor() const
  {
    return _pointColor;
  }

  /// Sets the point color.
  void setPointColor(const Color& color)
  {
    _pointColor = color;
  }

  /// Returns the point size.
  [[nodiscard]] auto pointSize() const
  {
    return _pointSize;
  }

  /// Sets the point size.
  void setPointSize(float size)
  {
    _pointSize = size;
  }

  /// Returns the line vertex colors.
  [[nodiscard]] const auto& lineColor(size_t index) const
  {
    assert(index < 2);
    return _lineColors[index];
  }

  void lineColor(Color (&colors)[2]) const
  {
    std::copy(_lineColors, _lineColors + 2, colors);
  }

  /// Sets the line color.
  void setLineColor(const Color& color)
  {
    std::fill_n(_lineColors, 2, color);
  }

  /// Sets the line vertex colors.
  void setLineColors(const Color (&colors)[2])
  {
    std::copy(colors, colors + 2, _lineColors);
  }

  /// Returns the line width.
  [[nodiscard]] float lineWidth() const
  {
    return _lineWidth;
  }

  /// Sets the line width.
  void setLineWidth(float width)
  {
    _lineWidth = width;
  }

  /// Returns the triangle vertex colors.
  [[nodiscard]] const auto& triangleColor(size_t index) const
  {
    assert(index < 3);
    return _triangleColors[index];
  }

  void triangleColors(Color (&colors)[3]) const
  {
    std::copy(_triangleColors, _triangleColors + 3, colors);
  }

  /// Sets the triangle color.
  void setTriangleColor(const Color& color)
  {
    std::fill_n(_triangleColors, 2, color);
  }

  /// Sets the triangle vertex colors.
  void setTriangleColors(const Color (&colors)[3])
  {
    std::copy(colors, colors + 3, _triangleColors);
  }

  /// Returns the quad vertex colors.
  [[nodiscard]] const auto& quadColor(size_t index) const
  {
    assert(index < 4);
    return _quadColors[index];
  }

  void quadColors(Color (&colors)[4]) const
  {
    std::copy(_quadColors, _quadColors + 4, colors);
  }

  /// Sets the quad color.
  void setQuadColor(const Color& color)
  {
    std::fill_n(_quadColors, 4, color);
  }

  /// Sets the quad vertex colors.
  void setQuadColors(const Color (&colors)[4])
  {
    std::copy(colors, colors + 4, _quadColors);
  }

  /// Returns the polygon mode.
  [[nodiscard]] auto polygonMode() const
  {
    return _polygonMode;
  }

  /// Sets the polygon mode.
  void setPolygonMode(PolygonMode mode)
  {
    glPolygonMode(GL_FRONT_AND_BACK, _polygonMode = mode);
  }

protected:
  /// Constructs a GL graphics base.
  GLGraphicsBase();

  /// Draws a point in NDC.
  void drawPoint(const vec4f*);

  /// Draws a line in NDC.
  void drawLine(const vec4f*);

  /// Draws a triangle in NDC.
  void drawTriangle(const vec4f*);

  /// Draws a quad in NDC.
  void drawQuad(const vec4f*);

private:
  GLuint _vao;
  GLSL::Program _pointDrawer;
  GLint _pointLoc;
  GLint _pointColorLoc;
  Color _pointColor;
  float _pointSize;
  GLSL::Program _lineDrawer;
  GLint _linePointsLoc;
  GLint _lineColorsLoc;
  Color _lineColors[2];
  float _lineWidth;
  GLSL::Program _triangleDrawer;
  GLint _trianglePointsLoc;
  GLint _triangleColorsLoc;
  Color _triangleColors[3];
  GLSL::Program _quadDrawer;
  GLint _quadPointsLoc;
  GLint _quadColorsLoc;
  Color _quadColors[4];
  PolygonMode _polygonMode;

}; // GLGraphicsBase

} // end namespace cg

#endif // __GLGraphicsBase_h
