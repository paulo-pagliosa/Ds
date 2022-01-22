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
// Last revision: 20/01/2022

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
  virtual const TriangleMesh* tesselate() const;
  virtual bool canIntersect() const;

  bool intersect(const Ray3f&) const;
  bool intersect(const Ray3f&, Intersection&) const;

  virtual vec3f normal(const Intersection&) const abstract;
  virtual Bounds3f bounds() const abstract;
  virtual Material* material() const;

  const auto& localToWorldMatrix() const
  {
    return _localToWorld;
  }

  const auto& worldToLocalMatrix() const
  {
    return _worldToLocal;
  }

  auto normalMatrix() const
  {
    return mat3f{_worldToLocal}.transposed();
  }

  virtual void setMaterial(Material*);
  virtual void setTransform(const vec3f&, const quatf&, const vec3f&);

protected:
  Reference<Material> _material;
  mat4f _localToWorld;
  mat4f _worldToLocal;

  // Protected constructor
  Primitive():
    _material{Material::defaultMaterial()},
    _localToWorld{1.0f},
    _worldToLocal{1.0f}
  {
    // do nothing
  }

protected:
  virtual bool localIntersect(const Ray3f&) const;
  virtual bool localIntersect(const Ray3f&, Intersection&) const;

}; // Primitive


/////////////////////////////////////////////////////////////////////
//
// Aggregate: generic aggregate model class
// =========
class Aggregate abstract: public Primitive
{
public:
  vec3f normal(const Intersection&) const override;
  Material* material() const override;

}; // Aggregate


/////////////////////////////////////////////////////////////////////
//
// ShapeInstance: shape instance class
// =============
class ShapeInstance: public Primitive
{
public:
  ShapeInstance(const Shape& shape):
    _shape(&shape)
  {
    // do nothing
  }

  const TriangleMesh* tesselate() const override;
  bool canIntersect() const override;
  vec3f normal(const Intersection&) const override;
  Bounds3f bounds() const override;

  const Shape* shape() const
  {
    return _shape;
  }

  void setShape(const Shape& shape)
  {
    _shape = &shape;
  }

private:
  Reference<Shape> _shape;

  bool localIntersect(const Ray3f&) const override;
  bool localIntersect(const Ray3f&, Intersection&) const override;

}; // ShapeInstance

} // end namespace cg

#endif // __Primitive_h
