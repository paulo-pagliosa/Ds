//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2025 Paulo Pagliosa.                        |
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
// Last revision: 02/12/2025

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
  static constexpr auto minMinWeight = 0.001f;
  static constexpr auto maxMaxRecursionLevel = 20u;
  static constexpr auto maxPixelSubdivisionLevel = 2u;

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
  static constexpr auto maxSteps = 1 << maxPixelSubdivisionLevel;

  struct PixelSample
  {
    Pixel color;
    bool cooked{};

  }; // PixelSample

  using SampleWindow = PixelSample[maxSteps + 1][maxSteps + 1];

  Reference<PrimitiveBVH> _bvh;
  struct VRC
  {
    vec3f u;
    vec3f v;
    vec3f n;

  } _vrc;
  Ray3f _pixelRay;
  uint64_t _numberOfRays;
  uint64_t _numberOfHits;
  uint32_t _maxRecursionLevel;
  float _minWeight;
  float _Vh;
  float _Vw;
  float _Ih;
  float _Iw;
  uint8_t _adaptivityDistance;

  void setPixelRay(float, float);
  void scan(Image&);
  void adaptiveScan(Image&);
  Pixel adapt(SampleWindow&, int, int, float, float, int);
  Color shoot(float, float);
  Color trace(const Ray3f&, uint32_t, float);
  Color shade(const Ray3f&, Intersection&, uint32_t, float);
  Color background() const;
  bool intersect(const Ray3f&, Intersection&);
  bool shadow(const Ray3f&);

  vec3f imageToWindow(float x, float y) const
  {
    return _Vw * (x * _Iw - 0.5f) * _vrc.u + _Vh * (y * _Ih - 0.5f) * _vrc.v;
  }

}; // RayTracer

} // end namespace cg

#endif // __RayTracer_h
