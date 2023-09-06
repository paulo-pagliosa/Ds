//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023 Paulo Pagliosa.                              |
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
// OVERVIEW: GLPoints3Renderer.cpp
// ========
// Source file for OpenGL 3D points renderer.
//
// Author: Paulo Pagliosa
// Last revision: 09/08/2023

#include "graphics/GLPoints3Renderer.h"
#include <cassert>

namespace cg
{ // begin namespace cg

#define STRINGIFY(A) "#version 400\n"#A

static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec4 color;
  uniform mat4 mvpMatrix;
  uniform int usePointColors;
  uniform vec4 pointColor;
  out vec4 vertexColor;

void main()
{
  gl_Position = mvpMatrix * position;
  vertexColor = usePointColors != 0 ? color : pointColor;
}
);

static const char* fragmentShader = STRINGIFY(
  in vec4 vertexColor;
  out vec4 fragmentColor;

  void main()
  {
    fragmentColor = vertexColor;
  }
);


/////////////////////////////////////////////////////////////////////
//
// GLPoints3Renderer implementation
// ================
inline void
GLPoints3Renderer::GLProgram::initUniformLocations()
{
  mvpMatrixLoc = uniformLocation("mvpMatrix");
  usePointColorsLoc = uniformLocation("usePointColors");
  pointColorLoc = uniformLocation("pointColor");
}

inline void
GLPoints3Renderer::GLProgram::initProgram()
{
  setShaders(vertexShader, fragmentShader).use();
  initUniformLocations();
}

GLPoints3Renderer::GLProgram::GLProgram():
  GLSL::Program{"Point Renderer"}
{
  auto cp = GLSL::Program::current();

  initProgram();
  setUniformVec4(pointColorLoc, Color::black);
  GLSL::Program::setCurrent(cp);
}

inline void
GLPoints3Renderer::updateView()
{
  camera()->update();
}

void
GLPoints3Renderer::begin()
{
  if (auto cp = GLSL::Program::current(); &_program != cp)
  {
    _lastState.program = cp;
    glGetFloatv(GL_POINT_SIZE, &_lastState.pointSize);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
    glPointSize(_pointSize);
    _program.use();
    _program.setUniform(_program.usePointColorsLoc, int(usePointColors));
    updateView();
  }
}

void
GLPoints3Renderer::end()
{
  if (auto cp = GLSL::Program::current(); &_program == cp)
  {
    GLSL::Program::setCurrent(_lastState.program);
    glPointSize(_lastState.pointSize);
    glBindVertexArray(_lastState.vao);
  }
}

namespace
{ // begin namespace

inline mat4f
mvpMatrix(const mat4f& t, const Camera& c)
{
  return c.projectionMatrix() * c.worldToCameraMatrix() * t;
}

} // end namespace

void
GLPoints3Renderer::drawPoints(const GLPoints3& points,
  const mat4f& t,
  int count,
  int offset)
{
  _program.setUniformMat4(_program.mvpMatrixLoc, mvpMatrix(t, *camera()));
  points.bind();
  glDrawArrays(GL_POINTS, offset, count);
}

void
GLPoints3Renderer::render(const GLPoints3& points,
  const vec3f& p,
  const mat3f& r,
  const vec3f& s)
{
  render(points, TRS(p, r, s));
}

void
GLPoints3Renderer::render(const GLPoints3& points, int index, const mat4f& t)
{
  assert(index >= 0 && index < (int)points.size());
  drawPoints(points, t, 1, index);
}

} // end namespace cg
