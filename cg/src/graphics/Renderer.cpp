//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Renderer.cpp
// ========
// Source file for generic renderer.
//
// Author: Paulo Pagliosa
// Last revision: 10/02/2022

#include "graphics/Renderer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Renderer implementation
// ========
Renderer::Renderer(SceneBase& scene, Camera& camera):
  _scene{&scene},
  _camera{&camera}
{
  // do nothing
}

void
Renderer::setScene(SceneBase& scene)
{
  if (&scene != _scene)
    _scene = &scene;
}

void
Renderer::setCamera(Camera& camera)
{
  if (&camera != _camera)
    _camera = &camera;
}

void
Renderer::setImageSize(int w, int h)
{
  _viewport.w = w;
  _viewport.h = h;
  _camera->setAspectRatio((float)(w) / (float)(h));
}

void
Renderer::update()
{
  // do nothing
}

vec3f
Renderer::project(const vec3f& p) const
{
  // TODO: consider viewport origin
  auto w = normalize(vpMatrix(_camera) * vec4f{p, 1});

  w.x = (w.x * 0.5f + 0.5f) * _viewport.w;
  w.y = (w.y * 0.5f + 0.5f) * _viewport.h;
  w.z = (w.z * 0.5f + 0.5f);
  return w;
}

vec3f
Renderer::unproject(const vec3f& w) const
{
  // TODO: consider viewport origin
  vec3f p{w.x / _viewport.w * 2 - 1, w.y / _viewport.h * 2 - 1, w.z * 2 - 1};
  mat4f m{vpMatrix(_camera)};

  m.invert();
  return normalize(m * vec4f{p, 1});
}

} // end namespace cg
