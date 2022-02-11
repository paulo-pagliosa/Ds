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
// Last revision: 10/02/2022

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
class Primitive: public SharedObject
{
public:
  virtual const TriangleMesh* tesselate() const;
  virtual bool canIntersect() const;

  bool intersect(const Ray3f&) const;
  bool intersect(const Ray3f&, Intersection&) const;
  virtual Material* material() const;

  virtual vec3f normal(const Intersection&) const = 0;
  virtual Bounds3f bounds() const = 0;

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
    return  _normalMatrix;
  }
 
  virtual void setMaterial(Material*);

  void setTransform(const mat4f& l2w, const mat4f& w2l)
  {
    _localToWorld = l2w;
    _worldToLocal = w2l;
    _normalMatrix = w2l.transposed();
  }

  void setTransform(const vec3f&, const quatf&, const vec3f&);

protected:
  Reference<Material> _material;
  mat4f _localToWorld;
  mat4f _worldToLocal;
  mat3f _normalMatrix;

  // Protected constructor
  Primitive():
    _material{Material::defaultMaterial()},
    _localToWorld{1},
    _worldToLocal{1},
    _normalMatrix{1}
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
class Aggregate: public Primitive
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
