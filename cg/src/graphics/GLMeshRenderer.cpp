//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: GLMeshRenderer.cpp
// ========
// Source file for OpenGL mesh renderer.
//
// Author: Paulo Pagliosa
// Last revision: 02/09/2023

#include "graphics/GLMeshRenderer.h"

namespace cg
{ // begin namespace cg

#define STRINGIFY(A) "#version 400\n"#A


/////////////////////////////////////////////////////////////////////
//
// GLMeshRenderer implementation
// ==============
static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec3 normal;
  layout(location = 2) in vec2 uv;
  layout(location = 3) in vec4 color;
  uniform mat4 mvMatrix;
  uniform mat3 normalMatrix;
  uniform mat4 mvpMatrix;
  out vec3 v_position;
  out vec3 v_normal;
  out vec2 v_uv;
  out vec4 v_color;

  void main()
  {
    gl_Position = mvpMatrix * position;
    v_position = vec3(mvMatrix * position);
    v_normal = normalize(normalMatrix * normal);
    v_color = color;
    v_uv = uv;
  }
);

static const char* geometryShader = STRINGIFY(
  layout(triangles) in;
  layout(triangle_strip, max_vertices = 3) out;

  in vec3 v_position[];
  in vec3 v_normal[];
  in vec2 v_uv[];
  in vec4 v_color[];
  uniform mat4 viewportMatrix;
  out vec3 g_position;
  out vec3 g_normal;
  out vec2 g_uv;
  out vec4 g_color;
  noperspective out vec3 g_edgeDistance;

  void main()
  {
    vec2 p0 = vec2(viewportMatrix *
      (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec2 p1 = vec2(viewportMatrix *
      (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec2 p2 = vec2(viewportMatrix *
      (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
    vec2 v0 = p2 - p1;
    vec2 v1 = p2 - p0;
    vec2 v2 = p1 - p0;
    float a = v2.x * v1.y - v1.x * v2.y;

    gl_Position = gl_in[0].gl_Position;
    g_edgeDistance = vec3(abs(a / length(v0)), 0, 0);
    g_position = v_position[0];
    g_normal = v_normal[0];
    g_color = v_color[0];
    g_uv = v_uv[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    g_edgeDistance = vec3(0, abs(a / length(v1)), 0);
    g_position = v_position[1];
    g_normal = v_normal[1];
    g_color = v_color[1];
    g_uv = v_uv[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    g_edgeDistance = vec3(0, 0, abs(a / length(v2)));
    g_position = v_position[2];
    g_normal = v_normal[2];
    g_color = v_color[2];
    g_uv = v_uv[2];
    EmitVertex();
    EndPrimitive();
  }
);

static const char* fragmentShader = STRINGIFY(
  struct LightProps
  {
    int type; // DIRECTIONAL/POINT/SPOT
    vec4 color; // color
    vec3 position; // VRC position
    vec3 direction; // VRC direction
    int falloff; // CONSTANT/LINEAR/QUADRATIC
    float range; // range (== 0 INFINITE)
    float angle; // spot angle
  };

  struct MaterialProps
  {
    vec4 Oa; // ambient color
    vec4 Od; // diffuse color
    vec4 Os; // specular spot color
    float shine; // specular shininess exponent
  };

  struct LineProps
  {
    float width;
    vec4 color;
  };

  subroutine vec4 mixColorType(vec4 color);
  subroutine void matPropsType(out MaterialProps m);

  in vec3 g_position;
  in vec3 g_normal;
  in vec2 g_uv;
  in vec4 g_color;
  noperspective in vec3 g_edgeDistance;
  uniform int projectionType; // PERSPECTIVE/PARALLEL
  uniform vec4 ambientLight;
  uniform int lightCount;
  uniform LightProps lights[8];
  uniform MaterialProps material;
  uniform int useTexture;
  uniform LineProps line;
  uniform sampler2D sampler;
  subroutine uniform mixColorType mixColor;
  subroutine uniform matPropsType matProps;
  layout(location = 0) out vec4 fragmentColor;

  subroutine(matPropsType)
  void modelMaterial(out MaterialProps m)
  {
    if (useTexture != 0)
    {
      m.Oa = vec4(0);
      m.Od = texture(sampler, g_uv);
    }
    else
    {
      m.Oa = material.Oa;
      m.Od = material.Od;
    }
    m.Os = material.Os;
    m.shine = material.shine;
  }

  subroutine(matPropsType)
  void colorMapMaterial(out MaterialProps m)
  {
    const float cmOa = 0.4;
    const float cmOd = 0.6;
    const float cmOs = 0.0;

    m = MaterialProps(g_color * cmOa, g_color * cmOd, g_color * cmOs, 100);
  }

  bool lightVector(int i, vec3 P, out vec3 L, out float d)
  {
    int type = lights[i].type;

    // DIRECTIONAL
    if (type == 0)
    {
      L = -lights[i].direction;
      return true;
    }
    L = lights[i].position - P;
    d = length(L);

    float range = lights[i].range;

    if (d == 0 || (range > 0 && d > range))
      return false;
    L /= d;
    // POINT
    if (type == 1)
      return true;

    // SPOT
    float DL = dot(lights[i].direction, L);

    return DL < 0 && lights[i].angle > radians(acos(DL));
  }

  vec4 lightColor(int i, float d)
  {
    int falloff = lights[i].falloff;

    // directional light or constant falloff
    if (lights[i].type == 0 || falloff == 0)
      return lights[i].color;

    float range = lights[i].range;
    float f;

    if (range == 0) // infinite range
    {
      f = 1 / d;
      if (falloff == 2) // quadratic falloff
        f *= f;
    }
    else
    {
      f = d / range;
      f = falloff == 2 ? 1 + f * (f - 2) : 1 - f;
    }
    return lights[i].color * f;
  }

  vec4 phong(vec3 P, vec3 N)
  {
    MaterialProps m;
    vec4 color;

    matProps(m);
    color = ambientLight * m.Oa;

    vec3 V = projectionType == 0 ?
      // PERSPECTIVE
      normalize(P) :
      // PARALLEL
      vec3(0, 0, -1);

    if (dot(N, V) > 0)
      //return backFaceColor;
      N *= -1;

    vec3 R = reflect(V, N);

    for (int i = 0; i < lightCount; i++)
    {
      vec3 L; float d;

      if (lightVector(i, P, L, d))
      {
        vec4 I = lightColor(i, d);

        color += I * m.Od * max(dot(N, L), 0);
        color += I * m.Os * pow(max(dot(R, L), 0), m.shine);
      }
    }
    return min(color, vec4(1));
  }

  subroutine(mixColorType)
  vec4 noMix(vec4 color)
  {
    return color;
  }

  subroutine(mixColorType)
  vec4 lineColorMix(vec4 color)
  {
    float d = min(min(g_edgeDistance.x, g_edgeDistance.y), g_edgeDistance.z);
    float mixVal;

    if (d < line.width - 1)
      mixVal = 1;
    else if (d > line.width + 1)
      mixVal = 0;
    else
    {
      float x = d - (line.width - 1);
      mixVal = exp2(-2 * (x * x));
    }
    return mix(color, line.color, mixVal);
  }

  void main()
  {
    fragmentColor = mixColor(phong(g_position, normalize(g_normal)));
  }
);

inline void
GLMeshRenderer::GLProgram::initUniformLightLocations(int i)
{
  lightLocs[i].type = uniformLightLocation(i, "type");
  lightLocs[i].color = uniformLightLocation(i, "color");
  lightLocs[i].position = uniformLightLocation(i, "position");
  lightLocs[i].direction = uniformLightLocation(i, "direction");
  lightLocs[i].falloff = uniformLightLocation(i, "falloff");
  lightLocs[i].range = uniformLightLocation(i, "range");
  lightLocs[i].angle = uniformLightLocation(i, "angle");
}

inline void
GLMeshRenderer::GLProgram::initUniformLocations()
{
  mvMatrixLoc = uniformLocation("mvMatrix");
  normalMatrixLoc = uniformLocation("normalMatrix");
  mvpMatrixLoc = uniformLocation("mvpMatrix");
  viewportMatrixLoc = uniformLocation("viewportMatrix");
  projectionTypeLoc = uniformLocation("projectionType");
  ambientLightLoc = uniformLocation("ambientLight");
  lightCountLoc = uniformLocation("lightCount");
  for (auto i = 0; i < maxLights; ++i)
    initUniformLightLocations(i);
  useTextureLoc = uniformLocation("useTexture");
  OaLoc = uniformLocation("material.Oa");
  OdLoc = uniformLocation("material.Od");
  OsLoc = uniformLocation("material.Os");
  nsLoc = uniformLocation("material.shine");
  lineWidthLoc = uniformLocation("line.width");
  lineColorLoc = uniformLocation("line.color");
}

inline void
GLMeshRenderer::GLProgram::initSubroutineIndices()
{
  noMixIdx = fragmentSubroutineIndex("noMix");
  lineColorMixIdx = fragmentSubroutineIndex("lineColorMix");
  modelMaterialIdx = fragmentSubroutineIndex("modelMaterial");
  colorMapMaterialIdx = fragmentSubroutineIndex("colorMapMaterial");
}

inline void
GLMeshRenderer::GLProgram::renderDefaultLights()
{
  setUniform(lightLocs[0].type, 1); // POINT
  setUniformVec4(lightLocs[0].color, vec4f{1, 1, 1, 0});
  setUniformVec3(lightLocs[0].position, vec3f{0, 0, 0});
  setUniform(lightLocs[0].range, 0.0f);
  setUniform(lightCountLoc, 1);
}

inline void
GLMeshRenderer::GLProgram::initProgram()
{
  setShader(GL_GEOMETRY_SHADER, geometryShader);
  setShader(GL_VERTEX_SHADER, vertexShader);
  setShader(GL_FRAGMENT_SHADER, fragmentShader).use();
  initUniformLocations();
  initSubroutineIndices();
}

GLMeshRenderer::GLProgram::GLProgram():
  GLSL::Program{"Mesh Renderer"}
{
  auto cp = GLSL::Program::current();

  initProgram();
  setUniform(lineWidthLoc, 0.5f);
  setUniformVec4(lineColorLoc, Color::gray);
  setUniformVec4(ambientLightLoc, Color::darkGray);
  renderMaterial(*Material::defaultMaterial());
  GLSL::Program::setCurrent(cp);
}

void
GLMeshRenderer::GLProgram::renderMaterial(const Material& material)
{
  setUniformVec4(OaLoc, material.ambient);
  setUniformVec4(OdLoc, material.diffuse);
  setUniformVec4(OsLoc, material.spot);
  setUniform(nsLoc, material.shine);
}

inline auto
lightDirection(const Light& light, const Camera& camera)
{
  if (light.flags.isSet(Light::LightBits::Camera))
    return light.direction();
  return camera.worldToCameraMatrix().transformVector(light.direction());
}

inline auto
lightPosition(const Light& light, const Camera& camera)
{
  if (light.flags.isSet(Light::LightBits::Camera))
    return light.position();
  return camera.worldToCameraMatrix().transform3x4(light.position());
}

void
GLMeshRenderer::GLProgram::renderLight(int i,
  const Light& light,
  const Camera& camera)
{
  setUniform(lightLocs[i].type, (int)light.type());
  setUniformVec4(lightLocs[i].color, light.color);
  setUniformVec3(lightLocs[i].position, lightPosition(light, camera));
  setUniformVec3(lightLocs[i].direction, lightDirection(light, camera));
  setUniform(lightLocs[i].falloff, (int)light.falloff);
  setUniform(lightLocs[i].range, light.range());
  setUniform(lightLocs[i].angle, light.spotAngle());
}

bool
GLMeshRenderer::setLight(int i, const Light& light)
{
  return light.isTurnedOn() ?
    void(_program.renderLight(i, light, *camera())), true : false;
}

inline void
GLMeshRenderer::updateView()
{
  camera()->update();

  GLint v[4];
  
  glGetIntegerv(GL_VIEWPORT, v);

  float w2 = v[2] / 2.0f;
  float h2 = v[3] / 2.0f;

  _viewportMatrix[0].set(w2, 0, 0, 0);
  _viewportMatrix[1].set(0, h2, 0, 0);
  _viewportMatrix[2].set(0, 0, 1, 0);
  _viewportMatrix[3].set(v[0] + w2, v[1] + h2, 0, 0);
}

void
GLMeshRenderer::begin()
{
  if (auto cp = GLSL::Program::current(); &_program != cp)
  {
    _lastState.program = cp;
    _lastState.depthTest = glIsEnabled(GL_DEPTH_TEST);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &_lastState.texture);
    updateView();
    _program.use();
    _program.setUniformMat4(_program.viewportMatrixLoc, _viewportMatrix);
    glPolygonMode(GL_FRONT_AND_BACK, (renderMode != Wireframe) + GL_LINE);
    glEnable(GL_DEPTH_TEST);
  }
}

void
GLMeshRenderer::end()
{
  if (auto cp = GLSL::Program::current(); &_program == cp)
  {
    GLSL::Program::setCurrent(_lastState.program);
    if (!_lastState.depthTest)
      glDisable(GL_DEPTH_TEST);
    glBindVertexArray(_lastState.vao);
    glBindTexture(GL_TEXTURE_2D, _lastState.texture);
  }
}

void
GLMeshRenderer::setMaterial(const Material& material, void* texture)
{
  _program.renderMaterial(material);
  _texture = texture != nullptr ? (GLuint)(intptr_t)texture : 0;
}

namespace
{ // begin namespace

inline mat4f
mvMatrix(const mat4f& t, const Camera* c)
{
  return c->worldToCameraMatrix() * t;
}

inline mat4f
mvpMatrix(const mat4f& mvm, const Camera* c)
{
  return c->projectionMatrix() * mvm;
}

inline auto
normalMatrix(const mat3f& n, const Camera* c)
{
  return mat3f{c->worldToCameraMatrix()} * n;
}

} // end namespace

void
GLMeshRenderer::render(TriangleMesh& mesh, const mat4f& t, const mat3f& n)
{
  if (_lightCount == 0)
    _program.renderDefaultLights();

  auto camera = this->camera();
  auto mv = mvMatrix(t, camera);

  _program.setUniformMat4(_program.mvMatrixLoc, mv);
  _program.setUniformMat4(_program.mvpMatrixLoc, mvpMatrix(mv, camera));
  _program.setUniformMat3(_program.normalMatrixLoc, normalMatrix(n, camera));

  GLuint subIds[2];

  subIds[0] = renderMode == HiddenLines ?
    _program.lineColorMixIdx :
    _program.noMixIdx;
  if (useVertexColors())
    subIds[1] = _program.colorMapMaterialIdx;
  else
  {
    subIds[1] = _program.modelMaterialIdx;

    int useTexture{mesh.hasUV() && _texture != 0};

    if (useTexture)
      glBindTexture(GL_TEXTURE_2D, _texture);
    _program.setUniform(_program.useTextureLoc, useTexture);
  }
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subIds);

  auto m = glMesh(&mesh);

  m->bind();
  glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
}

void
GLMeshRenderer::render(TriangleMesh& mesh,
  const vec3f& p,
  const mat3f& r,
  const vec3f& s)
{
  render(mesh, TRS(p, r, s), normalMatrix(r, s));
}

} // end namespace cg
