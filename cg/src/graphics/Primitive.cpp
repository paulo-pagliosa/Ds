//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Orthrus Group.                               |
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
// Last revision: 19/01/2022

#include "graphics/Primitive.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Primitive implementation
// =========
const TriangleMesh*
Primitive::mesh() const
{
  return nullptr;
}

bool
Primitive::canIntersect() const
{
  return true;
}

PrimitiveArray
Primitive::refine() const
{
  throw bad_invocation("Primitive", __func__);
}


/////////////////////////////////////////////////////////////////////
//
// Aggregate implementation
// =========
vec3f
Aggregate::normal(const Intersection&) const
{
  throw bad_invocation("Primitive", __func__);
}

const Material*
Aggregate::material() const
{
  throw bad_invocation("Primitive", __func__);
}


/////////////////////////////////////////////////////////////////////
//
// Primitive implementation
// =========
const Material*
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
Primitive::setTransform(const vec3f& p, const quatf& q, const vec3f& s)
//[]---------------------------------------------------[]
//|  Set transform                                      |
//[]---------------------------------------------------[]
{
  mat3f r{q};

  _localToWorld[0].set(r[0] * s[0]);
  _localToWorld[1].set(r[1] * s[1]);
  _localToWorld[2].set(r[2] * s[2]);
  _localToWorld[3].set(p, 1);
  r[0] *= math::inverse(s[0]);
  r[1] *= math::inverse(s[1]);
  r[2] *= math::inverse(s[2]);
  _normalMatrix.set(r);
  _worldToLocal[0].set(r[0][0], r[1][0], r[2][0]);
  _worldToLocal[1].set(r[0][1], r[1][1], r[2][1]);
  _worldToLocal[2].set(r[0][2], r[1][2], r[2][2]);
  _worldToLocal[3][0] = -(r[0].dot(p));
  _worldToLocal[3][1] = -(r[1].dot(p));
  _worldToLocal[3][2] = -(r[2].dot(p));
  _worldToLocal[3][3] = 1;
}


/////////////////////////////////////////////////////////////////////
//
// PrimitiveInstance implementation
// =================
const TriangleMesh*
PrimitiveInstance::mesh() const
{
  return _shape->mesh();
}

bool
PrimitiveInstance::intersect(const Ray3f& ray, Intersection& hit) const
{
  Ray3f localRay{ray, _worldToLocal};
  auto d = math::inverse(localRay.direction.length());

  localRay.direction *= d;
  if (!_shape->intersect(localRay, hit))
    return false;
  hit.distance *= d;
  hit.object = this;
  return true;
}

vec3f
PrimitiveInstance::normal(const Intersection& hit) const
{
  return _normalMatrix.transform(_shape->normal(hit)).versor();
}

Bounds3f
PrimitiveInstance::bounds() const
{
  return {_shape->bounds(), _localToWorld};
}

} // end namespace cg
