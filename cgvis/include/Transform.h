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
// OVERVIEW: Transform.h
// ========
// Class definition for vis transform.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#ifndef __VisTransform_h
#define __VisTransform_h

#include "graphics/TransformableObject.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Transform: vis transform class
// =========
class Transform
{
public:
  /// Constructs an identity transform.
  Transform(TransformableObject&);

  /// Returns the position of this transform.
  const auto& position() const
  {
    return _position;
  }

  /// Returns the rotation of this transform.
  const auto& rotation() const
  {
    return _rotation;
  }

  /// Returns the Euler angles (in degrees) of this transform.
  const auto& eulerAngles() const
  {
    return _eulerAngles;
  }

  const auto& scale() const
  {
    return _scale;
  }

  /// Sets the position of this transform.
  void setPosition(const vec3f& position)
  {
    _position = position;
    update();
  }

  /// Sets the rotation of this transform.
  void setRotation(const quatf& rotation)
  {
    _eulerAngles = rotation.eulerAngles();
    _rotation = rotation;
    update();
  }

  /// Sets the Euler angles (in degrees) of this transform.
  void setEulerAngles(const vec3f& angles)
  {
    _eulerAngles = angles;
    _rotation = quatf::eulerAngles(angles);
    update();
  }

  /// Sets the scale of this transform.
  void setScale(const vec3f& scale)
  {
    _scale = scale;
    update();
  }

  /// Sets the uniform scale of this transform.
  void setScale(float scale)
  {
    setScale(vec3f{scale});
  }

  /// Sets the position, rotation, and scale of this transform.
  void set(const vec3f& position,
    const quatf& rotation,
    const vec3f& scale)
  {
    _position = position;
    _eulerAngles = (_rotation = rotation).eulerAngles();
    _scale = scale;
    update();

  }

  /// Translates this transform.
  void translate(const vec3f& t)
  {
    setPosition(_position + t);
  }

  /// Rotates this transform.
  void rotate(const vec3f& angles)
  {
    rotate(quatf::eulerAngles(angles));
  }

  /// Rotates this transform around an \c axis.
  void rotate(const vec3f& axis, float angle)
  {
    rotate(quatf{angle, axis});
  }

  /// Returns the local to world matrix of this transform.
  const auto& localToWorldMatrix() const
  {
    return _object->localToWorldMatrix();
  }

  /// Returns the world to local matrix of this transform.
  const auto& worldToLocalMatrix() const
  {
    return _object->worldToLocalMatrix();
  }

  /// Transforms \c p from local space to world space.
  vec3f transform(const vec3f& p) const
  {
    return localToWorldMatrix().transform3x4(p);
  }

  /// Transforms \c p from world space to local space.
  vec3f inverseTransform(const vec3f& p) const
  {
    return worldToLocalMatrix().transform3x4(p);
  }

  /// Transforms \c v from local space to world space.
  vec3f transformVector(const vec3f& v) const
  {
    return localToWorldMatrix().transformVector(v);
  }

  /// Transforms \c v from world space to local space.
  vec3f inverseTransformVector(const vec3f& v) const
  {
    return worldToLocalMatrix().transformVector(v);
  }

  /// Transforms \c d from world space to local space.
  vec3f transformDirection(const vec3f& d) const
  {
    return _rotation.rotate(d);
  }

  /// Reset this transform.
  void reset();

private:
  TransformableObject* _object;
  vec3f _position;
  quatf _rotation;
  vec3f _eulerAngles;
  vec3f _scale;

  void rotate(const quatf& q)
  {
    setRotation(_rotation * q);
  }

  void update()
  {
    _object->setTransform(_position, _rotation, _scale);
  }

}; // Transform

} // end namespace cg::vis

#endif // __Transform_h
