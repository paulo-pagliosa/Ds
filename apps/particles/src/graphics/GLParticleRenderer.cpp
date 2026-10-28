//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLParticleRenderer.cpp
// ========
// Source file for OpenGL particle renderer.
//
// Author: Paulo Pagliosa
// Last revision: 28/10/2026

#include "graphics/GLParticleRenderer.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLParticleRenderer implementation
// ==================
static const char* vertexShader = R"glsl(
  #version 400
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec4 color;
  uniform mat4 mvMatrix;
  uniform mat4 projectionMatrix;
  uniform vec2 viewportSize;
  uniform int useParticleColors;
  uniform vec4 particleColor;
  uniform float radius;
  out vec4 vColor;

  void main()
  {
    vec4 p = mvMatrix * position;

    gl_Position = p;

    float d = radius * 2;
    vec4 v = projectionMatrix * vec4(d, d, p.z, p.w);
    vec2 s = viewportSize * v.xy / v.w;

    gl_PointSize = 0.25f * (s.x + s.y);
    vColor = useParticleColors != 0 ? color : particleColor;
  }
)glsl";

static const char* geometryShader = R"glsl(
  #version 400
  layout(points) in;
  layout(triangle_strip, max_vertices = 4) out;
  in vec4 vColor[];
  uniform mat4 projectionMatrix;
  uniform float radius;
  out vec4 gColor;
  out vec2 uv;

  void main()
  {
    gColor = vColor[0];

    vec4 p = gl_in[0].gl_Position;
    float pointSize = gl_in[0].gl_PointSize;

    gl_Position = projectionMatrix * (vec4(-radius, -radius, 0, 0) + p);
    gl_PointSize = pointSize;
    uv = vec2(0, 0);
    EmitVertex();
    gl_Position = projectionMatrix * (vec4(+radius, -radius, 0, 0) + p);
    gl_PointSize = pointSize;
    uv = vec2(1, 0);
    EmitVertex();
    gl_Position = projectionMatrix * (vec4(-radius, +radius, 0, 0) + p);
    gl_PointSize = pointSize;
    uv = vec2(0, 1);
    EmitVertex();
    gl_Position = projectionMatrix * (vec4(+radius, +radius, 0, 0) + p);
    gl_PointSize = pointSize;
    uv = vec2(1, 1);
    EmitVertex();
    EndPrimitive();
  }
)glsl";

static const char* fragmentShader = R"glsl(
  #version 400
  in vec2 uv;
  in vec4 gColor;
  out vec4 fragmentColor;

  void main()
  {
    const vec3 lightDir = vec3(0.577f, 0.577f, 0.577f);
    vec3 N;

    N.xy = uv * 2 - vec2(1);

    float s = dot(N.xy, N.xy);

    if (s > 1)
      discard;
    N.z = sqrt(1 - s);
    fragmentColor = gColor * max(0, dot(lightDir, N));

  }
)glsl";


/////////////////////////////////////////////////////////////////////
//
// GLParticleRenderer implementation
// ==================
inline void
GLParticleRenderer::GLProgram::initUniformLocations()
{
  mvMatrixLoc = uniformLocation("mvMatrix");
  projectionMatrixLoc = uniformLocation("projectionMatrix");
  viewportSizeLoc = uniformLocation("viewportSize");
  useParticleColorsLoc = uniformLocation("useParticleColors");
  particleColorLoc = uniformLocation("particleColor");
  radiusLoc = uniformLocation("radius");
}

inline void
GLParticleRenderer::GLProgram::initProgram()
{
  setShader(GL_GEOMETRY_SHADER, geometryShader);
  setShader(GL_VERTEX_SHADER, vertexShader);
  setShader(GL_FRAGMENT_SHADER, fragmentShader).use();
  initUniformLocations();
}

inline void
GLParticleRenderer::GLProgram::setParticleColor(const Color& color)
{
  setUniformVec4(particleColorLoc, color);
}

GLParticleRenderer::GLProgram::GLProgram():
  GLSL::Program{"Particle Renderer"}
{
  auto cp = GLSL::Program::current();

  initProgram();
  setParticleColor(Color::black);
  GLSL::Program::setCurrent(cp);
}

namespace
{ // begin namespace

inline mat4f
mvMatrix(const mat4f& t, const Camera& c)
{
  return c.worldToCameraMatrix() * t;
}

} // end namespace

inline void
GLParticleRenderer::GLProgram::setMVP(const mat4f& t, const Camera& camera)
{
  setUniformMat4(mvMatrixLoc, mvMatrix(t, camera));
  setUniformMat4(projectionMatrixLoc, camera.projectionMatrix());
}

inline void
GLParticleRenderer::GLProgram::setUseParticleColors(bool flag)
{
  setUniform(useParticleColorsLoc, (int)flag);
}

inline void
GLParticleRenderer::GLProgram::setRadius(float radius)
{
  setUniform(radiusLoc, radius);
}


inline void
GLParticleRenderer::GLProgram::setViewportSize(float w, float h)
{
  setUniform(viewportSizeLoc, w, h);
}

inline void
GLParticleRenderer::updateView()
{
  camera()->update();
}

void
GLParticleRenderer::begin()
{
  if (auto cp = GLSL::Program::current(); &_program != cp)
  {
    _lastState.program = cp;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
    _program.use();
    _program.setUseParticleColors(useParticleColors);
    updateView();
  }
}

void
GLParticleRenderer::setParticleColor(const Color& color)
{
  _program.assertInUse();
  _program.setParticleColor(color);
}

void
GLParticleRenderer::drawParticles(const GLPoints3& points,
  const mat4f& t,
  int count,
  int offset)
{
  _program.setMVP(t, *camera());
  _program.setRadius(_radius);
  {
    GLint v[4];

    glGetIntegerv(GL_VIEWPORT, v);
    _program.setViewportSize(float(v[2]), float(v[3]));
  }
  points.bind();
  glEnable(GL_PROGRAM_POINT_SIZE);
  glDrawArrays(GL_POINTS, offset, count);
}

void
GLParticleRenderer::end()
{
  if (auto cp = GLSL::Program::current(); &_program == cp)
  {
    GLSL::Program::setCurrent(_lastState.program);
    glBindVertexArray(_lastState.vao);
  }
}

} // begin namespace cg
