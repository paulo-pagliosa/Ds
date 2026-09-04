//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2020, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: GLMeshRendererBase.cpp
// ========
// Source file for OpenGL mesh renderer base.
//
// Author: Paulo Pagliosa
// Last revision: 31/08/2026

#include "graphics/GLMeshRendererBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLMeshRendererBase::Program implementation
// ===========================
static const char* vertexShader = R"glsl(
  #version 400
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
)glsl";

static const char* geometryShader = R"glsl(
  #version 400
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
)glsl";

static const char* fragmentShader = R"glsl(
  #version 400
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

  in vec3 g_position;
  in vec3 g_normal;
  in vec2 g_uv;
  in vec4 g_color;
  noperspective in vec3 g_edgeDistance;
  //uniform vec4 backFaceColor;
  uniform int projectionType; // PERSPECTIVE/PARALLEL
  uniform vec4 ambientLight;
  uniform int lightCount;
  uniform LightProps lights[8];
  uniform MaterialProps material;
  uniform LineProps line;
  uniform sampler2D sampler;
  uniform int hiddenLinesFlag;
  uniform int useTexture;
  uniform int useVertexColors;
  layout(location = 0) out vec4 fragmentColor;

  MaterialProps materialProps()
  {
    MaterialProps m;

    if (useVertexColors != 0)
    {
      const float ka = 0.4;
      const float kd = 0.6;

      m.Oa = ka * g_color;
      m.Od = kd * g_color;
      m.Os = material.Os;
      m.shine = material.shine;
    }
    else
    {
      m.Oa = material.Oa;
      m.Od = useTexture != 0 ? texture(sampler, g_uv) : material.Od;
      m.Os = material.Os;
      m.shine = material.shine;
    }
    return m;
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
    MaterialProps m = materialProps();
    vec3 V = projectionType == 0 ?
      // PERSPECTIVE
      normalize(P) :
      // PARALLEL
      vec3(0, 0, -1);

    if (dot(N, V) > 0)
      //return backFaceColor;
      N *= -1;

    vec3 R = reflect(V, N);
    vec4 color = ambientLight * m.Oa;

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

  float min3(vec3 v)
  {
    return min(min(v.x, v.y), v.z);
  }

  vec4 mixLineColor(vec4 color)
  {
    float d = min3(g_edgeDistance);
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
    vec4 color = phong(g_position, normalize(g_normal));
    fragmentColor = hiddenLinesFlag != 0 ? mixLineColor(color) : color;
  }
)glsl";

inline GLint
GLMeshRendererBase::GLProgram::uniformLightLocation(int i, const char* field)
{
  constexpr auto maxName = 32;
  char name[maxName];

  snprintf(name, maxName, "lights[%d].%s", i, field);
  return uniformLocation(name);
}

inline void
GLMeshRendererBase::GLProgram::initUniformLightLocations(int i)
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
GLMeshRendererBase::GLProgram::initUniformLocations()
{
  mvMatrixLoc = uniformLocation("mvMatrix");
  normalMatrixLoc = uniformLocation("normalMatrix");
  mvpMatrixLoc = uniformLocation("mvpMatrix");
  viewportMatrixLoc = uniformLocation("viewportMatrix");
  projectionTypeLoc = uniformLocation("projectionType");
  ambientLightLoc = uniformLocation("ambientLight");
  for (auto i = 0; i < maxLights; ++i)
    initUniformLightLocations(i);
  lightCountLoc = uniformLocation("lightCount");
  OaLoc = uniformLocation("material.Oa");
  OdLoc = uniformLocation("material.Od");
  OsLoc = uniformLocation("material.Os");
  nsLoc = uniformLocation("material.shine");
  lineWidthLoc = uniformLocation("line.width");
  lineColorLoc = uniformLocation("line.color");
  hiddenLinesFlagLoc = uniformLocation("hiddenLinesFlag");
  useTextureLoc = uniformLocation("useTexture");
  useVertexColorsLoc = uniformLocation("useVertexColors");
}

inline void
GLMeshRendererBase::GLProgram::initProgram()
{
  setShader(GL_GEOMETRY_SHADER, geometryShader);
  setShader(GL_VERTEX_SHADER, vertexShader);
  setShader(GL_FRAGMENT_SHADER, fragmentShader).use();
  initUniformLocations();
}

