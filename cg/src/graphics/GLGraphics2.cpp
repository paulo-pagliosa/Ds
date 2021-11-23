//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2019 Orthrus Group.                         |
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
// OVERVIEW: GLGraphics2.cpp
// ========
// Source file for OpenGL 2D graphics.
//
// Author: Paulo Pagliosa
// Last revision: 16/02/2019

#include "graphics/GLGraphics2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// View2 implementation
// =====
View2::View2():
  _bounds{{-1, -1}, {1, 1}},
  _delta{0, 0}
{
  _aspectRatio = _scale = _sx = _sy = 1;
  _tx = _ty = 0;
  _modified = false;
}

void
View2::setBounds(const Bounds2f& bounds)
{
  if (bounds.empty())
   return;
  _bounds = bounds;
  _modified = true;
}

void
View2::setAspectRatio(float aspectRatio)
{
  if (aspectRatio <= 0)
    return;
  _aspectRatio = aspectRatio;
  _modified = true;
}

void
View2::zoom(float scale)
{
  if (scale <= 0)
    return;
  _scale *= scale;
  _modified = true;
}

void
View2::pan(float x, float y)
{
  if (math::isZero(x) && math::isZero(y))
    return;
  _delta.x += x / _scale;
  _delta.y += y / _scale;
  _modified = true;
}

void
View2::updateView()
{
  if (!_modified)
    return;

  const auto& p1 = _bounds.min();
  const auto& p2 = _bounds.max();
  auto iw = 1 / (p2.x - p1.x) * _scale;
  auto ih = 1 / (p2.y - p1.y) * _scale;

  _aspectRatio > ih / iw ? iw = ih / _aspectRatio : ih = iw * _aspectRatio;
  _sx = 2 * iw;
  _sy = 2 * ih;
  _tx = -(_delta.x + p1.x + p2.x) * iw;
  _ty = -(_delta.y + p1.y + p2.y) * ih;
  _modified = false;
}

void
View2::setDefaultView()
{
  _aspectRatio = _scale = _sx = _sy = 1;
  _delta.x = _delta.y = _tx = _ty = 0;
  _modified = true;
}

inline void
View2::toNDC(const vec2f& p, vec4f& v) const
{
  v[0] = _sx * p.x + _tx;
  v[1] = _sy * p.y + _ty;
  v[2] = 0;
  v[3] = 1;
}


/////////////////////////////////////////////////////////////////////
//
// GLGraphics2 implementation
// ===========
void
GLGraphics2::drawPoint(const vec2f& p)
{
  vec4f v;

  toNDC(p, v);
  Base::drawPoint(&v);
}

void
GLGraphics2::drawLine(const vec2f& p1, const vec2f& p2)
{
  vec4f v[2];

  toNDC(p1, v[0]);
  toNDC(p2, v[1]);
  Base::drawLine(v);
}

void
GLGraphics2::drawTriangle(const vec2f& p1, const vec2f& p2, const vec2f& p3)
{
  vec4f v[3];

  toNDC(p1, v[0]);
  toNDC(p2, v[1]);
  toNDC(p3, v[2]);
  Base::drawTriangle(v);
}

void
GLGraphics2::drawCircumference(const vec2f& center, float radius)
{
  if (radius <= 0)
    return;

  const int ns{20};
  float c{cos(2 * math::pi<float>() / ns)};
  float s{sin(2 * math::pi<float>() / ns)};
  vec2f r{0, radius};
  vec4f p[2];

  toNDC(center + r, p[0]);
  for (int i = 0; i < ns; i++)
  {
    auto t = r;

    r.x = c * t.x - s * t.y;
    r.y = s * t.x + c * t.y;
    toNDC(center + r, p[1]);
    Base::drawLine(p);
    p[0] = p[1];
  }
}

void
GLGraphics2::drawBounds(const Bounds2f& bounds)
{
  const auto& p1 = bounds.min();
  const auto& p3 = bounds.max();
  vec2f p2{p3.x, p1.y};
  vec2f p4{p1.x, p3.y};

  if (polygonMode() == PolygonMode::LINE)
  {
    drawLine(p1, p2);
    drawLine(p2, p3);
    drawLine(p3, p4);
    drawLine(p4, p1);
  }
  else
  {
    drawTriangle(p1, p2, p3);
    drawTriangle(p3, p4, p1);
  }
}

vec2f
GLGraphics2::screenToWorld(const vec2f& p) const
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  auto x = ((p.x / viewport[2] * 2 - 1) - _tx) / _sx;
  auto y = ((p.y / viewport[3] * 2 - 1) - _ty) / _sy;

  return vec2f{x, y};
}

} // end namespace cg
