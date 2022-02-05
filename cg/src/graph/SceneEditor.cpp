//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Orthrus Group.                         |
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
// OVERVIEW: SceneEditor.cpp
// ========
// Source file for scene editor.
//
// Author: Paulo Pagliosa
// Last revision: 05/02/2022

#include "graph/SceneEditor.h"

namespace cg
{ // begin namespace cg

namespace graph
{ // begin namespace graph


/////////////////////////////////////////////////////////////////////
//
// SceneEditor impementation
// ===========
void
SceneEditor::setDefaultView(float aspect)
{
  _camera->setDefaultView(aspect);
}

void
SceneEditor::zoom(float s)
{
  _camera->zoom(s);
}

void
SceneEditor::rotateView(float ax, float ay)
{
  _camera->rotateYX(ay, ax);
}

void
SceneEditor::orbit(float ax, float ay)
{
  _camera->rotateYX(ay, ax, true);
}

void
SceneEditor::pan(const vec3f& d)
{
  _camera->translate(d);
}

void
SceneEditor::newFrame()
{
  const auto& bc = _scene->backgroundColor;

  glClearColor(bc.r, bc.g, bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
SceneEditor::drawCamera(const CameraProxy& proxy)
{
  auto& camera = *proxy.camera();
  float F, B;

  camera.clippingPlanes(F, B);
  B = math::min(F + 10.0f, B);

  auto h2 = camera.windowHeight() / 2;
  auto w2 = h2 * camera.aspectRatio();
  // front (projection) plane
  auto p1 = camera.cameraToWorld({-w2, -h2, -F});
  auto p2 = camera.cameraToWorld({+w2, -h2, -F});
  auto p3 = camera.cameraToWorld({+w2, +h2, -F});
  auto p4 = camera.cameraToWorld({-w2, +h2, -F});

  if (camera.projectionType() == Camera::Perspective)
  {
    const auto z = B / F;

    h2 *= z;
    w2 *= z;
  }

  // back plane
  auto p5 = camera.cameraToWorld({-w2, -h2, -B});
  auto p6 = camera.cameraToWorld({+w2, -h2, -B});
  auto p7 = camera.cameraToWorld({+w2, +h2, -B});
  auto p8 = camera.cameraToWorld({-w2, +h2, -B});

  setPolygonMode(LINE);
  setQuadColor(_cameraLineColor);
  drawQuad(p1, p2, p3, p4);
  drawQuad(p6, p5, p8, p7);
  drawQuad(p5, p1, p4, p8);
  drawQuad(p2, p6, p7, p3);
}

inline void
SceneEditor::outlineSphere(const vec3f& p, const mat3f& m, float r)
{
  drawCircle(p, r, m[0]);
  drawCircle(p, r, m[1]);
  drawCircle(p, r, m[2]);
}

inline void
SceneEditor::outlineCone(const vec3f& p, const mat3f& m, float a, float h)
{
  auto& n = m[2];
  auto bc = p + n * h;
  auto br = h * float(sin(math::toRadians(a) * 0.5));

  drawCircle(bc, br, n);

  auto ur = m[0] * br;
  auto vr = m[1] * br;

  drawLine(p, bc + ur);
  drawLine(p, bc - ur);
  drawLine(p, bc + vr);
  drawLine(p, bc - vr);
}

inline void
SceneEditor::outlineCylinder(const vec3f& p, const mat3f& m, float r, float h)
{
  auto& n = m[2];
  auto ur = m[0] * r;
  auto vr = m[1] * r;

  drawVector(p + ur, n, h);
  drawVector(p - ur, n, h);
  drawVector(p + vr, n, h);
  drawVector(p - vr, n, h);
}

void
SceneEditor::drawLight(const LightProxy& proxy)
{
  constexpr auto lr = 10;
  constexpr auto dl = 120;

  const auto& t = *proxy.transform();
  auto r = mat3f{t.rotation()};
  const auto& p = t.position();
  const auto& light = *proxy.light();

  setBasePoint(p);

  auto dt = glIsEnabled(GL_DEPTH_TEST);

  glDisable(GL_DEPTH_TEST);
  setMeshColor(_lightGismoColor);
  setPolygonMode(PolygonMode::FILL);
  setFlatMode(true);
  drawCircle(p, pixelsLength(lr), _camera->viewPlaneNormal());
  setFlatMode(false);
  setLineColor(_lightGismoColor);
  setPolygonMode(PolygonMode::LINE);
  if (auto type = light.type(); type == Light::Type::Directional)
    outlineCylinder(p, r, pixelsLength(lr * 2), pixelsLength(dl));
  else
  {
    float range = 20.0f;

    if (!light.flags.isSet(Light::Infinite))
      range = math::min(light.range(), range);
    if (type == Light::Type::Point)
      outlineSphere(p, r, range);
    else
      outlineCone(p, r, light.spotAngle(), range);
  }
  if (dt)
    glEnable(GL_DEPTH_TEST);
}

void
SceneEditor::drawTransform(const Transform& t)
{
  constexpr auto al = 120;

  auto r = mat3f{t.rotation()};
  const auto& p = t.position();

  setBasePoint(p);
  drawAxes(p, r, pixelsLength(al));
}

} // end namespace graph

} // end namespace cg
