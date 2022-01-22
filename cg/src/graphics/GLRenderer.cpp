//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Orthrus Group.                         |
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
// OVERVIEW: GLRenderer.cpp
// ========
// Source file for OpenGL renderer.
//
// Author: Paulo Pagliosa
// Last revision: 21/01/2022

#include "graphics/GLRenderer.h"

namespace cg
{ // begin namespace cg

#define STRINGIFY(A) "#version 400\n"#A


/////////////////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
static const char* vertexShader = STRINGIFY(
  layout(location = 0) in vec4 position;
  layout(location = 1) in vec3 normal;
  layout(location = 2) in vec4 color;
  uniform mat4 mvMatrix;
  uniform mat3 normalMatrix;
  uniform mat4 mvpMatrix;
  out vec3 vPosition;
  out vec3 vNormal;
  out vec4 vColor;

  void main()
  {
    vPosition = vec3(mvMatrix * position);
    vNormal = normalize(normalMatrix * normal);
    gl_Position = mvpMatrix * position;
    vColor = color;
  }
);

static const char* geometryShader = STRINGIFY(
  layout(triangles) in;
  layout(triangle_strip, max_vertices = 3) out;

  in vec3 vPosition[];
  in vec3 vNormal[];
  in vec4 vColor[];
  uniform mat4 _viewportMatrix;
  out vec3 gPosition;
  out vec3 gNormal;
  out vec4 gColor;
  noperspective out vec3 gEdgeDistance;

  void main()
  {
    vec2 p0 = vec2(_viewportMatrix *
      (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec2 p1 = vec2(_viewportMatrix *
      (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec2 p2 = vec2(_viewportMatrix *
      (gl_in[2].gl_Position / gl_in[2].gl_Position.w));
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos((b * b + c * c - a * a) / (2 * b * c));
    float delta = acos((a * a + c * c - b * b) / (2 * a * c));
    float ha = abs(c * sin(delta));
    float hb = abs(c * sin(alpha));
    float hc = abs(b * sin(alpha));

    gEdgeDistance = vec3(ha, 0, 0);
    gPosition = vPosition[0];
    gNormal = vNormal[0];
    gColor = vColor[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gEdgeDistance = vec3(0, hb, 0);
    gPosition = vPosition[1];
    gNormal = vNormal[1];
    gColor = vColor[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gEdgeDistance = vec3(0, 0, hc);
    gPosition = vPosition[2];
    gNormal = vNormal[2];
    gColor = vColor[2];
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
    float s; // specular shininess exponent
  };

  struct LineProps
  {
    float width;
    vec4 color;
  };

  subroutine vec4 mixColorType(vec4 color);
  subroutine void matPropsType(out MaterialProps m);

  in vec3 gPosition;
  in vec3 gNormal;
  in vec4 gColor;
  noperspective in vec3 gEdgeDistance;
  uniform vec4 ambientLight;
  uniform int nbLights;
  uniform LightProps lights[8];
  uniform int lightTypes[8];
  uniform MaterialProps material;
  uniform LineProps line;
  subroutine uniform mixColorType mixColor;
  subroutine uniform matPropsType matProps;
  layout(location = 0) out vec4 fragmentColor;

  subroutine(matPropsType)
  void modelMaterial(out MaterialProps m)
  {
    m = material;
  }

  subroutine(matPropsType)
  void colorMapMaterial(out MaterialProps m)
  {
    const float cmOa = 0.4;
    const float cmOd = 0.6;
    const float cmOs = 0.0;

    m = MaterialProps(gColor * cmOa, gColor * cmOd, gColor * cmOs, 100);
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
    for (int i = 0; i < nbLights; i++)
    {
      vec3 L = lightVector(i, P);
      vec3 V = normalize(P.xyz);
      vec3 R = reflect(L, N);

      color += lights[i].color * (m.Od * max(dot(L, N), 0) +
        m.Os * pow(max(dot(R, V), 0), m.s));
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
    float d = min(min(gEdgeDistance.x, gEdgeDistance.y), gEdgeDistance.z);
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
    fragmentColor = mixColor(phong(gPosition, normalize(gNormal)));
  }
);

struct GLRenderer::GLData
{
  struct LightPropLoc
  {
    GLint position;
    GLint color;
  };

  GLSL::Program program;
  mat4f viewportMatrix;
  GLint mvMatrixLoc;
  GLint normalMatrixLoc;
  GLint mvpMatrixLoc;
  GLint viewportMatrixLoc;
  GLint ambientLightLoc;
  GLint nbLightsLoc;
  LightPropLoc lightLocs[MAX_LIGHTS];
  GLint lightTypeLocs[MAX_LIGHTS];
  GLint OaLoc;
  GLint OdLoc;
  GLint OsLoc;
  GLint nsLoc;
  GLint lineWidthLoc;
  GLint lineColorLoc;
  GLuint noMixIdx;
  GLuint lineColorMixIdx;
  GLuint modelMaterialIdx;
  GLuint colorMapMaterialIdx;

  GLData();

  void uniformLocations();
  void subroutineIndices();
  void renderDefaultLights();

}; // GLRenderer::GLData

inline void
GLRenderer::GLData::uniformLocations()
{
  mvMatrixLoc = program.uniformLocation("mvMatrix");
  normalMatrixLoc = program.uniformLocation("normalMatrix");
  mvpMatrixLoc = program.uniformLocation("mvpMatrix");
  viewportMatrixLoc = program.uniformLocation("_viewportMatrix");
  ambientLightLoc = program.uniformLocation("ambientLight");
  nbLightsLoc = program.uniformLocation("nbLights");
  lightLocs[0].position = program.uniformLocation("lights[0].position");
  lightLocs[0].color = program.uniformLocation("lights[0].color");
  lightTypeLocs[0] = program.uniformLocation("lightTypes[0]");
  lightLocs[1].position = program.uniformLocation("lights[1].position");
  lightLocs[1].color = program.uniformLocation("lights[1].color");
  lightTypeLocs[1] = program.uniformLocation("lightTypes[1]");
  lightLocs[2].position = program.uniformLocation("lights[2].position");
  lightLocs[2].color = program.uniformLocation("lights[2].color");
  lightTypeLocs[2] = program.uniformLocation("lightTypes[2]");
  lightLocs[3].position = program.uniformLocation("lights[3].position");
  lightLocs[3].color = program.uniformLocation("lights[3].color");
  lightTypeLocs[3] = program.uniformLocation("lightTypes[3]");
  lightLocs[4].position = program.uniformLocation("lights[4].position");
  lightLocs[4].color = program.uniformLocation("lights[4].color");
  lightTypeLocs[4] = program.uniformLocation("lightTypes[4]");
  lightLocs[5].position = program.uniformLocation("lights[5].position");
  lightLocs[5].color = program.uniformLocation("lights[5].color");
  lightTypeLocs[5] = program.uniformLocation("lightTypes[5]");
  lightLocs[6].position = program.uniformLocation("lights[6].position");
  lightLocs[6].color = program.uniformLocation("lights[6].color");
  lightTypeLocs[6] = program.uniformLocation("lightTypes[6]");
  lightLocs[7].position = program.uniformLocation("lights[7].position");
  lightLocs[7].color = program.uniformLocation("lights[7].color");
  lightTypeLocs[7] = program.uniformLocation("lightTypes[7]");
  OaLoc = program.uniformLocation("material.Oa");
  OdLoc = program.uniformLocation("material.Od");
  OsLoc = program.uniformLocation("material.Os");
  nsLoc = program.uniformLocation("material.s");
  lineWidthLoc = program.uniformLocation("line.width");
  lineColorLoc = program.uniformLocation("line.color");
}

inline void
GLRenderer::GLData::subroutineIndices()
{
  noMixIdx = program.fragmentSubroutineIndex("noMix");
  lineColorMixIdx = program.fragmentSubroutineIndex("lineColorMix");
  modelMaterialIdx = program.fragmentSubroutineIndex("modelMaterial");
  colorMapMaterialIdx = program.fragmentSubroutineIndex("colorMapMaterial");
}

inline void
GLRenderer::GLData::renderDefaultLights()
{
  program.setUniformVec4(lightLocs[0].position, vec4f{0, 0, 0, 1});
  program.setUniform(lightLocs[0].color, 1, 1, 1, 0);
  program.setUniform(lightTypeLocs[0], 0);
  program.setUniform(nbLightsLoc, 1);
}

inline
GLRenderer::GLData::GLData():
  program{"Renderer Program"}
{
  program.setShader(GL_GEOMETRY_SHADER, geometryShader);
  program.setShader(GL_VERTEX_SHADER, vertexShader);
  program.setShader(GL_FRAGMENT_SHADER, fragmentShader);
  program.use();
  uniformLocations();
  subroutineIndices();
}

GLRenderer::GLRenderer(SceneBase& scene, Camera& camera):
  GLRendererBase{scene, camera},
  _gl{new GLData{}}
{
  // do nothing
}

GLRenderer::~GLRenderer()
{
  delete _gl;
}

void
GLRenderer::update()
{
  GLGraphics3::setView(_camera->position(), vpMatrix(_camera));
  glViewport(0, 0, _viewport.w, _viewport.h);

  auto w = _viewport.w / 2.0f;
  auto h = _viewport.h / 2.0f;

  _gl->viewportMatrix[0].set(w, 0, 0, 0);
  _gl->viewportMatrix[1].set(0, h, 0, 0);
  _gl->viewportMatrix[2].set(0, 0, 1, 0);
  _gl->viewportMatrix[3].set(w, h, 0, 0);
}

void
GLRenderer::renderLights()
{
  _gl->program.setUniformVec4(_gl->ambientLightLoc, _scene->ambientLight);
  if (_scene->lightCount() == 0)
  {
    _gl->renderDefaultLights();
    return;
  }

  const auto& vm = _camera->worldToCameraMatrix();
  int nl{0};

  for (auto& light : _scene->lights())
  {
    if (light->isTurnedOn())
    {
      if (light->type() == Light::Directional)
      {
        const auto d = vm.transformVector(light->direction);

        _gl->program.setUniformVec4(_gl->lightLocs[nl].position, d);
        _gl->program.setUniform(_gl->lightTypeLocs[nl], (int)1);
      }
      else
      {
        const auto p = vm * vec4f{light->position};

        _gl->program.setUniformVec4(_gl->lightLocs[nl].position, p);
        _gl->program.setUniform(_gl->lightTypeLocs[nl], (int)0);
      }
      _gl->program.setUniformVec4(_gl->lightLocs[nl].color, light->color);
      if (++nl == MAX_LIGHTS)
        break;
    }
  }
  _gl->program.setUniform(_gl->nbLightsLoc, nl);
}

void
GLRenderer::renderActors()
{
  for (auto& actor : _scene->actors())
  {
    if (!actor->isVisible())
      continue;
    if (auto mapper = actor->mapper())
    {
      mapper->update();
      if (!mapper->render(*this))
        if (auto primitive = mapper->primitive(); drawMesh(*primitive))
          if (flags.isSet(DrawActorBounds))
          {
            setLineColor(boundsColor);
            drawBounds(primitive->bounds());
          }
    }
  }
}

void
GLRenderer::beginRender()
{
  const auto& bc = _scene->backgroundColor;

  glClearColor((float)bc.r, (float)bc.g, (float)bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  _gl->program.use();
  _gl->program.setUniformMat4(_gl->viewportMatrixLoc, _gl->viewportMatrix);
  glPolygonMode(GL_FRONT_AND_BACK,
    (renderMode != RenderMode::Wireframe) + GL_LINE);
}

void
GLRenderer::render()
{
  update();
  beginRender();
  renderLights();
  renderActors();
  endRender();
}

void
GLRenderer::endRender()
{
  if (_renderFunction != nullptr)
    _renderFunction(*this);
  glFlush();
  _gl->program.disuse();
}

void
GLRenderer::renderMaterial(const Material& material)
{
  _gl->program.setUniformVec4(_gl->OaLoc, material.ambient);
  _gl->program.setUniformVec4(_gl->OdLoc, material.diffuse);
  _gl->program.setUniformVec4(_gl->OsLoc, material.spot);
  _gl->program.setUniform(_gl->nsLoc, material.shine);
  //_gl->program.setUniform(_gl->lineWidthLoc, material.lineWidth);
  //_gl->program.setUniformVec4(_gl->lineColorLoc, material.lineColor);
}

inline mat4f
mvMatrix(const Camera& c, const Primitive& p)
{
  return c.worldToCameraMatrix() * p.localToWorldMatrix();
}

inline mat4f
mvpMatrix(const mat4f& mvm, const Camera& c)
{
  return c.projectionMatrix() * mvm;
}

bool
GLRenderer::drawMesh(const Primitive& primitive)
{
  auto mesh = primitive.tesselate();

  if (nullptr == mesh)
    return false;

  auto mvm = mvMatrix(*_camera, primitive);

  _gl->program.setUniformMat4(_gl->mvMatrixLoc, mvm);
  _gl->program.setUniformMat4(_gl->mvpMatrixLoc, mvpMatrix(mvm, *_camera));
  _gl->program.setUniformMat3(_gl->normalMatrixLoc, primitive.normalMatrix());

  GLuint subIds[2];

  subIds[0] = renderMode == RenderMode::HiddenLines ?
    _gl->lineColorMixIdx :
    _gl->noMixIdx;
  subIds[1] = flags.isSet(UseVertexColors) ?
    _gl->colorMapMaterialIdx :
    _gl->modelMaterialIdx;
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subIds);
  renderMaterial(*primitive.material());
  if (auto m = glMesh(mesh))
  {
    m->bind();
    glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
  }
  return true;
}

void
GLRenderer::drawAxes(const mat4f& m)
{
  mat3f r{m};

  r[0].normalize();
  r[1].normalize();
  r[2].normalize();
  GLGraphics3::drawAxes(vec3f{m[3]}, r);
}

} // end namespace cg
