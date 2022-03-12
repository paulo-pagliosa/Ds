//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: Primitive.cpp
// ========
// Source file for primitive.
//
// Author: Paulo Pagliosa
// Last revision: 10/03/2022

#include "graphics/Primitive.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Primitive implementation
// =========
const TriangleMesh*
Primitive::tesselate() const
{
  return nullptr;
}

bool
Primitive::canIntersect() const
{
  return true;
}

bool
Primitive::localIntersect(const Ray3f&) const
{
  throw bad_invocation("Primitive", __func__);
}

inline auto
transform(const Ray3f& ray, const mat4f& m)
{
  Ray3f r;

  r.origin = m.transform3x4(ray.origin);
  r.direction = m.transformVector(ray.direction);

  auto d = r.direction.length();

  r.tMin = ray.tMin * d;
  r.tMax = ray.tMax * d;
  r.direction *= (d = 1 / d);
  return std::pair{r, d};
}

bool
Primitive::intersect(const Ray3f& ray) const
{
  assert(canIntersect());
  auto [localRay, d] = transform(ray, _worldToLocal);
  return localIntersect(localRay);
}

bool
Primitive::intersect(const Ray3f& ray, Intersection& hit) const
{
  assert(canIntersect());
  auto [localRay, d] = transform(ray, _worldToLocal);
  if (!localIntersect(localRay, hit))
    return false;
  hit.distance *= d;
  return true;
}

bool
Primitive::localIntersect(const Ray3f&, Intersection&) const
{
  throw bad_invocation("Primitive", __func__);
}

Material*
Primitive::material() const
{
  return _material;
}

void
Primitive::setMaterial(Material* m)
{
  _material = m == nullptr ? Material::defaultMaterial() : m;
}

void
Primitive::setTransform(const mat4f& l2w, const mat4f& w2l)
{
  TransformableObject::setTransform(l2w, w2l);
  _normalMatrix = mat3f{w2l}.transposed();
}

void
Primitive::setTransform(const vec3f& p, const quatf& q, const vec3f& s)
{
  TransformableObject::setTransform(p, q, s);
  _normalMatrix = mat3f{_worldToLocal}.transposed();
}


/////////////////////////////////////////////////////////////////////
//
// Aggregate implementation
// =========
vec3f
Aggregate::normal(const Intersection&) const
{
  throw bad_invocation("Aggregate", __func__);
}

Material*
Aggregate::material() const
{
  throw bad_invocation("Aggregate", __func__);
}


/////////////////////////////////////////////////////////////////////
//
// ShapeInstance implementation
// =============
const TriangleMesh*
ShapeInstance::tesselate() const
{
  return _shape->tesselate();
}

bool
ShapeInstance::canIntersect() const
{
  return _shape->canIntersect();
}

bool
ShapeInstance::localIntersect(const Ray3f& ray) const
{
  return _shape->intersect(ray);
}

bool
ShapeInstance::localIntersect(const Ray3f& ray, Intersection& hit) const
{
  return _shape->intersect(ray, hit) ? void(hit.object = this), true : false;
}

vec3f
ShapeInstance::normal(const Intersection& hit) const
{
  return _normalMatrix.transform(_shape->normal(hit)).versor();
}

Bounds3f
ShapeInstance::bounds() const
{
  return {_shape->bounds(), _localToWorld};
}

} // end namespace cg
