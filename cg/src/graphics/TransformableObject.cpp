//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: TransformableObject.cpp
// ========
// Source file for generic transformable object.
//
// Author: Paulo Pagliosa
// Last revision: 01/08/2023

#include "graphics/TransformableObject.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TransformableObject implementation
// ===================
void
TransformableObject::setTransform(const mat4f& l2w, const mat4f& w2l)
{
  _localToWorld = l2w;
  _worldToLocal = w2l;
}

void
TransformableObject::setTransform(const vec3f& p,
  const quatf& q,
  const vec3f& s)
{
  mat3f r{q};

  _localToWorld[0].set(r[0] * s[0]);
  _localToWorld[1].set(r[1] * s[1]);
  _localToWorld[2].set(r[2] * s[2]);
  _localToWorld[3].set(p, 1);
  r[0] *= math::inverse(s[0]);
  r[1] *= math::inverse(s[1]);
  r[2] *= math::inverse(s[2]);
  _worldToLocal[0].set(r[0][0], r[1][0], r[2][0]);
  _worldToLocal[1].set(r[0][1], r[1][1], r[2][1]);
  _worldToLocal[2].set(r[0][2], r[1][2], r[2][2]);
  _worldToLocal[3][0] = -(r[0].dot(p));
  _worldToLocal[3][1] = -(r[1].dot(p));
  _worldToLocal[3][2] = -(r[2].dot(p));
  _worldToLocal[3][3] = 1;
}

} // end namespace cg
