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
// OVERVIEW: GLLines3Renderer.cpp
// ========
// Source file for OpenGL 3D lines renderer.
//
// Author: Paulo Pagliosa
// Last revision: 28/08/2023

#include "graphics/GLLines3Renderer.h"


namespace cg
{ // begin namespace cg

#define STRINGIFY(A) "#version 400\n"#A

static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec4 color;
  uniform mat4 mvpMatrix;
  uniform int useVertexColors;
  uniform vec4 lineColor;
  out vec4 vertexColor;

  void main()
  {
    gl_Position = mvpMatrix * position;
    vertexColor = useVertexColors != 0 ? color : lineColor;
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
// GLLines3Renderer implementation
// ================
inline void
GLLines3Renderer::GLProgram::initUniformLocations()
{
  mvpMatrixLoc = uniformLocation("mvpMatrix");
  useVertexColorsLoc = uniformLocation("useVertexColors");
  lineColorLoc = uniformLocation("lineColor");
}

inline void
GLLines3Renderer::GLProgram::initProgram()
{
  setShaders(vertexShader, fragmentShader).use();
  initUniformLocations();
}

GLLines3Renderer::GLProgram::GLProgram():
  GLSL::Program{"Line Renderer"}
{
  auto cp = GLSL::Program::current();

  initProgram();
  setUniformVec4(lineColorLoc, Color::black);
  GLSL::Program::setCurrent(cp);
}

GLLines3Renderer::GLLines3Renderer(Camera* camera):
  _camera{!camera ? new Camera{} : camera}
{
  // do nothing
}

inline void
GLLines3Renderer::updateView()
{
  _camera->update();
}

void
GLLines3Renderer::setCamera(Camera* camera)
{
  if (camera != _camera.get())
    (_camera = camera ? camera : new Camera{})->update();
}

void
GLLines3Renderer::begin()
{
  if (auto cp = GLSL::Program::current(); &_program != cp)
  {
    _lastState.program = cp;
    glGetFloatv(GL_LINE_WIDTH, &_lastState.lineWidth);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
    glLineWidth(_lineWidth);
    _program.use();
    updateView();
  }
}

void
GLLines3Renderer::end()
{
  if (auto cp = GLSL::Program::current(); &_program == cp)
  {
    GLSL::Program::setCurrent(_lastState.program);
    glLineWidth(_lastState.lineWidth);
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
GLLines3Renderer::render(GLLines3& lines, const mat4f& t)
{
  _program.setUniformMat4(_program.mvpMatrixLoc, mvpMatrix(t, *_camera));
  _program.setUniform(_program.useVertexColorsLoc, int(useVertexColors));
  lines.bind();
  for (uint32_t n = lines.lineCount(), i = 0; i < n; ++i)
  {
    auto indices = lines.lineIndices(i);
    glDrawArrays(GL_LINE_STRIP, indices.x, indices.y - indices.x);
  }
}

void
GLLines3Renderer::render(GLLines3& lines,
  const vec3f& p,
  const mat3f& r,
  const vec3f& s)
{
  render(lines, TRS(p, r, s));
}

} // end namespace cg