inline void
GLMeshRendererBase::GLProgram::setLineColor(const Color& c)
{
  setUniformVec4(lineColorLoc, c);
}

inline void
GLMeshRendererBase::GLProgram::setAmbientLight(const Color& c)
{
  setUniformVec4(ambientLightLoc, c);
}

GLMeshRendererBase::GLProgram::GLProgram(GLMeshRendererBase& parent):
  GLSL::Program{"Mesh Renderer"}
{
  auto cp = GLSL::Program::current();

  initProgram();
  setUniform(lineWidthLoc, 0.5f);
  setLineColor(parent.lineColor(0));
  setAmbientLight(Color::darkGray);
  setMaterial(*Material::defaultMaterial());
  GLSL::Program::setCurrent(cp);
}

void
GLMeshRendererBase::GLProgram::setMaterial(const Material& material)
{
  setUniformVec4(OaLoc, material.ambient);
  setUniformVec4(OdLoc, material.diffuse);
  setUniformVec4(OsLoc, material.spot);
  setUniform(nsLoc, material.shine);
}

inline void
GLMeshRendererBase::GLProgram::setViewportMatrix(const mat4f& m)
{
  setUniformMat4(viewportMatrixLoc, m);
}

inline void
GLMeshRendererBase::GLProgram::setProjectionType(const Camera& camera)
{
  setUniform(projectionTypeLoc, camera.projectionType());
}

namespace
{ // begin namespace

inline mat4f
mvpMatrix(const mat4f& mvm, const Camera& c)
{
  return c.projectionMatrix() * mvm;
}

inline mat4f
mvMatrix(const mat4f& t, const Camera& c)
{
  return c.worldToCameraMatrix() * t;
}

inline auto
normalMatrix(const mat3f& n, const Camera& c)
{
  return mat3f{c.worldToCameraMatrix()} * n;
}

} // end namesspace

inline void
GLMeshRendererBase::GLProgram::setTransforms(const mat4f& t,
  const mat3f& n,
  const Camera& camera)
{
  auto mv = mvMatrix(t, camera);

  setUniformMat4(mvMatrixLoc, mv);
  setUniformMat4(mvpMatrixLoc, mvpMatrix(mv, camera));
  setUniformMat3(normalMatrixLoc, normalMatrix(n, camera));
}

namespace
{ // begin namespace

inline auto
lightPosition(const Light& light, const Camera& camera)
{
  return light.flags.isSet(Light::Camera) ?
    light.position() : camera.worldToCamera(light.position());
}

inline auto
lightDirection(const Light& light, const Camera& camera)
{
  return light.flags.isSet(Light::Camera) ?
    light.direction() : camera.worldToCameraVector(light.direction());
}

} // end namespace

bool
GLMeshRendererBase::GLProgram::setLight(int index,
  const Light& light,
  const Camera& camera)
{
  if (!light.isTurnedOn())
    return false;

  auto loc = lightLocs + index;

  setUniform(loc->type, (int)light.type());
  setUniformVec4(loc->color, light.color);
  setUniformVec3(loc->position, lightPosition(light, camera));
  setUniformVec3(loc->direction, lightDirection(light, camera));
  setUniform(loc->falloff, (int)light.falloff);
  setUniform(loc->range, light.range());
  setUniform(loc->angle, light.spotAngle());
  return true;
}

inline void
GLMeshRendererBase::GLProgram::setDefaultLights()
{
  setUniform(lightLocs->type, 1); // point light
  setUniformVec4(lightLocs->color, vec4f{1, 1, 1, 0});
  setUniformVec3(lightLocs->position, vec3f{0});
  setUniform(lightLocs->range, 0.0f);
  setUniform(lightCountLoc, 1);
}

void
GLMeshRendererBase::GLProgram::endLights(int count)
{
  count ? setUniform(lightCountLoc, count) : setDefaultLights();
}

inline void
GLMeshRendererBase::GLProgram::setHiddenLinesFlag(bool flag)
{
  setUniform(hiddenLinesFlagLoc, (int)flag);
}

