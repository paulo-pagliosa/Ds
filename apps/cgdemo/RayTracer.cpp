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
// OVERVIEW: RayTracer.cpp
// ========
// Source file for simple ray tracer.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#include "graphics/Camera.h"
#include "utils/Stopwatch.h"
#include "RayTracer.h"
#include <iostream>

using namespace std;

namespace cg
{ // begin namespace cg

namespace
{ // begin namespace

inline void
printElapsedTime(const char* s, Stopwatch::ms_time time)
{
  printf("%sElapsed time: %g ms\n", s, time);
}

} // end namespace


/////////////////////////////////////////////////////////////////////
//
// RayTracer implementation
// =========
RayTracer::RayTracer(SceneBase& scene, Camera& camera):
  Renderer{scene, camera},
  _maxRecursionLevel{6},
  _minWeight{minMinWeight}
{
  // do nothing
}

void
RayTracer::update()
{
  // Delete current BVH before creating a new one
  _bvh = nullptr;

  PrimitiveBVH::PrimitiveArray primitives;
  auto np = uint32_t(0);

  primitives.reserve(_scene->actorCount());
  for (auto actor : _scene->actors())
    if (actor->visible)
    {
      auto p = actor->mapper()->primitive();

      assert(p != nullptr);
      if (p->canIntersect())
      {
        primitives.push_back(p);
        np++;
      }
    }
  _bvh = new PrimitiveBVH{std::move(primitives)};
}

void
RayTracer::render()
{
  throw std::runtime_error("RayTracer::render() invoked");
}

void
RayTracer::renderImage(Image& image)
{
  Stopwatch timer;

  update();
  timer.start();
  {
    const auto& m = _camera->cameraToWorldMatrix();

    // VRC axes
    _vrc.u = m[0];
    _vrc.v = m[1];
    _vrc.n = m[2];
  }

  // init auxiliary mapping variables
  auto w = image.width(), h = image.height();

  setImageSize(w, h);
  _Iw = math::inverse(float(w));
  _Ih = math::inverse(float(h));
  {
    auto wh = _camera->windowHeight();

    if (w >= h)
      _Vw = (_Vh = wh) * w * _Ih;
    else
      _Vh = (_Vw = wh) * h * _Iw;
  }

  // init pixel ray
  float F, B;

  _camera->clippingPlanes(F, B);
  if (_camera->projectionType() == Camera::Perspective)
  {
    // distance from the camera position to a frustum back corner
    auto z = B / F * 0.5f;
    B = vec3f{_Vw * z, _Vh * z, B}.length();
  }
  _pixelRay.tMin = F;
  _pixelRay.tMax = B;
  _pixelRay.set(_camera->position(), -_vrc.n);
  _numberOfRays = _numberOfHits = 0;
  scan(image);

  auto et = timer.time();

  std::cout << "\nNumber of rays: " << _numberOfRays;
  std::cout << "\nNumber of hits: " << _numberOfHits;
  printElapsedTime("\nDONE! ", et);
}

void
RayTracer::setPixelRay(float x, float y)
//[]---------------------------------------------------[]
//|  Set pixel ray                                      |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//[]---------------------------------------------------[]
{
  auto p = imageToWindow(x, y);

  switch (_camera->projectionType())
  {
    case Camera::Perspective:
      _pixelRay.direction = (p - _camera->nearPlane() * _vrc.n).versor();
      break;

    case Camera::Parallel:
      _pixelRay.origin = _camera->position() + p;
      break;
  }
}

void
RayTracer::scan(Image& image)
{
  ImageBuffer scanLine{_viewport.w, 1};

  for (auto j = 0; j < _viewport.h; j++)
  {
    auto y = (float)j + 0.5f;

    printf("Scanning line %d of %d\r", j + 1, _viewport.h);
    for (auto i = 0; i < _viewport.w; i++)
      scanLine[i] = shoot((float)i + 0.5f, y);
    image.setData(0, j, scanLine);
  }
}

Color
RayTracer::shoot(float x, float y)
//[]---------------------------------------------------[]
//|  Shoot a pixel ray                                  |
//|  @param x coordinate of the pixel                   |
//|  @param y cordinates of the pixel                   |
//|  @return RGB color of the pixel                     |
//[]---------------------------------------------------[]
{
  // set pixel ray
  setPixelRay(x, y);

  // trace pixel ray
  Color color = trace(_pixelRay, 0, 1);

  // adjust RGB color
  if (color.r > 1.0f)
    color.r = 1.0f;
  if (color.g > 1.0f)
    color.g = 1.0f;
  if (color.b > 1.0f)
    color.b = 1.0f;
  // return pixel color
  return color;
}

Color
RayTracer::trace(const Ray3f& ray, uint32_t level, float weight)
//[]---------------------------------------------------[]
//|  Trace a ray                                        |
//|  @param the ray                                     |
//|  @param recursion level                             |
//|  @param ray weight                                  |
//|  @return color of the ray                           |
//[]---------------------------------------------------[]
{
  if (level > _maxRecursionLevel)
    return Color::black;
  ++_numberOfRays;

  Intersection hit;

  return intersect(ray, hit) ? shade(ray, hit, level, weight) : background();
}

inline constexpr auto
rt_eps()
{
  return 1e-4f;
}

bool
RayTracer::intersect(const Ray3f& ray, Intersection& hit)
//[]---------------------------------------------------[]
//|  Ray/object intersection                            |
//|  @param the ray (input)                             |
//|  @param information on intersection (output)        |
//|  @return true if the ray intersects an object       |
//[]---------------------------------------------------[]
{
  hit.object = nullptr;
  hit.distance = ray.tMax;
  return _bvh->intersect(ray, hit) ? ++_numberOfHits : false;
}

inline auto
maxRGB(const Color& c)
{
  return math::max(math::max(c.r, c.g), c.b);
}

Color
RayTracer::shade(const Ray3f& ray,
  Intersection& hit,
  uint32_t level,
  float weight)
//[]---------------------------------------------------[]
//|  Shade a point P                                    |
//|  @param the ray (input)                             |
//|  @param information on intersection (input)         |
//|  @param recursion level                             |
//|  @param ray weight                                  |
//|  @return color at point P                           |
//[]---------------------------------------------------[]
{
  auto primitive = (Primitive*)hit.object;

  assert(nullptr != primitive);

  auto N = primitive->normal(hit);
  const auto& V = ray.direction;
  auto NV = N.dot(V);

  // Make sure "real" normal is on right side
  if (NV > 0)
    N.negate(), NV = -NV;

  auto R = V - (2 * NV) * N; // reflection vector
  // Start with ambient lighting
  auto m = primitive->material();
  auto color = _scene->ambientLight * m->ambient;
  auto P = ray(hit.distance);

  // Compute direct lighting
  for (auto light : _scene->lights())
  {
    // If the light is turned off, then continue
    if (!light->isTurnedOn())
      continue;

    vec3f L;
    float d;

    // If the point P is out of the light range (for finite
    // point light or spotlight), then continue
    if (!light->lightVector(P, L, d))
      continue;

    auto NL = N.dot(L);

    // If light vector is backfaced, then continue
    if (NL <= 0)
      continue;

    auto lightRay = Ray3f{P + L * rt_eps(), L};

    lightRay.tMax = d;
    ++_numberOfRays;
    // If the point P is shadowed, then continue
    if (shadow(lightRay))
      continue;

    auto lc = light->lightColor(d);

    color += lc * m->diffuse * NL;
    if (m->shine <= 0 || (d = R.dot(L)) <= 0)
      continue;
    color += lc * m->spot * pow(d, m->shine);
  }
  // Compute specular reflection
  if (m->specular != Color::black)
  {
    weight *= maxRGB(m->specular);
    if (weight > _minWeight && level < _maxRecursionLevel)
    {
      auto reflectionRay = Ray3f{P + R * rt_eps(), R};
      color += m->specular * trace(reflectionRay, level + 1, weight);
    }
  }
  return color;
}

Color
RayTracer::background() const
//[]---------------------------------------------------[]
//|  Background                                         |
//|  @return background color                           |
//[]---------------------------------------------------[]
{
  return _scene->backgroundColor;
}

bool
RayTracer::shadow(const Ray3f& ray)
//[]---------------------------------------------------[]
//|  Verifiy if ray is a shadow ray                     |
//|  @param the ray (input)                             |
//|  @return true if the ray intersects an object       |
//[]---------------------------------------------------[]
{
  return _bvh->intersect(ray) ? ++_numberOfHits : false;
}

} // end namespace cg
