//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// Last revision: 31/09/2026

#include "graphics/GLRenderer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
GLRenderer::GLRenderer(SceneBase& scene, Camera& camera):
  Renderer{scene, &camera}
{
  // do nothing
}

void
GLRenderer::updateView(Camera& camera)
{
  GLMeshRendererBase::updateView(camera);
  _windowViewportRatio = camera.windowHeight() / _viewport.h;
}

void
GLRenderer::setBasePoint(const vec3f& p)
{
  _basePointZ = -camera()->worldToCamera(_basePoint = p).z;
}

float
GLRenderer::pixelsLength(float d) const
{
  const auto camera = this->camera();

  if (camera->projectionType() == Camera::Perspective)
    d *= _basePointZ / camera->nearPlane();
  return _windowViewportRatio * d;
}

void
GLRenderer::renderLights()
{
  const auto& lights = _scene->lights();

  setLights(_scene->lights().cbegin(), lights.cend(), *camera());
  setAmbientLight(_scene->ambientLight);
}

void
GLRenderer::renderActors()
{
  for (auto actor : _scene->actors())
  {
    if (!actor->isVisible())
      continue;

    auto mapper = actor->mapper();

    assert(mapper);
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
  begin(*camera());
}

void
GLRenderer::render()
{
  beginRender();
  renderLights();
  renderActors();
  endRender();
}

void
GLRenderer::endRender()
{
  if (_renderFunction)
    _renderFunction(*this);
  glFlush();
  end();
}

bool
GLRenderer::drawMesh(const Primitive& primitive)
{
  auto mesh = primitive.tesselate();

  if (!mesh)
    return false;
  setMaterial(*primitive.material());
  if (!GLMeshRendererBase::render(*mesh,
    primitive.localToWorldMatrix(),
    primitive.normalMatrix(),
    *camera()))
    return false;
  if (flags.isSet(DrawBounds))
  {
    setLineColor(boundsColor);
    drawBounds(primitive.bounds());
  }
  return true;
}

bool
GLRenderer::drawSubMesh(const TriangleMesh& mesh,
  int count,
  int index,
  const Material& material,
  const mat4f& t,
  const mat3f& n)
{
  if (count <= 0 || index < 0)
    return false;
  if (const auto nt = mesh.triangleCount(); index >= nt)
    return false;
  else if (auto end = index + count; end > nt)
    count = end - nt;
  setMaterial(material);
  return GLMeshRendererBase::render(mesh, count, index, t, n, *camera());
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