inline void
GLMeshRendererBase::GLProgram::setUseTexture(bool flag)
{
  setUniform(useTextureLoc, (int)flag);
}

inline void
GLMeshRendererBase::GLProgram::setUseVertexColors(bool flag)
{
  setUniform(useVertexColorsLoc, (int)flag);
}


/////////////////////////////////////////////////////////////////////
//
// GLMeshRendererBase implementation
// ==================
GLMeshRendererBase::~GLMeshRendererBase()
{
  delete _program;
}

void
GLMeshRendererBase::setLineColor(const Color& color)
{
  _program->assertInUse();
  _program->setLineColor(color);
  GLGraphics3::setLineColor(color);
}

void
GLMeshRendererBase::setAmbientLight(const Color& color)
{
  _program->assertInUse();
  _program->setAmbientLight(color);
}

void
GLMeshRendererBase::setMaterial(const Material& material, void* texture)
{
  _program->assertInUse();
  _program->setMaterial(material);
  _texture = texture ? (GLuint)(intptr_t)texture : 0;
}

void
GLMeshRendererBase::end()
{
  if (auto cp = GLSL::Program::current(); _program == cp)
  {
    GLSL::Program::setCurrent(_lastState.program);
    glBindVertexArray(_lastState.vao);
    glBindTexture(GL_TEXTURE_2D, _lastState.texture);
    if (!_lastState.depthTest)
      glDisable(GL_DEPTH_TEST);
  }
}

GLMeshRendererBase::GLMeshRendererBase():
  _program{new GLProgram{*this}}
{
  // do nothing
}

void
GLMeshRendererBase::updateView(Camera& camera)
{
  GLGraphics3::updateView(camera);

  GLint v[4];

  glGetIntegerv(GL_VIEWPORT, v);

  float w = v[2] / 2.0f;
  float h = v[3] / 2.0f;

  _viewportMatrix[0].set(w, 0, 0, 0);
  _viewportMatrix[1].set(0, h, 0, 0);
  _viewportMatrix[2].set(0, 0, 1, 0);
  _viewportMatrix[3].set(v[0] + w, v[1] + h, 0, 0);
}

void
GLMeshRendererBase::begin(Camera& camera)
{
  if (auto cp = GLSL::Program::current(); _program != cp)
  {
    _lastState.program = cp;
    _lastState.depthTest = glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &_lastState.texture);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
    glPolygonMode(GL_FRONT_AND_BACK, (renderMode != Wireframe) + GL_LINE);
    updateView(camera);
    _program->use();
    _program->setProjectionType(camera);
    _program->setViewportMatrix(_viewportMatrix);
  }
}

bool
GLMeshRendererBase::render(const TriangleMesh& mesh,
  int count,
  int index,
  const mat4f& t,
  const mat3f& n,
  const Camera& camera)
{
  _program->assertInUse();

  auto m = glMesh(&mesh);

  if (!m)
    return false;
  m->bind();
  _program->setTransforms(t, n, camera);
  _program->setHiddenLinesFlag(!(renderMode != HiddenLines));

  const auto uvcFlag = useVertexColors();
  auto utFlag = false;

  if (!uvcFlag && (utFlag = mesh.hasUV() && _texture))
    glBindTexture(GL_TEXTURE_2D, _texture);
  _program->setUseTexture(utFlag);
  _program->setUseVertexColors(uvcFlag);
  // For efficiency reasons, validation of 'count' and 'index'
  // parameters against valid mesh triangles should be handled
  // by derived class methods.
  glDrawElements(GL_TRIANGLES,
    count * 3,
    GL_UNSIGNED_INT,
    (void*)(sizeof(TriangleMesh::Triangle) * index));
  return true;
}

bool
GLMeshRendererBase::render(const TriangleMesh& mesh,
  const mat4f& t,
  const mat3f& n,
  const Camera& camera)
{
  if (!render(mesh, mesh.triangleCount(), 0, t, n, camera))
    return false;
  if (flags.isSet(DrawNormals))
  {
    setVectorColor(normalColor);
    drawNormals(mesh, t, n, _normalScale);
  }
  return true;
}


} // end namespace cg
