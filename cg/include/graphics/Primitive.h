//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2026 Paulo Pagliosa.                        |
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
// Last revision: 08/09/2026

#ifndef __Primitive_h
#define __Primitive_h

#include "graphics/Material.h"
#include "graphics/Shape.h"
#include "graphics/TransformableObject.h"

namespace cg
{ // begin namespace cg

class Primitive;
class Aggregate;

using PrimitiveArray = Array<Reference<Primitive>>;


/////////////////////////////////////////////////////////////////////
//
// Primitive: generic primitive model class
// =========
class Primitive: public TransformableObject
{
public:
  [[nodiscard]] virtual const TriangleMesh* tesselate() const;
  [[nodiscard]] virtual bool canIntersect() const;

  [[nodiscard]] bool intersect(const Ray3f&, Intersection&) const;
  [[nodiscard]] bool intersect(const Ray3f&) const;
  [[nodiscard]] virtual Material* material() const;

  [[nodiscard]] virtual vec3f normal(const Intersection&) const = 0;
  [[nodiscard]] virtual Bounds3f bounds() const = 0;

  virtual void setMaterial(Material*);

  [[nodiscard]] const auto& normalMatrix() const
  {
    return  _normalMatrix;
  }
 
  void setTransform(const mat4f&, const mat4f&) override;
  void setTransform(const vec3f&, const quatf&, const vec3f&) override;

protected:
  Reference<Material> _material;
  mat3f _normalMatrix;

  // Protected constructor
  Primitive():
    _material{Material::defaultMaterial()},
    _normalMatrix{1}
  {
    // do nothing
  }

protected:
  virtual bool localIntersect(const Ray3f&, Intersection&) const;
  virtual bool localIntersect(const Ray3f&) const;

}; // Primitive


/////////////////////////////////////////////////////////////////////
//
// Aggregate: generic aggregate model class
// =========
class Aggregate: public Primitive
{
public:
  [[nodiscard]] vec3f normal(const Intersection&) const override;
  [[nodiscard]] Material* material() const override;

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

  [[nodiscard]] const TriangleMesh* tesselate() const override;
  [[nodiscard]] bool canIntersect() const override;
  [[nodiscard]] vec3f normal(const Intersection&) const override;
  [[nodiscard]] Bounds3f bounds() const override;

  [[nodiscard]] const Shape* shape() const
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
