//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2019 Orthrus Group.                         |
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
// OVERVIEW: GLGraphicsBase.cpp
// ========
// Source file for OpenGL graphics base.
//
// Author: Paulo Pagliosa
// Last revision: 16/02/2019

#include "graphics/GLGraphicsBase.h"

namespace cg
{ // begin namespace cg

static const char* pointVertexShader =
  "#version 400\n"
  "uniform vec4 p;\n"
  "uniform vec4 pointColor;\n"
  "out vec4 color;\n"
  "void main() {\n"
  "gl_Position = p;\n"
  "color = pointColor;\n"
  "}";

static const char* lineVertexShader =
  "#version 400\n"
  "uniform vec4 p[2];\n"
  "uniform vec4 vertexColors[2];\n"
  "out vec4 color;\n"
  "void main() {\n"
  "gl_Position = p[gl_VertexID];\n"
  "color = vertexColors[gl_VertexID];\n"
  "}";

static const char* triangleVertexShader =
  "#version 400\n"
  "uniform vec4 p[3];\n"
  "uniform vec4 vertexColors[3];\n"
  "out vec4 color;\n"
  "void main() {\n"
  "gl_Position = p[gl_VertexID];\n"
  "color = vertexColors[gl_VertexID];\n"
  "}";

static const char* fragmentShader =
  "#version 400\n"
  "in vec4 color;\n"
  "out vec4 fragmentColor;\n"
  "void main() {\n"
  "fragmentColor = color;\n"
  "}";


/////////////////////////////////////////////////////////////////////
//
// GLGraphicsBase implementation
// ==============
GLGraphicsBase::GLGraphicsBase():
  _pointDrawer{"Point Drawer"},
  _lineDrawer{"Line Drawer"},
  _triangleDrawer{"Triangle Drawer"},
  _polygonMode{FILL}
{
  using namespace GLSL;

  _pointDrawer.setShaders(pointVertexShader, fragmentShader);
  _pointDrawer.use();
  _pointLoc = _pointDrawer.uniformLocation("p");
  _pointColorLoc = _pointDrawer.uniformLocation("pointColor");
  _lineDrawer.setShaders(lineVertexShader, fragmentShader);
  _lineDrawer.use();
  _linePointsLoc = _lineDrawer.uniformLocation("p[0]");
  _lineColorsLoc = _lineDrawer.uniformLocation("vertexColors[0]");
  _triangleDrawer.setShaders(triangleVertexShader, fragmentShader);
  _triangleDrawer.use();
  _trianglePointsLoc = _triangleDrawer.uniformLocation("p[0]");
  _triangleColorsLoc = _triangleDrawer.uniformLocation("vertexColors[0]");
  glGenVertexArrays(1, &_vao);
  setPointSize(4);
  setPointColor(Color::red);
  setLineWidth(1);
  setLineColor(Color::white);
  setTriangleColor(Color::white);
}

GLGraphicsBase::~GLGraphicsBase()
{
  glDeleteVertexArrays(1, &_vao);
}

void
GLGraphicsBase::drawPoint(const vec4f* point)
{
  using namespace GLSL;

  auto cp = Program::current();

  _pointDrawer.use();
  _pointDrawer.setUniformVec4(_pointColorLoc, _pointColor);
  glUniform4fv(_pointLoc, 1, (float*)point);
  glBindVertexArray(_vao);
  glPointSize(_pointSize);
  glDrawArrays(GL_POINTS, 0, 1);
  Program::setCurrent(cp);
}

void
GLGraphicsBase::drawLine(const vec4f* points)
{
  using namespace GLSL;

  auto cp = Program::current();

  _lineDrawer.use();
  glUniform4fv(_lineColorsLoc, 2, (float*)_lineColors);
  glUniform4fv(_linePointsLoc, 2, (float*)points);
  glBindVertexArray(_vao);
  glLineWidth(_lineWidth);
  glDrawArrays(GL_LINES, 0, 2);
  Program::setCurrent(cp);
}

void
GLGraphicsBase::drawTriangle(const vec4f* points)
{
  using namespace GLSL;

  auto cp = Program::current();

  _triangleDrawer.use();
  glBindVertexArray(_vao);
  glUniform4fv(_triangleColorsLoc, 3, (float*)_triangleColors);
  glUniform4fv(_trianglePointsLoc, 3, (float*)points);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  Program::setCurrent(cp);
}

} // end namespace cg
