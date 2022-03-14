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
// OVERVIEW: Transform.cpp
// ========
// Source file for vis transform.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#include "Transform.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Transform implementation
// =========
Transform::Transform(TransformableObject& object):
  _object{&object},
  _position{0.0f},
  _rotation{quatf::identity()},
  _eulerAngles{0.0f},
  _scale{1.0f}
{
  // do nothing
}

void
Transform::reset()
{
  _position.set(0.0f);
  _rotation = quatf::identity();
  _eulerAngles.set(0.0f);
  _scale.set(1.0f);
  update();
}

} // end namespace cg::vis
