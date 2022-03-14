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
// OVERVIEW: TransformableObject.h
// ========
// Class definition for generic transformable object.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#ifndef __TransformableObject_h
#define __TransformableObject_h

#include "core/SharedObject.h"
#include "math/Matrix4x4.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TransformableObject: generic transformable object class
// ===================
class TransformableObject: public virtual SharedObject
{
public:
  const auto& localToWorldMatrix() const
  {
    return _localToWorld;
  }

  const auto& worldToLocalMatrix() const
  {
    return _worldToLocal;
  }

  virtual void setTransform(const mat4f&, const mat4f&);
  virtual void setTransform(const vec3f&, const quatf&, const vec3f&);

protected:
  mat4f _localToWorld{1.0f};
  mat4f _worldToLocal{1.0f};

}; // TransformableObject

} // end namespace cg

#endif // __TransformableObject_h
