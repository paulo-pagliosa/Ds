//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// Last revision: 22/01/2022

#ifndef __GLGraphicsBase_h
#define __GLGraphicsBase_h

#include "Color.h"
#include "GLProgram.h"

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
  const Color& pointColor() const
  {
    return _pointColor;
  }

  /// Sets the point color.
  void setPointColor(const Color& color)
  {
    _pointColor = color;
  }

  /// Returns the point size.
  float pointSize() const
  {
    return _pointSize;
  }

  /// Sets the point size.
  void setPointSize(float size)
  {
    _pointSize = size;
  }

  /// Returns the vertex line colors.
  void lineColor(Color colors[2]) const
  {
    colors[0] = _lineColors[0];
    colors[1] = _lineColors[1];
  }

  /// Sets the line color.
  void setLineColor(const Color& color)
  {
    _lineColors[0] = color;
    _lineColors[1] = color;
  }

  /// Sets the vertex line colors.
  void setLineColors(const Color colors[2])
  {
    _lineColors[0] = colors[0];
    _lineColors[1] = colors[1];
  }

  /// Returns the line width.
  float lineWidth() const
  {
    return _lineWidth;
  }

  /// Sets the line width.
  void setLineWidth(float width)
  {
    _lineWidth = width;
  }

  /// Returns the vertex triangle colors.
  void triangleColors(Color colors[3]) const
  {
    colors[0] = _polygonColors[0];
    colors[1] = _polygonColors[1];
    colors[2] = _polygonColors[2];
  }

  /// Sets the triangle color.
  void setTriangleColor(const Color& color)
  {
    _polygonColors[0] = color;
    _polygonColors[1] = color;
    _polygonColors[2] = color;
  }

  /// Sets the vertex triangle colors.
  void setTriangleColors(const Color colors[3])
  {
    _polygonColors[0] = colors[0];
    _polygonColors[1] = colors[1];
    _polygonColors[2] = colors[2];
  }

  auto polygonMode() const
  {
    return _polygonMode;
  }

  void setPolygonMode(PolygonMode mode)
  {
    glPolygonMode(GL_FRONT_AND_BACK, _polygonMode = mode);
  }

  /// Returns the vertex quad colors.
  void quadColors(Color colors[4]) const
  {
    triangleColors(colors);
    colors[3] = _polygonColors[3];
  }

  /// Sets the quad color.
  void setQuadColor(const Color& color)
  {
    setTriangleColor(_polygonColors[3] = color);
  }

  /// Sets the vertex quad colors.
  void setQuadColors(const Color colors[4])
  {
    setTriangleColors(colors);
    _polygonColors[3] = colors[3];
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
  GLSL::Program _quadDrawer;
  GLint _quadPointsLoc;
  GLint _quadColorsLoc;
  Color _polygonColors[4];
  PolygonMode _polygonMode;

}; // GLGraphicsBase

} // end namespace cg

#endif // __GLGraphicsBase_h
