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
// OVERVIEW: Primitive.h
// ========
// Class definition for primitive.
//
// Author: Paulo Pagliosa
// Last revision: 18/01/2022

#ifndef __Primitive_h
#define __Primitive_h

#include "graphics/Material.h"
#include "graphics/Shape.h"

namespace cg
{ // begin namespace cg

class Primitive;
class Aggregate;

using PrimitiveArray = Array<Reference<Primitive>>;


/////////////////////////////////////////////////////////////////////
//
// Primitive: generic primitive model class
// =========
class Primitive abstract: public SharedObject
{
public:
  virtual const TriangleMesh* mesh() const;
  virtual bool canIntersect() const;
  virtual PrimitiveArray refine() const;
  virtual bool intersect(const Ray3f&, Intersection&) const abstract;
  virtual vec3f normal(const Intersection&) const abstract;
  virtual Bounds3f bounds() const abstract;
  virtual const Material* material() const abstract;

  auto& localToWorldMatrix() const
  {
    return _localToWorld;
  }

  auto& worldToLocalMatrix() const
  {
    return _worldToLocal;
  }

  auto& normalMatrix() const
  {
    return _normalMatrix;
  }

  virtual void setMaterial(Material*);
  virtual void setTransform(const vec3f&, const quatf&, const vec3f&);

protected:
  Reference<Material> _material;
  mat4f _localToWorld;
  mat4f _worldToLocal;
  mat3f _normalMatrix;

  // Protected constructor
  Primitive():
    _material{Material::defaultMaterial()},
    _localToWorld{1.0f},
    _worldToLocal{1.0f},
    _normalMatrix{1.0f}
  {
    // do nothing
  }

}; // Primitive


/////////////////////////////////////////////////////////////////////
//
// Aggregate: generic aggregate model class
// =========
class Aggregate abstract: public Primitive
{
public:
  vec3f normal(const Intersection&) const override;
  const Material* material() const override;

}; // Aggregate


/////////////////////////////////////////////////////////////////////
//
// PrimitiveInstance: primitive instance class
// =================
class PrimitiveInstance: public Primitive
{
public:
  PrimitiveInstance(Shape& shape, const Primitive& primitive):
    Primitive{primitive},
    _shape(&shape)
  {
    // do nothing
  }

  const TriangleMesh* mesh() const override;
  bool intersect(const Ray3f&, Intersection&) const override;
  vec3f normal(const Intersection&) const override;
  Bounds3f bounds() const override;

private:
  Reference<Shape> _shape;

}; // PrimitiveInstance

} // end namespace cg

#endif // __Primitive_h
