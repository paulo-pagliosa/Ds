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
// Last revision: 18/09/2023

#include "graphics/GLLines3Renderer.h"

namespace cg
{ // begin namespace cg

#define STRINGIFY(A) "#version 400\n"#A

static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec4 color;
  uniform mat4 mvpMatrix;
  uniform int usePointColors;
  uniform vec4 lineColor;
  out vec4 v_color;

  void main()
  {
    gl_Position = mvpMatrix * position;
    v_color = usePointColors != 0 ? color : lineColor;
  }
);

static const char* geometryShader = STRINGIFY(
  layout(lines) in;
  layout(triangle_strip, max_vertices = 4) out;

  in vec4 v_color[];
  uniform vec2 viewportSize;
  uniform float lineWidth;
  out vec4 g_color;
  noperspective out float smoothline;

  void handleVertex(const int i, vec2 offset)
  {
    g_color = v_color[i];
    smoothline = (lineWidth + 1) * 0.5f;
    gl_Position = gl_in[i].gl_Position;
    gl_Position.xy += offset * gl_Position.w;
    EmitVertex();
    smoothline *= -1;
    gl_Position = gl_in[i].gl_Position;
    gl_Position.xy -= offset * gl_Position.w;
    EmitVertex();
  }

  void main()
  {
    vec2 p0 = gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
    vec2 p1 = gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
    vec2 pd = normalize((p1 - p0) * viewportSize);
    vec2 offset = vec2(-pd.y, pd.x) * (lineWidth + 1) / viewportSize;

    handleVertex(0, offset);
    handleVertex(1, offset);
    EndPrimitive();
  }
);

static const char* fragmentShader = STRINGIFY(
  in vec4 v_color;// g_color;
  //noperspective in float smoothline;
  //uniform float lineWidth;
  out vec4 f_color;

  void main()
  {
    f_color = v_color;// g_color;
    //f_color.a *= clamp((lineWidth + 1) * 0.5f - abs(smoothline), 0, 1);
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
  usePointColorsLoc = uniformLocation("usePointColors");
  lineColorLoc = uniformLocation("lineColor");
  //lineWidthLoc = uniformLocation("lineWidth");
  //viewportSizeLoc = uniformLocation("viewportSize");
}

inline void
GLLines3Renderer::GLProgram::initProgram()
{
  //setShader(GL_GEOMETRY_SHADER, geometryShader);
  setShader(GL_VERTEX_SHADER, vertexShader);
  setShader(GL_FRAGMENT_SHADER, fragmentShader).use();
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

inline void
GLLines3Renderer::updateView()
{
  camera()->update();
  /*
  GLint v[4];

  glGetIntegerv(GL_VIEWPORT, v);
  _program.setUniformVec2(_program.viewportSizeLoc,
    vec2f{float(v[2]), float(v[3])});
  */
}

void
GLLines3Renderer::begin()
{
  if (auto cp = GLSL::Program::current(); &_program != cp)
  {
    _lastState.program = cp;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
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
GLLines3Renderer::drawLines(const GLLines3& lines,
  const mat4f& t,
  int count,
  int offset)
{
  _program.setUniformMat4(_program.mvpMatrixLoc, mvpMatrix(t, *camera()));
  _program.setUniform(_program.usePointColorsLoc, int(usePointColors));
  //_program.setUniform(_program.lineWidthLoc, _lineWidth);
  lines.bind();
  for (int n = offset + count, i = offset; i < n; ++i)
  {
    auto indices = lines.lineIndices(i);
    glDrawArrays(GL_LINE_STRIP, indices.x, indices.y - indices.x);
  }
}

void
GLLines3Renderer::render(const GLLines3& lines,
  const vec3f& p,
  const mat3f& r,
  const vec3f& s)
{
  render(lines, TRS(p, r, s));
}

void
GLLines3Renderer::render(const GLLines3& lines, int index, const mat4f& t)
{
  assert(index >= 0 && index < (int)lines.lineCount());
  drawLines(lines, t, 1, index);
}
} // end namespace cg
