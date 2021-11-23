//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// Last revision: 04/06/2019

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
    v_position = vec3(mvMatrix * position);
    v_normal = normalize(normalMatrix * normal);
    gl_Position = mvpMatrix * position;
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
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos((b * b + c * c - a * a) / (2 * b * c));
    float delta = acos((a * a + c * c - b * b) / (2 * a * c));
    float ha = abs(c * sin(delta));
    float hb = abs(c * sin(alpha));
    float hc = abs(b * sin(alpha));

    g_edgeDistance = vec3(ha, 0, 0);
    g_position = v_position[0];
    g_normal = v_normal[0];
    g_color = v_color[0];
    g_uv = v_uv[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    g_edgeDistance = vec3(0, hb, 0);
    g_position = v_position[1];
    g_normal = v_normal[1];
    g_color = v_color[1];
    g_uv = v_uv[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    g_edgeDistance = vec3(0, 0, hc);
    g_position = v_position[2];
    g_normal = v_normal[2];
    g_color = v_color[2];
    g_uv = v_uv[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
  }
);

static const char* fragmentShader = STRINGIFY(
  struct LightProps
  {
    vec4 position; // light position in eye coordinates
    vec4 color; // light color
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
  uniform vec4 ambientLight;
  uniform int lightCount;
  uniform LightProps lights[8];
  uniform int lightTypes[8];
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

  vec3 lightVector(int i, vec3 P)
  {
    if (lightTypes[i] == 1) // directional light
      return -vec3(lights[i].position);
    return normalize(vec3(lights[i].position) - P);
  }

  vec4 phong(vec3 P, vec3 N)
  {
    MaterialProps m;
    vec4 color;

    matProps(m);
    color = ambientLight * m.Oa;
    for (int i = 0; i < lightCount; i++)
    {
      vec3 L = lightVector(i, P);
      vec3 V = normalize(P.xyz);
      vec3 R = reflect(L, N);

      color += lights[i].color * (m.Od * max(dot(L, N), 0) +
        m.Os * pow(max(dot(R, V), 0), m.shine));
    }
    return color;
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
GLMeshRenderer::initUniformLocations()
{
  _mvMatrixLoc = _program.uniformLocation("mvMatrix");
  _normalMatrixLoc = _program.uniformLocation("normalMatrix");
  _mvpMatrixLoc = _program.uniformLocation("mvpMatrix");
  _viewportMatrixLoc = _program.uniformLocation("viewportMatrix");
  _ambientLightLoc = _program.uniformLocation("ambientLight");
  _lightCountLoc = _program.uniformLocation("lightCount");
  _lightLocs[0].position = _program.uniformLocation("lights[0].position");
  _lightLocs[0].color = _program.uniformLocation("lights[0].color");
  _lightTypeLocs[0] = _program.uniformLocation("lightTypes[0]");
  _lightLocs[1].position = _program.uniformLocation("lights[1].position");
  _lightLocs[1].color = _program.uniformLocation("lights[1].color");
  _lightTypeLocs[1] = _program.uniformLocation("lightTypes[1]");
  _lightLocs[2].position = _program.uniformLocation("lights[2].position");
  _lightLocs[2].color = _program.uniformLocation("lights[2].color");
  _lightTypeLocs[2] = _program.uniformLocation("lightTypes[2]");
  _lightLocs[3].position = _program.uniformLocation("lights[3].position");
  _lightLocs[3].color = _program.uniformLocation("lights[3].color");
  _lightTypeLocs[3] = _program.uniformLocation("lightTypes[3]");
  _lightLocs[4].position = _program.uniformLocation("lights[4].position");
  _lightLocs[4].color = _program.uniformLocation("lights[4].color");
  _lightTypeLocs[4] = _program.uniformLocation("lightTypes[4]");
  _lightLocs[5].position = _program.uniformLocation("lights[5].position");
  _lightLocs[5].color = _program.uniformLocation("lights[5].color");
  _lightTypeLocs[5] = _program.uniformLocation("lightTypes[5]");
  _lightLocs[6].position = _program.uniformLocation("lights[6].position");
  _lightLocs[6].color = _program.uniformLocation("lights[6].color");
  _lightTypeLocs[6] = _program.uniformLocation("lightTypes[6]");
  _lightLocs[7].position = _program.uniformLocation("lights[7].position");
  _lightLocs[7].color = _program.uniformLocation("lights[7].color");
  _lightTypeLocs[7] = _program.uniformLocation("lightTypes[7]");
  _useTextureLoc = _program.uniformLocation("useTexture");
  _OaLoc = _program.uniformLocation("material.Oa");
  _OdLoc = _program.uniformLocation("material.Od");
  _OsLoc = _program.uniformLocation("material.Os");
  _nsLoc = _program.uniformLocation("material.shine");
  _lineWidthLoc = _program.uniformLocation("line.width");
  _lineColorLoc = _program.uniformLocation("line.color");
}

inline void
GLMeshRenderer::initSubroutineIndices()
{
  _noMixIdx = _program.fragmentSubroutineIndex("noMix");
  _lineColorMixIdx = _program.fragmentSubroutineIndex("lineColorMix");
  _modelMaterialIdx = _program.fragmentSubroutineIndex("modelMaterial");
  _colorMapMaterialIdx = _program.fragmentSubroutineIndex("colorMapMaterial");
}

inline void
GLMeshRenderer::setDefaultLights()
{
  _program.setUniformVec4(_lightLocs[0].position, vec4f{0, 0, 0, 1});
  _program.setUniform(_lightLocs[0].color, 1, 1, 1, 0);
  _program.setUniform(_lightTypeLocs[0], 0);
  _program.setUniform(_lightCountLoc, _lightCount = 1);
}

inline auto
lightDirection(const Light& light, Camera* camera)
{
  if (light.flags.isSet(Light::LightBits::Camera))
    return light.direction;
  return camera->worldToCameraMatrix().transformVector(light.direction);
}

inline auto
lightPosition(const Light& light, Camera* camera)
{
  if (light.flags.isSet(Light::LightBits::Camera))
    return light.position;
  return camera->worldToCameraMatrix().transform(light.position);
}

bool
GLMeshRenderer::setLight(int i, const Light& light)
{
  if (!light.isTurnedOn())
    return false;
  if (light.isDirectional())
  {
    const auto d = vec4f{lightDirection(light, _camera)};

    _program.setUniformVec4(_lightLocs[i].position, d);
    _program.setUniform(_lightTypeLocs[i], (int)1);
  }
  else
  {
    const auto p = vec4f{lightPosition(light, _camera)};

    _program.setUniformVec4(_lightLocs[i].position, p);
    _program.setUniform(_lightTypeLocs[i], (int)0);
  }
  _program.setUniformVec4(_lightLocs[i].color, light.color);
  return true;
}

inline void
GLMeshRenderer::initProgram()
{
  auto cp = GLSL::Program::current();

  _program.use();
  initUniformLocations();
  initSubroutineIndices();
  setMaterial(Material{});
  _program.setUniform(_lineWidthLoc, 0.5f);
  _program.setUniformVec4(_lineColorLoc, Color::gray);
  _program.setUniformVec4(_ambientLightLoc, Color::darkGray);
  GLSL::Program::setCurrent(cp);
}

GLMeshRenderer::GLMeshRenderer(Camera* camera):
  _camera{camera == nullptr ? new Camera{} : camera},
  _program{"Mesh Renderer"}
{
  _program.setShader(GL_GEOMETRY_SHADER, geometryShader);
  _program.setShader(GL_VERTEX_SHADER, vertexShader);
  _program.setShader(GL_FRAGMENT_SHADER, fragmentShader);
  initProgram();
}

inline void
GLMeshRenderer::updateView()
{
  _camera->update();

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
GLMeshRenderer::setCamera(Camera* camera)
{
  if (camera != _camera)
    (_camera = nullptr != camera ? camera : new Camera{})->update();
}

void
GLMeshRenderer::begin()
{
  _lastState.program = GLSL::Program::current();
  _lastState.depthTest = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &_lastState.texture);
  updateView();
  glPolygonMode(GL_FRONT_AND_BACK, (renderMode != Wireframe) + GL_LINE);
  glEnable(GL_DEPTH_TEST);
  _program.use();
  _program.setUniformMat4(_viewportMatrixLoc, _viewportMatrix);
  if (_lightCount == 0)
    setDefaultLights();
}

void
GLMeshRenderer::end()
{
  GLSL::Program::setCurrent(_lastState.program);
  if (!_lastState.depthTest)
    glDisable(GL_DEPTH_TEST);
  glBindVertexArray(_lastState.vao);
  glBindTexture(GL_TEXTURE_2D, _lastState.texture);
}

void
GLMeshRenderer::setMaterial(const Material& material)
{
  _program.setUniformVec4(_OaLoc, material.ambient);
  _program.setUniformVec4(_OdLoc, material.diffuse);
  _program.setUniformVec4(_OsLoc, material.spot);
  _program.setUniform(_nsLoc, material.shine);
  if (material.texture == nullptr)
    _texture = 0;
  else
    _texture = (GLuint)(intptr_t)material.texture; // TODO
}

inline auto
normalMatrix(const mat3f& n, const Camera* c)
{
  return mat3f{c->worldToCameraMatrix()} * n;
}

void
GLMeshRenderer::render(TriangleMesh& mesh, const mat4f& t, const mat3f& n)
{
  auto mv = _camera->worldToCameraMatrix() * t;

  _program.setUniformMat4(_mvMatrixLoc, mv);
  _program.setUniformMat3(_normalMatrixLoc, normalMatrix(n, _camera));
  _program.setUniformMat4(_mvpMatrixLoc, _camera->projectionMatrix() * mv);

  GLuint i[2];

  i[0] = renderMode == HiddenLines ? _lineColorMixIdx : _noMixIdx;
  if (useVertexColors())
    i[1] = _colorMapMaterialIdx;
  else
  {
    i[1] = _modelMaterialIdx;

    int useTexture{mesh.hasUV() && _texture != 0};

    if (useTexture)
      glBindTexture(GL_TEXTURE_2D, _texture);
    _program.setUniform(_useTextureLoc, useTexture);
  }
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, i);

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
