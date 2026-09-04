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
// OVERVIEW: Renderer.h
// ========
// Class definition for generic renderer.
//
// Author: Paulo Pagliosa
// Last revision: 04/09/2026

#ifndef __Renderer_h
#define __Renderer_h

#include "graphics/CameraHolder.h"
#include "graphics/SceneBase.h"
#include <utility>

namespace cg
{ // begin namespace cg

struct Viewport
{
  int x;
  int y;
  int w;
  int h;

}; // Viewport


/////////////////////////////////////////////////////////////////////
//
// Renderer: generic renderer class
// ========
class Renderer: public CameraHolder, public virtual SharedObject
{
public:
  // Constructors
  Renderer() = default;

  Renderer(SceneBase& scene, Camera* camera):
    CameraHolder{camera},
    _scene{&scene}
  {
    // do nothing
  }

  Renderer(const Renderer& other):
    Renderer{*other.scene(), other.camera()}
  {
    // do nothing
  }

  [[nodiscard]] SceneBase* scene() const
  {
    return _scene;
  }

  void setScene(SceneBase& scene)
  {
    if (&scene != _scene.get())
      _scene = &scene;
  }

  void imageSize(int& w, int& h) const
  {
    w = _viewport.w;
    h = _viewport.h;
  }

  [[nodiscard]] auto imageSize() const
  {
    return std::pair{_viewport.w, _viewport.h};
  }

  void setImageSize(int, int);

  [[nodiscard]] vec3f project(const vec3f&) const;
  [[nodiscard]] vec3f unproject(const vec3f&) const;

  virtual void render() = 0;

protected:
  Reference<SceneBase> _scene;
  Viewport _viewport{0, 0, 1, 1};

}; // Renderer

[[nodiscard]] constexpr auto
normalize(const vec4f& p)
{
  return vec3f{p} * math::inverse(p.w);
}

} // end namespace cg

#endif // __Renderer_h
