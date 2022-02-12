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
// OVERVIEW: RayTracer.h
// ========
// Class definition for simple ray tracer.
//
// Author: Paulo Pagliosa
// Last revision: 07/02/2022

#ifndef __RayTracer_h
#define __RayTracer_h

#include "geometry/Intersection.h"
#include "graphics/Image.h"
#include "graphics/PrimitiveBVH.h"
#include "graphics/Renderer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// RayTracer: simple ray tracer class
// =========
class RayTracer: public Renderer
{
public:
  static constexpr auto minMinWeight = float(0.001);
  static constexpr auto maxMaxRecursionLevel = uint32_t(20);

  RayTracer(SceneBase&, Camera&);

  auto minWeight() const
  {
    return _minWeight;
  }

  void setMinWeight(float w)
  {
    _minWeight = math::max(w, minMinWeight);
  }

  auto maxRecursionLevel() const
  {
    return _maxRecursionLevel;
  }

  void setMaxRecursionLevel(uint32_t rl)
  {
    _maxRecursionLevel = math::min(rl, maxMaxRecursionLevel);
  }

  void update() override;
  void render() override;
  virtual void renderImage(Image&);

private:
  Reference<PrimitiveBVH> _bvh;
  struct VRC
  {
    vec3f u;
    vec3f v;
    vec3f n;

  } _vrc;
  float _minWeight;
  uint32_t _maxRecursionLevel;
  uint64_t _numberOfRays;
  uint64_t _numberOfHits;
  Ray3f _pixelRay;
  float _Vh;
  float _Vw;
  float _Ih;
  float _Iw;

  void scan(Image& image);
  void setPixelRay(float x, float y);
  Color shoot(float x, float y);
  bool intersect(const Ray3f&, Intersection&);
  Color trace(const Ray3f& ray, uint32_t level, float weight);
  Color shade(const Ray3f&, Intersection&, uint32_t, float);
  bool shadow(const Ray3f&);
  Color background() const;

  vec3f imageToWindow(float x, float y) const
  {
    return _Vw * (x * _Iw - 0.5f) * _vrc.u + _Vh * (y * _Ih - 0.5f) * _vrc.v;
  }

}; // RayTracer

} // end namespace cg

#endif // __RayTracer_h
