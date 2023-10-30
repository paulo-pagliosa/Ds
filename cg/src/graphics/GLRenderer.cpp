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
// OVERVIEW: GLRenderer.cpp
// ========
// Source file for OpenGL renderer.
//
// Author: Paulo Pagliosa
// Last revision: 30/10/2023

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
    gl_Position = mvpMatrix * position;
    vPosition = vec3(mvMatrix * position);
    vNormal = normalize(normalMatrix * normal);
    vColor = color;
  }
);

static const char* geometryShader = STRINGIFY(
  layout(triangles) in;
  layout(triangle_strip, max_vertices = 3) out;

  in vec3 vPosition[];
  in vec3 vNormal[];
  in vec4 vColor[];
  uniform mat4 viewportMatrix;
  out vec3 gPosition;
  out vec3 gNormal;
  out vec4 gColor;
  noperspective out vec3 gEdgeDistance;

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
    gEdgeDistance = vec3(abs(a / length(v0)), 0, 0);
    gPosition = vPosition[0];
    gNormal = vNormal[0];
    gColor = vColor[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    gEdgeDistance = vec3(0, abs(a / length(v1)), 0);
    gPosition = vPosition[1];
    gNormal = vNormal[1];
    gColor = vColor[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    gEdgeDistance = vec3(0, 0, abs(a / length(v2)));
    gPosition = vPosition[2];
    gNormal = vNormal[2];
    gColor = vColor[2];
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

  in vec3 gPosition;
  in vec3 gNormal;
  in vec4 gColor;
  noperspective in vec3 gEdgeDistance;
  //uniform vec4 backFaceColor = vec4(1, 0, 1, 1);
  uniform int projectionType; // PERSPECTIVE/PARALLEL
  uniform vec4 ambientLight;
  uniform int lightCount;
  uniform LightProps lights[8];
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
    const float Oa = 0.4;
    const float Od = 0.6;

    m = MaterialProps(gColor * Oa, gColor * Od, vec4(1), material.shine);
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
    if (lights[i].type == 0  || falloff == 0)
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
    GLint type;
    GLint color;
    GLint position;
    GLint direction;
    GLint falloff;
    GLint range;
    GLint angle;
  };

  GLSL::Program program;
  mat4f viewportMatrix;
  GLint mvMatrixLoc;
  GLint normalMatrixLoc;
  GLint mvpMatrixLoc;
  GLint viewportMatrixLoc;
  GLint projectionTypeLoc;
  GLint ambientLightLoc;
  GLint lightCountLoc;
  LightPropLoc lightLocs[maxLights];
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

  GLint uniformLightLocation(int i, const char* field)
  {
    constexpr auto maxName = 32;
    char name[maxName];

    snprintf(name, maxName, "lights[%d].%s", i, field);
    return program.uniformLocation(name);
  }

  void uniformLightLocations(int);
  void uniformLocations();
  void subroutineIndices();
  void renderDefaultLights();

}; // GLRenderer::GLData

inline void
GLRenderer::GLData::uniformLightLocations(int i)
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
GLRenderer::GLData::uniformLocations()
{
  mvMatrixLoc = program.uniformLocation("mvMatrix");
  normalMatrixLoc = program.uniformLocation("normalMatrix");
  mvpMatrixLoc = program.uniformLocation("mvpMatrix");
  viewportMatrixLoc = program.uniformLocation("viewportMatrix");
  projectionTypeLoc = program.uniformLocation("projectionType");
  ambientLightLoc = program.uniformLocation("ambientLight");
  lightCountLoc = program.uniformLocation("lightCount");
  for (auto i = 0; i < maxLights; ++i)
    uniformLightLocations(i);
  OaLoc = program.uniformLocation("material.Oa");
  OdLoc = program.uniformLocation("material.Od");
  OsLoc = program.uniformLocation("material.Os");
  nsLoc = program.uniformLocation("material.shine");
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
  program.setUniform(lightLocs[0].type, 1); // POINT
  program.setUniformVec4(lightLocs[0].color, vec4f{1, 1, 1, 0});
  program.setUniformVec3(lightLocs[0].position, vec3f{0, 0, 0});
  program.setUniform(lightLocs[0].range, 0.0f);
  program.setUniform(lightCountLoc, 1);
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
  // TODO: do it only when camera or viewport has been changed
  GLGraphics3::setView(_camera->position(), vpMatrix(_camera));
  glViewport(0, 0, _viewport.w, _viewport.h);

  auto w = _viewport.w / 2.0f;
  auto h = _viewport.h / 2.0f;

  _gl->viewportMatrix[0].set(w, 0, 0, 0);
  _gl->viewportMatrix[1].set(0, h, 0, 0);
  _gl->viewportMatrix[2].set(0, 0, 1, 0);
  _gl->viewportMatrix[3].set(w, h, 0, 0);
  _windowViewportRatio = _camera->windowHeight() / _viewport.h;
}

void
GLRenderer::setBasePoint(const vec3f& p)
{
  _basePointZ = -_camera->worldToCamera(_basePoint = p).z;
}

float
GLRenderer::pixelsLength(float d) const
{
  if (_camera->projectionType() == Camera::Perspective)
    d *= _basePointZ / _camera->nearPlane();
  return _windowViewportRatio * d;
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

  for (auto light : _scene->lights())
  {
    if (!light->isTurnedOn())
      continue;
    _gl->program.setUniform(_gl->lightLocs[nl].type, (int)light->type());
    _gl->program.setUniformVec4(_gl->lightLocs[nl].color, light->color);
    {
      const auto p = vm.transform3x4(light->position());
      _gl->program.setUniformVec3(_gl->lightLocs[nl].position, p);
    }
    {
      const auto d = vm.transformVector(light->direction()).versor();
      _gl->program.setUniformVec3(_gl->lightLocs[nl].direction, d);
    }
    _gl->program.setUniform(_gl->lightLocs[nl].falloff, (int)light->falloff);
    _gl->program.setUniform(_gl->lightLocs[nl].range, light->range());
    _gl->program.setUniform(_gl->lightLocs[nl].angle, light->spotAngle());
    if (++nl == maxLights)
      break;
  }
  _gl->program.setUniform(_gl->lightCountLoc, nl);
}

void
GLRenderer::renderActors()
{
  for (auto actor : _scene->actors())
  {
    if (!actor->isVisible())
      continue;

    auto mapper = actor->mapper();

    assert(mapper != nullptr);
    mapper->update();
    if (!mapper->render(*this))
    {
      if (auto primitive = mapper->primitive())
        drawMesh(*primitive);
    }
    else if (flags.isSet(DrawBounds))
    {
      setLineColor(boundsColor);
      drawBounds(mapper->bounds());
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
  _gl->program.setUniform(_gl->projectionTypeLoc, _camera->projectionType());
  _gl->program.setUniformMat4(_gl->viewportMatrixLoc, _gl->viewportMatrix);
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

bool
GLRenderer::drawMesh(const Primitive& primitive)
{
  auto mesh = primitive.tesselate();

  if (!mesh)
    return false;

  auto& t = primitive.localToWorldMatrix();
  auto& n = primitive.normalMatrix();

  drawMesh(*mesh, *primitive.material(), t, n, mesh->data().triangleCount, 0);
  if (flags.isSet(DrawBounds))
  {
    setLineColor(boundsColor);
    drawBounds(primitive.bounds());
  }
  /*
  setVectorColor(Color::gray);
  drawNormals(*mesh, t, n, 0.5f);
  */
  return true;
}

void
GLRenderer::drawMesh(const TriangleMesh& mesh,
  const Material& material,
  const mat4f& t,
  const mat3f& n,
  int count,
  int offset)
{
  auto mvm = mvMatrix(t, _camera);

  _gl->program.setUniformMat4(_gl->mvMatrixLoc, mvm);
  _gl->program.setUniformMat4(_gl->mvpMatrixLoc, mvpMatrix(mvm, _camera));
  _gl->program.setUniformMat3(_gl->normalMatrixLoc, normalMatrix(n, _camera));
  glPolygonMode(GL_FRONT_AND_BACK, (renderMode != Wireframe) + GL_LINE);

  GLuint subIds[2];

  subIds[0] = renderMode != HiddenLines ?
    _gl->noMixIdx :
    _gl->lineColorMixIdx;
  subIds[1] = flags.isSet(UseVertexColors) ?
    _gl->colorMapMaterialIdx :
    _gl->modelMaterialIdx;
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subIds);
  renderMaterial(material);

  auto m = glMesh(&mesh);

  m->bind();
  glDrawElements(GL_TRIANGLES,
    count * 3,
    GL_UNSIGNED_INT,
    (void*)(sizeof(TriangleMesh::Triangle) * offset));
}

bool
GLRenderer::drawSubMesh(const TriangleMesh& mesh,
  int count,
  int offset,
  const Material& material,
  const mat4f& t,
  const mat3f& n)
{
  {
    if (count <= 0 || offset < 0)
      return false;

    const auto nt = mesh.data().triangleCount;

    if (offset >= nt)
      return false;
    if (auto end = offset + count; end > nt)
      count = end - nt;
  }
  drawMesh(mesh, material, t, n, count, offset);
  return true;
}

void
GLRenderer::drawAxes(const mat4f& m, float s)
{
  mat3f r{m};

  r[0].normalize();
  r[1].normalize();
  r[2].normalize();
  GLGraphics3::drawAxes(vec3f{m[3]}, r, s);
}

} // end namespace cg
